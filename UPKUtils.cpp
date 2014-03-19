#include "UPKUtils.h"

#include <cstring>

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
    if (UPKFile.is_open())
    {
        UPKFile.close();
        UPKFile.clear();
    }
    UPKFile.open(filename, std::ios::binary | std::ios::in | std::ios::out);
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

/// relatively safe behaviour
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

bool UPKUtils::MoveResizeObject(uint32_t idx, int newObjectSize, int resizeAt)
{
    if (idx < 1 || idx >= ExportTable.size())
        return false;
    std::vector<char> data = GetExportData(idx);
    UPKFile.seekg(0, std::ios::end);
    uint32_t newObjectOffset = UPKFile.tellg();
    /// if object needs resizing
    if (newObjectSize > 0 && (unsigned)newObjectSize != data.size())
    {
        UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*8);
        UPKFile.write(reinterpret_cast<char*>(&newObjectSize), sizeof(newObjectSize));
        /// if resizing occurs at the middle of an object
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
    UPKFile.seekp(ExportTable[idx].EntryOffset + sizeof(uint32_t)*9);
    UPKFile.write(reinterpret_cast<char*>(&newObjectOffset), sizeof(newObjectOffset));
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

/*bool UPKUtils::AddNameListEntry(NameListEntry entry)
{
    std::ofstream newUPK((UPKfilename + ".new").c_str(), std::ios::binary);
    assert(newUPK.is_open() == true);
    uint32_t newEntrySize = 4 + entry.NameLength + 8;
    upkFile.seekg(0);
    newUPK.seekp(0);
    uint32_t offset = 0;
    for (; offset < headerSizeOffset; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newHeaderSize;
    upkFile.read(reinterpret_cast<char*>(&newHeaderSize), sizeof(newHeaderSize));
    newHeaderSize += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newHeaderSize), sizeof(newHeaderSize));
    for (offset = upkFile.tellg(); offset < NameCountOffset; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newNameCount;
    upkFile.read(reinterpret_cast<char*>(&newNameCount), sizeof(newNameCount));
    ++newNameCount;
    newUPK.write(reinterpret_cast<char*>(&newNameCount), sizeof(newNameCount));
    for (int i = 0; i < 8; ++i)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newExportOffset;
    upkFile.read(reinterpret_cast<char*>(&newExportOffset), sizeof(newExportOffset));
    newExportOffset += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newExportOffset), sizeof(newExportOffset));
    for (int i = 0; i < 4; ++i)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newImportOffset;
    upkFile.read(reinterpret_cast<char*>(&newImportOffset), sizeof(newImportOffset));
    newImportOffset += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newImportOffset), sizeof(newImportOffset));
    uint32_t newDependsOffset;
    upkFile.read(reinterpret_cast<char*>(&newDependsOffset), sizeof(newDependsOffset));
    newDependsOffset += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newDependsOffset), sizeof(newDependsOffset));
    uint32_t newUnknown1;
    upkFile.read(reinterpret_cast<char*>(&newUnknown1), sizeof(newUnknown1));
    newUnknown1 += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newUnknown1), sizeof(newUnknown1));
    for (offset = upkFile.tellg(); offset < NameListEndOffset; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    newUPK.write(reinterpret_cast<char*>(&entry.NameLength), sizeof(entry.NameLength));
    newUPK.write(entry.NameString.c_str(), entry.NameLength);
    newUPK.write(reinterpret_cast<char*>(&entry.NameFlagsH), sizeof(entry.NameFlagsH));
    newUPK.write(reinterpret_cast<char*>(&entry.NameFlagsL), sizeof(entry.NameFlagsL));
    for (offset = upkFile.tellg(); offset < header.ExportOffset; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    for (unsigned i = 0; i < header.ExportCount; ++i)
    {
        ObjectListEntry EntryToRead;

        upkFile.read(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));

        EntryToRead.DataOffset += newEntrySize;

        newUPK.write(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));

        for (unsigned j = 0; j < EntryToRead.NumAdditionalFields * sizeof(uint32_t); ++j)
        {
            uint8_t ch = upkFile.get();
            newUPK.put(ch);
        }
    }
    for (offset = upkFile.tellg(); offset < upkFileSize; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    newUPK.close();
    open((UPKfilename + ".new").c_str());
    return true;
}

bool UPKUtils::AddObjectListEntry(ObjectListEntry entry)
{
    std::ofstream newUPK((UPKfilename + ".new2").c_str(), std::ios::binary);
    assert(newUPK.is_open() == true);
    uint32_t newEntrySize = 17 * 4 + entry.NumAdditionalFields * 4;
    upkFile.seekg(0);
    newUPK.seekp(0);
    uint32_t offset = 0;
    for (; offset < headerSizeOffset; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newHeaderSize;
    upkFile.read(reinterpret_cast<char*>(&newHeaderSize), sizeof(newHeaderSize));
    newHeaderSize += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newHeaderSize), sizeof(newHeaderSize));
    for (offset = upkFile.tellg(); offset < NameCountOffset + 8; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newExportCount;
    upkFile.read(reinterpret_cast<char*>(&newExportCount), sizeof(newExportCount));
    ++newExportCount;
    std::cout << "newExportCount = " << newExportCount << std::endl;
    newUPK.write(reinterpret_cast<char*>(&newExportCount), sizeof(newExportCount));
    for (unsigned i = 0; i < 12; ++i)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    uint32_t newDependsOffset;
    upkFile.read(reinterpret_cast<char*>(&newDependsOffset), sizeof(newDependsOffset));
    newDependsOffset += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newDependsOffset), sizeof(newDependsOffset));
    uint32_t newUnknown1;
    upkFile.read(reinterpret_cast<char*>(&newUnknown1), sizeof(newUnknown1));
    newUnknown1 += newEntrySize;
    newUPK.write(reinterpret_cast<char*>(&newUnknown1), sizeof(newUnknown1));
    for (offset = upkFile.tellg(); offset < header.ExportOffset; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    for (unsigned i = 0; i < header.ExportCount; ++i)
    {
        ObjectListEntry EntryToRead;

        upkFile.read(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));

        EntryToRead.DataOffset += newEntrySize;

        newUPK.write(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));

        for (unsigned j = 0; j < EntryToRead.NumAdditionalFields * sizeof(uint32_t); ++j)
        {
            uint8_t ch = upkFile.get();
            newUPK.put(ch);
        }
    }
    newUPK.write(reinterpret_cast<char*>(&entry), sizeof(entry));
    for (unsigned j = 0; j < entry.NumAdditionalFields * 4; ++j)
    {
        newUPK.put(0);
    }
    for (offset = upkFile.tellg(); offset < upkFileSize; ++offset)
    {
        uint8_t ch = upkFile.get();
        newUPK.put(ch);
    }
    newUPK.close();
    open((UPKfilename + ".new2").c_str());
    return true;
}

bool UPKUtils::AddImportListEntry(ImportListEntry entry)
{
    return true;
}*/
