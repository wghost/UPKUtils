#ifndef UPKUTILS_H
#define UPKUTILS_H

#include <string>
#include <fstream>
#include <vector>
#include <cassert>

struct Generation
{
    uint32_t ExportCount;
    uint32_t NameCount;
    uint32_t NetObjectCount;
};

struct packageHeader
{
    uint32_t Signature;
    uint16_t Version;
    uint16_t LicenseVersion;
    uint32_t HeaderSize;
    uint32_t FolderNameLength;
    std::string   FolderName;
    uint32_t PackageFlags;
 	uint32_t NameCount;
  	uint32_t NameOffset;
  	uint32_t ExportCount;
  	uint32_t ExportOffset;
  	uint32_t ImportCount;
  	uint32_t ImportOffset;
  	uint32_t DependsOffset;
  	uint32_t Unknown1;
  	uint32_t Unknown2;
  	uint32_t Unknown3;
  	uint32_t Unknown4;
  	std::vector<uint8_t>  GUID;
  	uint32_t GenerationsCount;
    std::vector<Generation> generations;
    uint32_t EngineVersion;
    uint32_t CookerVersion;
    uint32_t CompressionFlags;
};

struct NameListEntry
{
    uint32_t NameLength;
    std::string   NameString;
    uint32_t Field1;
    uint32_t Field2;
};

struct ObjectListEntry
{
    uint32_t ObjType;
    uint32_t ParentClassRef;
    uint32_t OwnerRef;
    uint32_t NameListIdx;
    uint32_t Field5;
    uint32_t Field6;
    uint32_t PropertyFlags;
    uint32_t Field8;
    uint32_t ObjectFileSize;
    uint32_t DataOffset;
    uint32_t Field11;
    uint32_t NumAdditionalFields;
    uint32_t Field13;
    uint32_t Field14;
    uint32_t Field15;
    uint32_t Field16;
    uint32_t Field17;
};

struct ImportListEntry
{
    uint32_t PackageID;
    uint32_t Field1;
    uint32_t ObjType;
    uint32_t Field3;
    uint32_t OwnerRef;
    uint32_t NameListIdx;
    uint32_t Field6;
};

class UPKUtils
{
public:
    UPKUtils() {}
    ~UPKUtils() {}

    UPKUtils(const char filename[]);

    bool open(const char filename[]);
    void close();

    bool ReadUPKHeader();
    bool ReconstructObjectNames();

    int FindNamelistEntryByName(std::string entryName);
    int FindObjectListEntryByName(std::string entryName);
    int FindImportListEntryByName(std::string entryName);

    size_t FindDataChunk(std::vector<char> data);

    NameListEntry GetNameListEntryByIdx(int idx);
    ObjectListEntry GetObjectListEntryByIdx(int idx);
    ImportListEntry GetImportListEntryByIdx(int idx);
    uint32_t GetNameListEntryFileOffsetByIdx(int idx);
    uint32_t GetNameListNameFileOffsetByIdx(int idx);

    packageHeader GetHeader();
    int GetNameListSize();
    int GetObjectListSize();
    int GetImportListSize();
    std::string GetNameByIdx(int idx);
    std::string GetObjectNameByIdx(int idx);
    std::string GetImportNameByIdx(int idx);

    size_t GetFileSize();
    bool CheckValidFileOffset(size_t offset);
    bool good();

    std::vector<char> GetObjectData(int idx);
    bool WriteObjectData(int idx, std::vector<char> data);
    bool WriteNamelistName(int idx, std::string name);
    bool WriteData(size_t offset, std::vector<char> data);

    bool MoveObject(int idx, uint32_t newObjectSize = 0, bool isFunction = false);

private:
    std::fstream upkFile;
    packageHeader header;
    std::vector<NameListEntry> NameList;
    std::vector<ObjectListEntry> ObjectList;
    std::vector<ImportListEntry> ImportList;
    std::vector<std::string> ObjectNameList;
    std::vector<std::string> ImportNameList;
    std::vector<uint32_t> NameListOffsets;
    std::vector<uint32_t> ObjectListOffsets;
    size_t upkFileSize;
};

#endif // UPKUTILS_H
