#include "UPKUtils.h"

#include <iostream>
#include <cstring>

uint8_t PatchUPKhash [] = {0x7A, 0xA0, 0x56, 0xC9,
                           0x60, 0x5F, 0x7B, 0x31,
                           0x72, 0x5D, 0x4B, 0xC4,
                           0x7C, 0xD2, 0x4D, 0xD9 };

UPKUtils::UPKUtils(const char filename[])
{
    upkFile.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    assert(upkFile.is_open() == true);
    upkFile.seekg(0, std::ios::end);
    upkFileSize = upkFile.tellg();
    upkFile.seekg(0, std::ios::beg);
    ReadUPKHeader();
    ReconstructObjectNames();
}

bool UPKUtils::open(const char filename[])
{
    if (upkFile.is_open())
    {
        upkFile.close();
        upkFile.clear();
    }
    upkFile.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!upkFile.is_open())
        return false;
    upkFile.seekg(0, std::ios::end);
    upkFileSize = upkFile.tellg();
    upkFile.seekg(0, std::ios::beg);
    ReadUPKHeader();
    ReconstructObjectNames();
    return true;
}

void UPKUtils::close()
{
    if (upkFile.is_open())
    {
        upkFile.close();
        upkFile.clear();
    }
}

inline size_t UPKUtils::GetFileSize()
{
    return upkFileSize;
}

bool UPKUtils::CheckValidFileOffset(size_t offset)
{
    return (offset >= header.NameOffset && offset < upkFileSize);
}

bool UPKUtils::good()
{
    return (upkFile.is_open() && upkFile.good());
}

bool UPKUtils::ReadUPKHeader()
{
    NameList.clear();
    NameListOffsets.clear();
    ObjectList.clear();
    ImportList.clear();
    header.generations.clear();

    if (!upkFile.is_open() || !upkFile.good())
        return false;

    upkFile.clear();
    upkFile.seekg(0, std::ios::beg);

    upkFile.read(reinterpret_cast<char*>(&header.Signature), sizeof(header.Signature));
    upkFile.read(reinterpret_cast<char*>(&header.Version), sizeof(header.Version));
    upkFile.read(reinterpret_cast<char*>(&header.LicenseVersion), sizeof(header.LicenseVersion));
    upkFile.read(reinterpret_cast<char*>(&header.HeaderSize), sizeof(header.HeaderSize));
    upkFile.read(reinterpret_cast<char*>(&header.FolderNameLength), sizeof(header.FolderNameLength));

    getline(upkFile, header.FolderName, '\0');

    upkFile.read(reinterpret_cast<char*>(&header.PackageFlags), sizeof(header.PackageFlags));
    upkFile.read(reinterpret_cast<char*>(&header.NameCount), sizeof(header.NameCount));
    upkFile.read(reinterpret_cast<char*>(&header.NameOffset), sizeof(header.NameOffset));
    upkFile.read(reinterpret_cast<char*>(&header.ExportCount), sizeof(header.ExportCount));
    upkFile.read(reinterpret_cast<char*>(&header.ExportOffset), sizeof(header.ExportOffset));
    upkFile.read(reinterpret_cast<char*>(&header.ImportCount), sizeof(header.ExportCount));
    upkFile.read(reinterpret_cast<char*>(&header.ImportOffset), sizeof(header.ImportOffset));
    upkFile.read(reinterpret_cast<char*>(&header.DependsOffset), sizeof(header.DependsOffset));

    upkFile.read(reinterpret_cast<char*>(&header.Unknown1), sizeof(header.Unknown1));
    upkFile.read(reinterpret_cast<char*>(&header.Unknown2), sizeof(header.Unknown2));
    upkFile.read(reinterpret_cast<char*>(&header.Unknown3), sizeof(header.Unknown3));
    upkFile.read(reinterpret_cast<char*>(&header.Unknown4), sizeof(header.Unknown4));

    header.GUID.resize(16);
    upkFile.read(reinterpret_cast<char*>(header.GUID.data()), header.GUID.size());

    upkFile.read(reinterpret_cast<char*>(&header.GenerationsCount), sizeof(header.GenerationsCount));

    for (uint32_t i = 0; i < header.GenerationsCount; ++i)
    {
        Generation EntryToRead;
        upkFile.read(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));
        header.generations.push_back(EntryToRead);
    }

    upkFile.read(reinterpret_cast<char*>(&header.EngineVersion), sizeof(header.EngineVersion));
    upkFile.read(reinterpret_cast<char*>(&header.CookerVersion), sizeof(header.CookerVersion));
    upkFile.read(reinterpret_cast<char*>(&header.CompressionFlags), sizeof(header.CompressionFlags));

    upkFile.seekg(header.NameOffset);

    for (uint32_t i = 0; i < header.NameCount; ++i)
    {
        NameListEntry EntryToRead;

        NameListOffsets.push_back(upkFile.tellg());

        upkFile.read(reinterpret_cast<char*>(&EntryToRead.NameLength), sizeof(EntryToRead.NameLength));
        getline(upkFile, EntryToRead.NameString, '\0');
        upkFile.read(reinterpret_cast<char*>(&EntryToRead.Field1), sizeof(EntryToRead.Field1));
        upkFile.read(reinterpret_cast<char*>(&EntryToRead.Field2), sizeof(EntryToRead.Field2));

        NameList.push_back(EntryToRead);
    }

    upkFile.seekg(header.ExportOffset);

    ObjectListEntry nullObject = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    ObjectList.push_back(nullObject);
    ObjectListOffsets.push_back(upkFile.tellg());

    for (uint32_t i = 0; i < header.ExportCount; ++i)
    {
        ObjectListEntry EntryToRead;

        ObjectListOffsets.push_back(upkFile.tellg());

        upkFile.read(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));

        ObjectList.push_back(EntryToRead);

        if (EntryToRead.NumAdditionalFields != 0)
            upkFile.seekg(EntryToRead.NumAdditionalFields * sizeof(uint32_t), std::ios::cur);
    }

    upkFile.seekg(header.ImportOffset);

    ImportListEntry nullObjectImport = {0, 0, 0, 0, 0, 0, 0};

    ImportList.push_back(nullObjectImport);

    for (uint32_t i = 0; i < header.ImportCount; ++i)
    {
        ImportListEntry EntryToRead;

        upkFile.read(reinterpret_cast<char*>(&EntryToRead), sizeof(EntryToRead));

        ImportList.push_back(EntryToRead);
    }

    return true;
}

bool UPKUtils::ReconstructObjectNames()
{
    if (!upkFile.is_open() || !upkFile.good())
        return false;

    ObjectNameList.clear();
    ObjectNameList.push_back("");
    ImportNameList.clear();
    ImportNameList.push_back("");

    for (uint32_t i = 1; i < ObjectList.size(); ++i)
    {
        std::string NameString;
        ObjectListEntry EntryToRead = ObjectList[i];
        NameString = NameList[EntryToRead.NameListIdx].NameString;
        uint32_t objRef = EntryToRead.OwnerRef;
        while (objRef != 0 && objRef < ObjectList.size())
        {
            if (ObjectList[objRef].NameListIdx != 0 && ObjectList[objRef].NameListIdx < NameList.size())
                NameString = NameList[ObjectList[objRef].NameListIdx].NameString + "." + NameString;
            objRef = ObjectList[objRef].OwnerRef;
        }
        ObjectNameList.push_back(NameString);
    }

    for (uint32_t i = 1; i < ImportList.size(); ++i)
    {
        std::string NameString;
        ImportListEntry EntryToRead = ImportList[i];
        NameString = NameList[EntryToRead.NameListIdx].NameString;
        int32_t objRef = EntryToRead.OwnerRef;
        while (objRef != 0)
        {
            uint32_t nameRef = 0;
            if (objRef > 0 && uint32_t(objRef) < ObjectList.size())
            {
                nameRef = ObjectList[objRef].NameListIdx;
                objRef = ObjectList[objRef].OwnerRef;
            }
            else if (uint32_t(-objRef) < ImportList.size())
            {
                nameRef = ImportList[-objRef].NameListIdx;
                objRef = ImportList[-objRef].OwnerRef;
            }

            if (nameRef != 0 && nameRef < NameList.size())
                NameString = NameList[nameRef].NameString + "." + NameString;
        }
        NameString = NameList[EntryToRead.PackageIDidx].NameString + "::" + NameString;
        ImportNameList.push_back(NameString);
    }

    return true;
}

int UPKUtils::FindNamelistEntryByName(std::string entryName)
{
    for (unsigned int i = 0; i < NameList.size(); ++i)
        if (NameList[i].NameString == entryName)
            return i;
    return -1;
}

int UPKUtils::FindObjectListEntryByName(std::string entryName)
{
    for (unsigned int i = 0; i < ObjectNameList.size(); ++i)
        if (ObjectNameList[i] == entryName)
            return i;
    return -1;
}

int UPKUtils::FindImportListEntryByName(std::string entryName)
{
    for (unsigned int i = 0; i < ImportNameList.size(); ++i)
        if (ImportNameList[i] == entryName)
            return i;
    return -1;
}

NameListEntry UPKUtils::GetNameListEntryByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < NameList.size());
    return NameList[idx];
}

ObjectListEntry UPKUtils::GetObjectListEntryByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < ObjectList.size());
    return ObjectList[idx];
}

ImportListEntry UPKUtils::GetImportListEntryByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < ImportList.size());
    return ImportList[idx];
}

uint32_t UPKUtils::GetNameListEntryFileOffsetByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < NameListOffsets.size());
    return NameListOffsets[idx];
}

uint32_t UPKUtils::GetNameListNameFileOffsetByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < NameListOffsets.size());
    return (NameListOffsets[idx] + sizeof(NameList[idx].NameLength));
}

std::vector<char> UPKUtils::GetObjectData(int idx)
{
    assert(idx >= 0 && size_t(idx) < ObjectList.size());
    std::vector<char> data;
    if (!upkFile.is_open() || !upkFile.good())
        return data;
    data.resize(ObjectList[idx].ObjectFileSize);
    upkFile.seekg(ObjectList[idx].DataOffset);
    upkFile.read(data.data(), data.size());
    return data;
}

bool UPKUtils::WriteObjectData(int idx, std::vector<char> data, std::vector<char> *backupData)
{
    assert(idx >= 0 && size_t(idx) < ObjectList.size());
    if (!upkFile.is_open() || !upkFile.good())
        return false;
    if (ObjectList[idx].ObjectFileSize != data.size())
        return false;
    if (backupData != nullptr)
    {
        backupData->clear();
        backupData->resize(data.size());
        upkFile.seekg(ObjectList[idx].DataOffset);
        upkFile.read(backupData->data(), backupData->size());
    }
    upkFile.seekp(ObjectList[idx].DataOffset);
    upkFile.write(data.data(), data.size());
    return true;
}

bool UPKUtils::WriteNamelistName(int idx, std::string name)
{
    assert(idx >= 0 && size_t(idx) < NameList.size());
    if (!upkFile.is_open() || !upkFile.good())
        return false;
    if (NameList[idx].NameLength - 1 != name.length())
        return false;
    upkFile.seekp(NameListOffsets[idx] + sizeof(NameList[idx].NameLength));
    upkFile.write(name.c_str(), name.length());
    ReadUPKHeader();
    ReconstructObjectNames();
    return true;
}

bool UPKUtils::WriteData(size_t offset, std::vector<char> data, std::vector<char> *backupData)
{
    if (!CheckValidFileOffset(offset))
        return false;
    if (!upkFile.is_open() || !upkFile.good())
        return false;
    if (backupData != nullptr)
    {
        backupData->clear();
        backupData->resize(data.size());
        upkFile.seekg(offset);
        upkFile.read(backupData->data(), backupData->size());
    }
    upkFile.seekp(offset);
    upkFile.write(data.data(), data.size());
    if (offset < header.ImportOffset) // changed name/objectlist data
    {
        ReadUPKHeader();
        ReconstructObjectNames();
    }
    return true;
}

size_t UPKUtils::FindDataChunk(std::vector<char> data)
{
    size_t offset = 0, idx = 0;
    std::vector<char> fileBuf(upkFileSize);

    upkFile.seekg(0);
    upkFile.read(fileBuf.data(), fileBuf.size());

    char* pFileBuf = fileBuf.data();
    char* pData = data.data();

    for (char* p = pFileBuf; p != pFileBuf + fileBuf.size() - data.size(); ++p)
    {
        if (memcmp(p, pData, data.size()) == 0)
        {
            offset = idx;
            break;
        }
        ++idx;
    }

    upkFile.clear();
    upkFile.seekg(header.NameOffset);
    return offset;
}

bool UPKUtils::MoveObject(int idx, uint32_t newObjectSize)
{
    std::vector<char> data = GetObjectData(idx);
    upkFile.seekg(0, std::ios::end);
    uint32_t newObjectOffset = upkFile.tellg();
    bool isFunction = (GetObjectOrImportNameByIdx(ObjectList[idx].ObjTypeRef).find("Function") != std::string::npos);
    if (newObjectSize > ObjectList[idx].ObjectFileSize)
    {
        upkFile.seekp(ObjectListOffsets[idx] + sizeof(uint32_t)*8);
        upkFile.write(reinterpret_cast<char*>(&newObjectSize), sizeof(newObjectSize));
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
            memcpy(&oldMemSize, data.data() + 0x28, 0x4);  // copy function memory size
            memcpy(&oldFileSize, data.data() + 0x2C, 0x4); // and file size
            uint32_t newMemSize = oldMemSize + diffSize;   // find new sizes
            uint32_t newFileSize = oldFileSize + diffSize;
            uint32_t headSize = 0x30 + oldFileSize - 1;    // head size (all data before 0x53)
            uint32_t tailSize = ObjectList[idx].ObjectFileSize - headSize; // tail size (0x53 and all data after)
            std::vector<char> newData(newObjectSize);
            memset(newData.data(), 0x0B, newObjectSize);     // fill new data with 0x0B
            memcpy(newData.data(), data.data(), headSize);   // copy all data before 0x53
            memcpy(newData.data() + 0x28, &newMemSize, 0x4); // set new memory size
            memcpy(newData.data() + 0x2C, &newFileSize, 0x4);// and file size
            memcpy(newData.data() + headSize + diffSize, data.data() + headSize, tailSize); // copy 0x53 and all data after
            data = newData;
        }
    }
    upkFile.seekp(ObjectListOffsets[idx] + sizeof(uint32_t)*9);
    upkFile.write(reinterpret_cast<char*>(&newObjectOffset), sizeof(newObjectOffset));
    upkFile.seekp(newObjectOffset);
    upkFile.write(data.data(), data.size());
    // write backup info
    upkFile.write(reinterpret_cast<char*>(&PatchUPKhash[0]), 16);
    upkFile.write(reinterpret_cast<char*>(&ObjectList[idx].ObjectFileSize), sizeof(ObjectList[idx].ObjectFileSize));
    upkFile.write(reinterpret_cast<char*>(&ObjectList[idx].DataOffset), sizeof(ObjectList[idx].DataOffset));
    // backup info end
    upkFile.seekg(0, std::ios::end);
    upkFileSize = upkFile.tellg();
    upkFile.seekg(0, std::ios::beg);
    ReadUPKHeader();
    ReconstructObjectNames();
    return true;
}

bool UPKUtils::UndoMoveObject(int idx)
{
    upkFile.seekg(ObjectList[idx].DataOffset + ObjectList[idx].ObjectFileSize);
    uint8_t readHash [16];
    upkFile.read(reinterpret_cast<char*>(&readHash[0]), 16);
    if (memcmp(readHash, PatchUPKhash, 16) != 0)
        return false;
    uint32_t oldObjectFileSize, oldObjectOffset;
    upkFile.read(reinterpret_cast<char*>(&oldObjectFileSize), sizeof(oldObjectFileSize));
    upkFile.read(reinterpret_cast<char*>(&oldObjectOffset), sizeof(oldObjectOffset));
    upkFile.seekp(ObjectListOffsets[idx] + sizeof(uint32_t)*8);
    upkFile.write(reinterpret_cast<char*>(&oldObjectFileSize), sizeof(oldObjectFileSize));
    upkFile.write(reinterpret_cast<char*>(&oldObjectOffset), sizeof(oldObjectOffset));
    upkFile.seekg(0, std::ios::end);
    upkFileSize = upkFile.tellg();
    upkFile.seekg(0, std::ios::beg);
    ReadUPKHeader();
    ReconstructObjectNames();
    return true;
}

packageHeader UPKUtils::GetHeader()
{
    return header;
}

int UPKUtils::GetNameListSize()
{
    return NameList.size();
}

int UPKUtils::GetObjectListSize()
{
    return ObjectList.size();
}

int UPKUtils::GetImportListSize()
{
    return ImportList.size();
}

std::string UPKUtils::GetNameByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < NameList.size());
    return NameList[idx].NameString;
}

std::string UPKUtils::GetObjectNameByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < ObjectNameList.size());
    return ObjectNameList[idx];
}

std::string UPKUtils::GetImportNameByIdx(int idx)
{
    assert(idx >= 0 && size_t(idx) < ImportNameList.size());
    return ImportNameList[idx];
}

std::string UPKUtils::GetObjectOrImportNameByIdx(int idx)
{
    if (idx < 0)
        return GetImportNameByIdx(-idx);
    else
        return GetObjectNameByIdx(idx);
}
