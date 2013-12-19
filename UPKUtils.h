#ifndef UPKUTILS_H
#define UPKUTILS_H

#include <string>
#include <fstream>
#include <vector>
#include <cassert>

struct UnrealFunctionFlags
{
    uint32_t Static = 0x00002000;
    uint32_t Singular = 0x00000020;
    uint32_t Native = 0x00000400;
    uint32_t NoExport = 0x00004000;
    uint32_t Exec = 0x00000200;
    uint32_t Latent = 0x00000008;
    uint32_t Iterator = 0x00000004;
    uint32_t Simulated = 0x00000100;
    uint32_t Server = 0x00200000;
    uint32_t Client = 0x01000000;
    uint32_t Reliable = 0x00000080;
//??? Unreliable
    uint32_t Public = 0x00020000;
    uint32_t Private = 0x00040000;
    uint32_t Protected = 0x00080000;
    uint32_t Operator = 0x00001000;
    uint32_t PreOperator = 0x00000010;
//??? PostOperator
    uint32_t Event = 0x00000800;
    uint32_t Const = 0x00008000;
    uint32_t Final = 0x00000001;
    uint32_t Defined = 0x00000002;
};

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
    uint32_t NameFlagsH;
    uint32_t NameFlagsL;
};

struct ObjectListEntry
{
    int32_t  ObjTypeRef;
    int32_t  ParentClassRef;
    int32_t  OwnerRef;
    uint32_t NameListIdx;
    uint32_t Field5;
    uint32_t Field6;
    uint32_t ObjectFlagsH;
    uint32_t ObjectFlagsL;
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
    uint32_t PackageIDidx;
    uint32_t Field1;
    uint32_t ObjTypeIdx;
    uint32_t Field3;
    int32_t  OwnerRef;
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
    std::string GetObjectOrImportNameByIdx(int idx);

    size_t GetFileSize();
    bool CheckValidFileOffset(size_t offset);
    bool good();

    std::vector<char> GetObjectData(int idx);
    bool WriteObjectData(int idx, std::vector<char> data, std::vector<char> *backupData = nullptr);
    bool WriteNamelistName(int idx, std::string name);
    bool WriteData(size_t offset, std::vector<char> data, std::vector<char> *backupData = nullptr);

    bool MoveObject(int idx, uint32_t newObjectSize = 0);
    bool UndoMoveObject(int idx);

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
