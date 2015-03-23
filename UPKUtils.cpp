#include "UPKUtils.h"

#include <cstring>
#include <sstream>

uint8_t PatchUPKhash [] = {0x7A, 0xA0, 0x56, 0xC9,
                           0x60, 0x5F, 0x7B, 0x31,
                           0x72, 0x5D, 0x4B, 0xC4,
                           0x7C, 0xD2, 0x4D, 0xD9 };

UPKUtils::UPKUtils(const char* filename)
{
    if (UPKUtils::Read(filename) == false && UPKFile.is_open())
    {
        UPKFile.close();
    }
}

bool UPKUtils::Read(const char* filename)
{
    UPKFileName = filename;
    if (UPKFile.is_open())
    {
        UPKFile.close();
        UPKFile.clear();
    }
    UPKFile.open(UPKFileName, std::ios::binary | std::ios::in | std::ios::out);
    if (!UPKFile.is_open())
        return false;
    return UPKUtils::Reload();
}

bool UPKUtils::Reload()
{
    if (!IsLoaded())
        return false;
    UPKFile.clear();
    UPKFile.seekg(0, std::ios::end);
    UPKFileSize = UPKFile.tellg();
    UPKFile.seekg(0);
    return UPKInfo::Read(UPKFile);
}

std::vector<char> UPKUtils::GetExportData(uint32_t idx)
{
    std::vector<char> data;
    if (idx < 1 || idx >= ExportTable.size())
        return data;
    data.resize(ExportTable[idx].SerialSize);
    UPKFile.seekg(ExportTable[idx].SerialOffset);
    UPKFile.read(data.data(), data.size());
    LastAccessedExportObjIdx = idx;
    return data;
}

void UPKUtils::SaveExportData(uint32_t idx)
{
    if (idx < 1 || idx >= ExportTable.size())
        return;
    std::string filename = ExportTable[idx].FullName + "." + ExportTable[idx].Type;
    std::vector<char> dataChunk = GetExportData(idx);
    std::ofstream out(filename.c_str(), std::ios::binary);
    out.write(dataChunk.data(), dataChunk.size());
}

/// relatively safe behavior (old realization)
bool UPKUtils::MoveExportData(uint32_t idx, uint32_t newObjectSize)
{
    if (idx < 1 || idx >= ExportTable.size())
        return false;
    std::vector<char> data = GetExportData(idx);
    UPKFile.seekg(0, std::ios::end);
    uint32_t newObjectOffset = UPKFile.tellg();
    bool isFunction = (ExportTable[idx].Type == "Function");
    if (newObjectSize > ExportTable[idx].SerialSize)
    {
        UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*8);
        UPKFile.write(reinterpret_cast<char*>(&newObjectSize), sizeof(newObjectSize));
        unsigned int diffSize = newObjectSize - data.size();
        if (isFunction == false)
        {
            for (unsigned int i = 0; i < diffSize; ++i)
                data.push_back(0x00);
        }
        else
        {
            uint32_t oldMemSize = 0;
            uint32_t oldFileSize = 0;
            memcpy(&oldMemSize, data.data() + 0x28, 0x4);  /// copy function memory size
            memcpy(&oldFileSize, data.data() + 0x2C, 0x4); /// and file size
            uint32_t newMemSize = oldMemSize + diffSize;   /// find new sizes
            uint32_t newFileSize = oldFileSize + diffSize;
            uint32_t headSize = 0x30 + oldFileSize - 1;    /// head size (all data before 0x53)
            uint32_t tailSize = ExportTable[idx].SerialSize - headSize; /// tail size (0x53 and all data after)
            std::vector<char> newData(newObjectSize);
            memset(newData.data(), 0x0B, newObjectSize);     /// fill new data with 0x0B
            memcpy(newData.data(), data.data(), headSize);   /// copy all data before 0x53
            memcpy(newData.data() + 0x28, &newMemSize, 0x4); /// set new memory size
            memcpy(newData.data() + 0x2C, &newFileSize, 0x4);/// and file size
            memcpy(newData.data() + headSize + diffSize, data.data() + headSize, tailSize); /// copy 0x53 and all data after
            data = newData;
        }
    }
    UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*9);
    UPKFile.write(reinterpret_cast<char*>(&newObjectOffset), sizeof(newObjectOffset));
    UPKFile.seekp(newObjectOffset);
    UPKFile.write(data.data(), data.size());
    /// write backup info
    UPKFile.write(reinterpret_cast<char*>(&PatchUPKhash[0]), 16);
    UPKFile.write(reinterpret_cast<char*>(&ExportTable[idx].SerialSize), sizeof(ExportTable[idx].SerialSize));
    UPKFile.write(reinterpret_cast<char*>(&ExportTable[idx].SerialOffset), sizeof(ExportTable[idx].SerialOffset));
    /// reload package
    UPKUtils::Reload();
    return true;
}

bool UPKUtils::UndoMoveExportData(uint32_t idx)
{
    if (idx < 1 || idx >= ExportTable.size())
        return false;
    UPKFile.seekg(ExportTable[idx].SerialOffset + ExportTable[idx].SerialSize);
    uint8_t readHash [16];
    UPKFile.read(reinterpret_cast<char*>(&readHash[0]), 16);
    if (memcmp(readHash, PatchUPKhash, 16) != 0)
        return false;
    uint32_t oldObjectFileSize, oldObjectOffset;
    UPKFile.read(reinterpret_cast<char*>(&oldObjectFileSize), sizeof(oldObjectFileSize));
    UPKFile.read(reinterpret_cast<char*>(&oldObjectOffset), sizeof(oldObjectOffset));
    UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*8);
    UPKFile.write(reinterpret_cast<char*>(&oldObjectFileSize), sizeof(oldObjectFileSize));
    UPKFile.write(reinterpret_cast<char*>(&oldObjectOffset), sizeof(oldObjectOffset));
    /// reload package
    UPKUtils::Reload();
    return true;
}

std::vector<char> UPKUtils::GetResizedDataChunk(uint32_t idx, int newObjectSize, int resizeAt)
{
    std::vector<char> data;
    if (idx < 1 || idx >= ExportTable.size())
        return data;
    /// get export object serial data
    data = GetExportData(idx);
    /// if object needs resizing
    if (newObjectSize > 0 && (unsigned)newObjectSize != data.size())
    {
        /// if resizing occurs in the middle of an object
        if (resizeAt > 0 && resizeAt < newObjectSize)
        {
            int diff = newObjectSize - data.size();
            std::vector<char> newData(newObjectSize);
            memset(newData.data(), 0, newObjectSize); /// fill with zeros
            memcpy(newData.data(), data.data(), resizeAt); /// copy head
            if (diff > 0) /// if expanding
                memcpy(newData.data() + resizeAt + diff, data.data() + resizeAt, data.size() - resizeAt);
            else /// if shrinking
                memcpy(newData.data() + resizeAt, data.data() + resizeAt - diff, data.size() - (resizeAt - diff));
            data = newData;
        }
        else
        {
            data.resize(newObjectSize, 0);
        }
    }
    return data;
}

bool UPKUtils::MoveResizeObject(uint32_t idx, int newObjectSize, int resizeAt)
{
    if (idx < 1 || idx >= ExportTable.size())
        return false;
    std::vector<char> data = GetResizedDataChunk(idx, newObjectSize, resizeAt);
    /// move write pointer to the end of file
    UPKFile.seekg(0, std::ios::end);
    uint32_t newObjectOffset = UPKFile.tellg();
    /// if object needs resizing
    if (ExportTable[idx].SerialSize != data.size())
    {
        /// write new SerialSize to ExportTable entry
        UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*8);
        UPKFile.write(reinterpret_cast<char*>(&newObjectSize), sizeof(newObjectSize));
    }
    /// write new SerialOffset to ExportTable entry
    UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*9);
    UPKFile.write(reinterpret_cast<char*>(&newObjectOffset), sizeof(newObjectOffset));
    /// write new SerialData
    if (data.size() > 0)
    {
        UPKFile.seekp(newObjectOffset);
        UPKFile.write(data.data(), data.size());
    }
    /// write backup info
    UPKFile.write(reinterpret_cast<char*>(&PatchUPKhash[0]), 16);
    UPKFile.write(reinterpret_cast<char*>(&ExportTable[idx].SerialSize), sizeof(ExportTable[idx].SerialSize));
    UPKFile.write(reinterpret_cast<char*>(&ExportTable[idx].SerialOffset), sizeof(ExportTable[idx].SerialOffset));
    /// reload package
    UPKUtils::Reload();
    return true;
}

bool UPKUtils::UndoMoveResizeObject(uint32_t idx)
{
    return UndoMoveExportData(idx);
}

std::string UPKUtils::Deserialize(UObjectReference ObjRef, bool TryUnsafe, bool QuickMode)
{
    if (ObjRef < 1 || ObjRef >= (int)ExportTable.size())
        return "Bad object reference!\n";
    UObject* Obj;
    if (ExportTable[ObjRef].ObjectFlagsH & (uint32_t)UObjectFlagsH::PropertiesObject)
    {
        Obj = UObjectFactory::Create(GlobalType::UObject);
    }
    else
    {
        Obj = UObjectFactory::Create(ExportTable[ObjRef].Type);
    }
    if (Obj == nullptr)
        return "Can't create object of given type!\n";
    std::string res;
    UPKFile.seekg(ExportTable[ObjRef].SerialOffset);
    Obj->SetRef(ObjRef);
    Obj->SetUnsafe(TryUnsafe);
    Obj->SetQuickMode(QuickMode);
    res = Obj->Deserialize(UPKFile, *dynamic_cast<UPKInfo*>(this));
    delete Obj;
    return res;
}

bool UPKUtils::CheckValidFileOffset(size_t offset)
{
    if (IsLoaded() == false)
    {
        return false;
    }
    /// does not allow to change package signature and version
    return (offset >= 8 && offset < UPKFileSize);
}

bool UPKUtils::WriteExportData(uint32_t idx, std::vector<char> data, std::vector<char> *backupData)
{
    if (idx < 1 || idx >= ExportTable.size())
        return false;
    if (!IsLoaded())
        return false;
    if (ExportTable[idx].SerialSize != data.size())
        return false;
    if (backupData != nullptr)
    {
        backupData->clear();
        backupData->resize(data.size());
        UPKFile.seekg(ExportTable[idx].SerialOffset);
        UPKFile.read(backupData->data(), backupData->size());
    }
    UPKFile.seekp(ExportTable[idx].SerialOffset);
    UPKFile.write(data.data(), data.size());
    return true;
}

bool UPKUtils::WriteNameTableName(uint32_t idx, std::string name)
{
    if (idx < 1 || idx >= NameTable.size())
        return false;
    if (!IsLoaded())
        return false;
    if ((unsigned)(NameTable[idx].NameLength - 1) != name.length())
        return false;
    UPKFile.seekp(NameTable[idx].EntryOffset + sizeof(NameTable[idx].NameLength));
    UPKFile.write(name.c_str(), name.length());
    /// reload package
    UPKUtils::Reload();
    return true;
}

bool UPKUtils::WriteData(size_t offset, std::vector<char> data, std::vector<char> *backupData)
{
    if (!CheckValidFileOffset(offset))
        return false;
    if (backupData != nullptr)
    {
        backupData->clear();
        backupData->resize(data.size());
        UPKFile.seekg(offset);
        UPKFile.read(backupData->data(), backupData->size());
    }
    UPKFile.seekp(offset);
    UPKFile.write(data.data(), data.size());
    /// if changed header
    if (offset < Summary.SerialOffset)
    {
        /// reload package
        UPKUtils::Reload();
    }
    return true;
}

std::vector<char> UPKUtils::GetBulkData(size_t offset, std::vector<char> data)
{
    UBulkDataMirror DataMirror;
    DataMirror.SetBulkData(data);
    DataMirror.SetFileOffset(offset + DataMirror.GetBulkDataRelOffset());
    std::string mirrorStr = DataMirror.Serialize();
    std::vector<char> mirrorVec(mirrorStr.size());
    memcpy(mirrorVec.data(), mirrorStr.data(), mirrorStr.size());
    return mirrorVec;
}

size_t UPKUtils::FindDataChunk(std::vector<char> data, size_t beg, size_t limit)
{
    if (limit != 0 && (limit - beg + 1 < data.size() || limit < beg))
        return 0;
    size_t offset = 0, idx = beg;
    bool found = false;
    std::vector<char> fileBuf((limit == 0 ? UPKFileSize : limit) - beg + 1);

    UPKFile.seekg(beg);
    UPKFile.read(fileBuf.data(), fileBuf.size());

    char* pFileBuf = fileBuf.data();
    char* pData = data.data();

    for (char* p = pFileBuf; p != pFileBuf + fileBuf.size() - data.size() + 1; ++p)
    {
        if (memcmp(p, pData, data.size()) == 0)
        {
            found = true;
            offset = idx;
            break;
        }
        ++idx;
    }

    if (found == false)
        offset = 0;

    UPKFile.clear();
    UPKFile.seekg(0);
    return offset;
}

size_t UPKUtils::GetScriptSize(uint32_t idx)
{
    if (idx < 1 || idx >= ExportTable.size())
        return 0;
    UObject* Obj;
    Obj = UObjectFactory::Create(ExportTable[idx].Type);
    if (Obj == nullptr)
        return 0;
    UPKFile.seekg(ExportTable[idx].SerialOffset);
    Obj->SetRef(idx);
    Obj->SetUnsafe(false);
    Obj->SetQuickMode(true);
    Obj->Deserialize(UPKFile, *dynamic_cast<UPKInfo*>(this));
    if (Obj->IsStructure() == false)
    {
        delete Obj;
        return 0;
    }
    UStruct* St = dynamic_cast<UStruct*>(Obj);
    if (St == nullptr)
    {
        delete Obj;
        return 0;
    }
    size_t ScriptSize = St->GetScriptSerialSize();
    delete Obj;
    return ScriptSize;
}

size_t UPKUtils::GetScriptMemSize(uint32_t idx)
{
    if (idx < 1 || idx >= ExportTable.size())
        return 0;
    UObject* Obj;
    Obj = UObjectFactory::Create(ExportTable[idx].Type);
    if (Obj == nullptr)
        return 0;
    UPKFile.seekg(ExportTable[idx].SerialOffset);
    Obj->SetRef(idx);
    Obj->SetUnsafe(false);
    Obj->SetQuickMode(true);
    Obj->Deserialize(UPKFile, *dynamic_cast<UPKInfo*>(this));
    if (Obj->IsStructure() == false)
    {
        delete Obj;
        return 0;
    }
    UStruct* St = dynamic_cast<UStruct*>(Obj);
    if (St == nullptr)
    {
        delete Obj;
        return 0;
    }
    size_t ScriptMemSize = St->GetScriptMemorySize();
    delete Obj;
    return ScriptMemSize;
}

size_t UPKUtils::GetScriptRelOffset(uint32_t idx)
{
    if (idx < 1 || idx >= ExportTable.size())
        return 0;
    UObject* Obj;
    Obj = UObjectFactory::Create(ExportTable[idx].Type);
    if (Obj == nullptr)
        return 0;
    UPKFile.seekg(ExportTable[idx].SerialOffset);
    Obj->SetRef(idx);
    Obj->SetUnsafe(false);
    Obj->SetQuickMode(true);
    Obj->Deserialize(UPKFile, *dynamic_cast<UPKInfo*>(this));
    if (Obj->IsStructure() == false)
    {
        delete Obj;
        return 0;
    }
    UStruct* St = dynamic_cast<UStruct*>(Obj);
    if (St == nullptr)
    {
        delete Obj;
        return 0;
    }
    size_t ScriptRelOffset = St->GetScriptOffset() - ExportTable[idx].SerialOffset;
    delete Obj;
    return ScriptRelOffset;
}

bool UPKUtils::ResizeInPlace(uint32_t idx, int newObjectSize, int resizeAt)
{
    if (!UPKFile.good())
    {
        return false;
    }
    if (idx < 1 || idx >= ExportTable.size())
        return false;
    std::vector<char> data = GetResizedDataChunk(idx, newObjectSize, resizeAt);
    int diffSize = data.size() - ExportTable[idx].SerialSize;
    /// increase offsets
    for (unsigned i = 1; i <= Summary.ExportCount; ++i)
    {
        if (i != idx && ExportTable[i].SerialOffset > ExportTable[idx].SerialOffset)
        {
            ExportTable[i].SerialOffset += diffSize;
        }
    }
    /// backup serialized export data into memory
    UPKFile.clear();
    UPKFile.seekg(Summary.SerialOffset);
    std::vector<char> serializedDataBeforeIdx(ExportTable[idx].SerialOffset - UPKFile.tellg());
    if (serializedDataBeforeIdx.size() > 0)
    {
        UPKFile.read(serializedDataBeforeIdx.data(), serializedDataBeforeIdx.size());
    }
    UPKFile.seekg(ExportTable[idx].SerialOffset + ExportTable[idx].SerialSize);
    std::vector<char> serializedDataAfterIdx(UPKFileSize - UPKFile.tellg());
    if (serializedDataAfterIdx.size() > 0)
    {
        UPKFile.read(serializedDataAfterIdx.data(), serializedDataAfterIdx.size());
    }
    /// save new serial size
    ExportTable[idx].SerialSize = newObjectSize;
    /// serialize header
    std::vector<char> serializedHeader = SerializeHeader();
    /// rewrite package
    UPKFile.close();
    UPKFile.open(UPKFileName.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
    /// write serialized header
    UPKFile.write(serializedHeader.data(), serializedHeader.size());
    /// write serialized export data before resized object
    if (serializedDataBeforeIdx.size() > 0)
    {
        UPKFile.write(serializedDataBeforeIdx.data(), serializedDataBeforeIdx.size());
    }
    /// write resized export object data
    UPKFile.write(data.data(), data.size());
    /// write serialized export data after resized object
    if (serializedDataAfterIdx.size() > 0)
    {
        UPKFile.write(serializedDataAfterIdx.data(), serializedDataAfterIdx.size());
    }
    /// reload package
    UPKUtils::Read(UPKFileName.c_str());
    return true;
}

bool UPKUtils::AddNameEntry(FNameEntry Entry)
{
    if (!UPKFile.good())
    {
        return false;
    }
    size_t oldSerialOffset = Summary.SerialOffset;
    /// increase header size
    Summary.HeaderSize += Entry.EntrySize;
    /// add entry
    ++Summary.NameCount;
    NameTable.push_back(Entry);
    /// increase offsets
    Summary.ImportOffset += Entry.EntrySize;
    Summary.ExportOffset += Entry.EntrySize;
    Summary.DependsOffset += Entry.EntrySize;
    Summary.SerialOffset += Entry.EntrySize;
    for (unsigned i = 1; i <= Summary.ExportCount; ++i)
    {
        ExportTable[i].SerialOffset += Entry.EntrySize;
    }
    /// backup serialized export data into memory
    UPKFile.clear();
    UPKFile.seekg(oldSerialOffset);
    std::vector<char> serializedData(UPKFileSize - oldSerialOffset);
    UPKFile.read(serializedData.data(), serializedData.size());
    /// rewrite package
    UPKFile.seekp(0);
    /// serialize header
    std::vector<char> serializedHeader = SerializeHeader();
    /// write serialized header
    UPKFile.write(serializedHeader.data(), serializedHeader.size());
    /// write serialized export data
    UPKFile.write(serializedData.data(), serializedData.size());
    /// reload package
    UPKUtils::Reload();
    return true;
}

bool UPKUtils::AddImportEntry(FObjectImport Entry)
{
    if (!UPKFile.good())
    {
        return false;
    }
    size_t oldSerialOffset = Summary.SerialOffset;
    /// increase header size
    Summary.HeaderSize += Entry.EntrySize;
    /// add entry
    ++Summary.ImportCount;
    ImportTable.push_back(Entry);
    /// increase offsets
    Summary.ExportOffset += Entry.EntrySize;
    Summary.DependsOffset += Entry.EntrySize;
    Summary.SerialOffset += Entry.EntrySize;
    for (unsigned i = 1; i <= Summary.ExportCount; ++i)
    {
        ExportTable[i].SerialOffset += Entry.EntrySize;
    }
    /// backup serialized export data into memory
    UPKFile.clear();
    UPKFile.seekg(oldSerialOffset);
    std::vector<char> serializedData(UPKFileSize - oldSerialOffset);
    UPKFile.read(serializedData.data(), serializedData.size());
    /// rewrite package
    UPKFile.seekp(0);
    /// serialize header
    std::vector<char> serializedHeader = SerializeHeader();
    /// write serialized header
    UPKFile.write(serializedHeader.data(), serializedHeader.size());
    /// write serialized export data
    UPKFile.write(serializedData.data(), serializedData.size());
    /// reload package
    UPKUtils::Reload();
    return true;
}

bool UPKUtils::AddExportEntry(FObjectExport Entry)
{
    if (!UPKFile.good())
    {
        return false;
    }
    unsigned oldExportCount = Summary.ExportCount;
    size_t oldSerialOffset = Summary.SerialOffset;
    /// increase header size
    Summary.HeaderSize += Entry.EntrySize;
    /// add entry
    ++Summary.ExportCount;
    if (Entry.SerialSize < 16) /// PrevObject + NoneIdx + NextRef
    {
        Entry.SerialSize = 16;
    }
    Entry.SerialOffset = UPKFileSize + Entry.EntrySize;
    ExportTable.push_back(Entry);
    /// increase offsets
    Summary.DependsOffset += Entry.EntrySize;
    Summary.SerialOffset += Entry.EntrySize;
    for (unsigned i = 1; i <= oldExportCount; ++i)
    {
        ExportTable[i].SerialOffset += Entry.EntrySize;
    }
    /// backup serialized export data into memory
    UPKFile.clear();
    UPKFile.seekg(oldSerialOffset);
    std::vector<char> serializedData(UPKFileSize - oldSerialOffset);
    UPKFile.read(serializedData.data(), serializedData.size());
    /// rewrite package
    UPKFile.seekp(0);
    /// serialize header
    std::vector<char> serializedHeader = SerializeHeader();
    /// write serialized header
    UPKFile.write(serializedHeader.data(), serializedHeader.size());
    /// write serialized export data
    UPKFile.write(serializedData.data(), serializedData.size());
    /// write new export serialized data
    std::vector<char> serializedEntry(Entry.SerialSize);
    UObjectReference PrevObjRef = oldExportCount;
    memcpy(serializedEntry.data(), reinterpret_cast<char*>(&PrevObjRef), sizeof(PrevObjRef));
    memcpy(serializedEntry.data() + sizeof(PrevObjRef), reinterpret_cast<char*>(&NoneIdx), sizeof(NoneIdx));
    UPKFile.write(serializedEntry.data(), serializedEntry.size());
    /// reload package
    UPKUtils::Reload();
    /// link export object to owner
    LinkChild(Entry.OwnerRef, Summary.ExportCount);
    return true;
}

bool UPKUtils::LinkChild(UObjectReference OwnerRef, UObjectReference ChildRef)
{
    if (OwnerRef < 1 || OwnerRef >= (int)ExportTable.size())
        return false;
    UObject* Obj;
    /// deserialize owner object to get first child
    Obj = UObjectFactory::Create(ExportTable[OwnerRef].Type);
    if (Obj == nullptr || Obj->IsStructure() == false)
    {
        return false;
    }
    UPKFile.seekg(ExportTable[OwnerRef].SerialOffset);
    Obj->SetRef(OwnerRef);
    Obj->SetUnsafe(false);
    Obj->SetQuickMode(true);
    Obj->Deserialize(UPKFile, *dynamic_cast<UPKInfo*>(this));
    UStruct* StructObj = dynamic_cast<UStruct*>(Obj);
    if (StructObj == nullptr)
    {
        delete Obj;
        return false;
    }
    UObjectReference FirstChildRef = StructObj->GetFirstChildRef();
    /// owner has no children
    if (FirstChildRef == 0)
    {
        /// link child to owner
        UPKFile.seekg(StructObj->GetFirstChildRefOffset());
        UPKFile.write(reinterpret_cast<char*>(&ChildRef), sizeof(ChildRef));
        delete Obj;
        return true;
    }
    delete Obj;
    /// find last child
    UObjectReference NextRef = FirstChildRef;
    size_t LastRefOffset = 0;
    while (NextRef != 0)
    {
        Obj = UObjectFactory::Create(GlobalType::UField);
        if (Obj == nullptr)
        {
            return false;
        }
        UPKFile.seekg(ExportTable[NextRef].SerialOffset);
        Obj->SetRef(NextRef);
        Obj->SetUnsafe(false);
        Obj->SetQuickMode(true);
        Obj->Deserialize(UPKFile, *dynamic_cast<UPKInfo*>(this));
        UField* FieldObj = dynamic_cast<UField*>(Obj);
        if (FieldObj == nullptr)
        {
            delete Obj;
            return false;
        }
        NextRef = FieldObj->GetNextRef();
        LastRefOffset = FieldObj->GetNextRefOffset();
        delete Obj;

    }
    /// link new child to last child
    UPKFile.seekg(LastRefOffset);
    UPKFile.write(reinterpret_cast<char*>(&ChildRef), sizeof(ChildRef));
    return true;
}

bool UPKUtils::Deserialize(FNameEntry& entry, std::vector<char>& data)
{
    if (data.size() < 12)
    {
        return false;
    }
    std::stringstream ss;
    ss.write(data.data(), data.size());
    ss.read(reinterpret_cast<char*>(&entry.NameLength), 4);
    if (data.size() != 12U + entry.NameLength)
    {
        return false;
    }
    if (entry.NameLength > 0)
    {
        getline(ss, entry.Name, '\0');
    }
    else
    {
        entry.Name = "";
    }
    ss.read(reinterpret_cast<char*>(&entry.NameFlagsL), 4);
    ss.read(reinterpret_cast<char*>(&entry.NameFlagsH), 4);
    /// memory variables
    entry.EntrySize = data.size();
    return true;
}

bool UPKUtils::Deserialize(FObjectImport& entry, std::vector<char>& data)
{
    if (data.size() != 28)
    {
        return false;
    }
    std::stringstream ss;
    ss.write(data.data(), data.size());
    ss.read(reinterpret_cast<char*>(&entry.PackageIdx), sizeof(entry.PackageIdx));
    ss.read(reinterpret_cast<char*>(&entry.TypeIdx), sizeof(entry.TypeIdx));
    ss.read(reinterpret_cast<char*>(&entry.OwnerRef), sizeof(entry.OwnerRef));
    ss.read(reinterpret_cast<char*>(&entry.NameIdx), sizeof(entry.NameIdx));
    /// memory variables
    entry.EntrySize = data.size();
    entry.Name = IndexToName(entry.NameIdx);
    entry.FullName = entry.Name;
    if (entry.OwnerRef != 0)
    {
        entry.FullName = ResolveFullName(entry.OwnerRef) + "." + entry.Name;
    }
    entry.Type = IndexToName(entry.TypeIdx);
    if (entry.Type == "")
    {
        entry.Type = "Class";
    }
    return true;
}

bool UPKUtils::Deserialize(FObjectExport& entry, std::vector<char>& data)
{
    if (data.size() < 68)
    {
        return false;
    }
    std::stringstream ss;
    ss.write(data.data(), data.size());
    ss.read(reinterpret_cast<char*>(&entry.TypeRef), sizeof(entry.TypeRef));
    ss.read(reinterpret_cast<char*>(&entry.ParentClassRef), sizeof(entry.ParentClassRef));
    ss.read(reinterpret_cast<char*>(&entry.OwnerRef), sizeof(entry.OwnerRef));
    ss.read(reinterpret_cast<char*>(&entry.NameIdx), sizeof(entry.NameIdx));
    ss.read(reinterpret_cast<char*>(&entry.ArchetypeRef), sizeof(entry.ArchetypeRef));
    ss.read(reinterpret_cast<char*>(&entry.ObjectFlagsH), sizeof(entry.ObjectFlagsH));
    ss.read(reinterpret_cast<char*>(&entry.ObjectFlagsL), sizeof(entry.ObjectFlagsL));
    ss.read(reinterpret_cast<char*>(&entry.SerialSize), sizeof(entry.SerialSize));
    ss.read(reinterpret_cast<char*>(&entry.SerialOffset), sizeof(entry.SerialOffset));
    ss.read(reinterpret_cast<char*>(&entry.ExportFlags), sizeof(entry.ExportFlags));
    ss.read(reinterpret_cast<char*>(&entry.NetObjectCount), sizeof(entry.NetObjectCount));
    ss.read(reinterpret_cast<char*>(&entry.GUID), sizeof(entry.GUID));
    ss.read(reinterpret_cast<char*>(&entry.Unknown1), sizeof(entry.Unknown1));
    entry.NetObjects.resize(entry.NetObjectCount);
    if (data.size() != 68 + entry.NetObjectCount)
    {
        return false;
    }
    if (entry.NetObjectCount > 0)
    {
        ss.read(reinterpret_cast<char*>(entry.NetObjects.data()), entry.NetObjects.size()*4);
    }
    /// memory variables
    entry.EntrySize = data.size();
    entry.Name = IndexToName(entry.NameIdx);
    entry.FullName = entry.Name;
    if (entry.OwnerRef != 0)
    {
        entry.FullName = ResolveFullName(entry.OwnerRef) + "." + entry.Name;
    }
    entry.Type = ObjRefToName(entry.TypeRef);
    if (entry.Type == "")
    {
        entry.Type = "Class";
    }
    return true;
}

std::vector<char> UPKUtils::SerializeHeader()
{
    std::stringstream ss;
    ss.write(reinterpret_cast<char*>(&Summary.Signature), 4);
    int32_t Ver = (Summary.LicenseeVersion << 16) + Summary.Version;
    ss.write(reinterpret_cast<char*>(&Ver), 4);
    ss.write(reinterpret_cast<char*>(&Summary.HeaderSize), 4);
    ss.write(reinterpret_cast<char*>(&Summary.FolderNameLength), 4);
    if (Summary.FolderNameLength > 0)
    {
        ss.write(Summary.FolderName.c_str(), Summary.FolderNameLength);
    }
    ss.write(reinterpret_cast<char*>(&Summary.PackageFlags), 4);
    ss.write(reinterpret_cast<char*>(&Summary.NameCount), 4);
    ss.write(reinterpret_cast<char*>(&Summary.NameOffset), 4);
    ss.write(reinterpret_cast<char*>(&Summary.ExportCount), 4);
    ss.write(reinterpret_cast<char*>(&Summary.ExportOffset), 4);
    ss.write(reinterpret_cast<char*>(&Summary.ImportCount), 4);
    ss.write(reinterpret_cast<char*>(&Summary.ImportOffset), 4);
    ss.write(reinterpret_cast<char*>(&Summary.DependsOffset), 4);
    ss.write(reinterpret_cast<char*>(&Summary.SerialOffset), 4);
    ss.write(reinterpret_cast<char*>(&Summary.Unknown2), 4);
    ss.write(reinterpret_cast<char*>(&Summary.Unknown3), 4);
    ss.write(reinterpret_cast<char*>(&Summary.Unknown4), 4);
    ss.write(reinterpret_cast<char*>(&Summary.GUID), sizeof(Summary.GUID));
    ss.write(reinterpret_cast<char*>(&Summary.GenerationsCount), 4);
    for (unsigned i = 0; i < Summary.GenerationsCount; ++i)
    {
        FGenerationInfo Entry = Summary.Generations[i];
        ss.write(reinterpret_cast<char*>(&Entry.ExportCount), 4);
        ss.write(reinterpret_cast<char*>(&Entry.NameCount), 4);
        ss.write(reinterpret_cast<char*>(&Entry.NetObjectCount), 4);
    }
    ss.write(reinterpret_cast<char*>(&Summary.EngineVersion), 4);
    ss.write(reinterpret_cast<char*>(&Summary.CookerVersion), 4);
    ss.write(reinterpret_cast<char*>(&Summary.CompressionFlags), 4);
    ss.write(reinterpret_cast<char*>(&Summary.NumCompressedChunks), 4);
    for (unsigned i = 0; i < Summary.NumCompressedChunks; ++i)
    {
        FCompressedChunk CompressedChunk = Summary.CompressedChunks[i];
        ss.write(reinterpret_cast<char*>(&CompressedChunk.UncompressedOffset), 4);
        ss.write(reinterpret_cast<char*>(&CompressedChunk.UncompressedSize), 4);
        ss.write(reinterpret_cast<char*>(&CompressedChunk.CompressedOffset), 4);
        ss.write(reinterpret_cast<char*>(&CompressedChunk.CompressedSize), 4);
    }
    if (Summary.UnknownDataChunk.size() > 0)
    {
        ss.write(Summary.UnknownDataChunk.data(), Summary.UnknownDataChunk.size());
    }
    for (unsigned i = 0; i < Summary.NameCount; ++i)
    {
        FNameEntry Entry = NameTable[i];
        ss.write(reinterpret_cast<char*>(&Entry.NameLength), 4);
        if (Entry.NameLength > 0)
        {
            ss.write(Entry.Name.c_str(), Entry.NameLength);
        }
        ss.write(reinterpret_cast<char*>(&Entry.NameFlagsL), 4);
        ss.write(reinterpret_cast<char*>(&Entry.NameFlagsH), 4);
    }
    for (unsigned i = 1; i <= Summary.ImportCount; ++i)
    {
        FObjectImport Entry = ImportTable[i];
        ss.write(reinterpret_cast<char*>(&Entry.PackageIdx), sizeof(Entry.PackageIdx));
        ss.write(reinterpret_cast<char*>(&Entry.TypeIdx), sizeof(Entry.TypeIdx));
        ss.write(reinterpret_cast<char*>(&Entry.OwnerRef), sizeof(Entry.OwnerRef));
        ss.write(reinterpret_cast<char*>(&Entry.NameIdx), sizeof(Entry.NameIdx));
    }
    for (unsigned i = 1; i <= Summary.ExportCount; ++i)
    {
        FObjectExport Entry = ExportTable[i];
        ss.write(reinterpret_cast<char*>(&Entry.TypeRef), sizeof(Entry.TypeRef));
        ss.write(reinterpret_cast<char*>(&Entry.ParentClassRef), sizeof(Entry.ParentClassRef));
        ss.write(reinterpret_cast<char*>(&Entry.OwnerRef), sizeof(Entry.OwnerRef));
        ss.write(reinterpret_cast<char*>(&Entry.NameIdx), sizeof(Entry.NameIdx));
        ss.write(reinterpret_cast<char*>(&Entry.ArchetypeRef), sizeof(Entry.ArchetypeRef));
        ss.write(reinterpret_cast<char*>(&Entry.ObjectFlagsH), sizeof(Entry.ObjectFlagsH));
        ss.write(reinterpret_cast<char*>(&Entry.ObjectFlagsL), sizeof(Entry.ObjectFlagsL));
        ss.write(reinterpret_cast<char*>(&Entry.SerialSize), sizeof(Entry.SerialSize));
        ss.write(reinterpret_cast<char*>(&Entry.SerialOffset), sizeof(Entry.SerialOffset));
        ss.write(reinterpret_cast<char*>(&Entry.ExportFlags), sizeof(Entry.ExportFlags));
        ss.write(reinterpret_cast<char*>(&Entry.NetObjectCount), sizeof(Entry.NetObjectCount));
        ss.write(reinterpret_cast<char*>(&Entry.GUID), sizeof(Entry.GUID));
        ss.write(reinterpret_cast<char*>(&Entry.Unknown1), sizeof(Entry.Unknown1));
        if (Entry.NetObjectCount > 0)
        {
            ss.write(reinterpret_cast<char*>(Entry.NetObjects.data()), Entry.NetObjects.size()*4);
        }
    }
    if (DependsBuf.size() > 0)
    {
        ss.write(DependsBuf.data(), DependsBuf.size());
    }
    std::vector<char> ret(ss.tellp());
    ss.read(ret.data(), ret.size());
    return ret;
}
