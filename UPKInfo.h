///
/// This is the class to extract header information from UPK files:
/// summary, compressed chunks, name table, import table, export table and
/// depends table
///
#ifndef UPKINFO_H
#define UPKINFO_H

#include <vector>
#include <iostream>

#include "UFlags.h"

#define VER_XCOM 845
<<<<<<< Updated upstream
#define VER_BATMAN 576
#define VER_BATMAN_CITY 805
=======
#define VER_XMEN_ORIGINS 568
#define VER_BATMAN 576
#define VER_BATMAN_CITY 805
#define VER_XCOM 845
#define VER_XCOM_DECLASSIFIED 849
#define VER_MURDERED_SOUL_SUSPECT 860
#define VER_STYX 866
#define VER_SHERLOCK_7 868
#define VER_KILLING_FLOOR_2 871
#define VER_BULLETSTORM_FULL_CLIP 887
#define VER_ASSASSINS_CREED 889
#define VER_SHADOW_COMPLEX 893
#define VER_SHERLOCK_8 895
>>>>>>> Stashed changes

enum class UPKReadErrors
{
    NoErrors = 0,
    FileError,
    BadSignature,
    BadVersion,
    IsCompressed
};

typedef int32_t UObjectReference;

struct FGuid
{
    /// persistent
    uint32_t GUID_A;
    uint32_t GUID_B;
    uint32_t GUID_C;
    uint32_t GUID_D;
};

struct FGenerationInfo
{
    int32_t ExportCount;
    int32_t NameCount;
    int32_t NetObjectCount;
};

struct FCompressedChunkBlock
{
    uint32_t CompressedSize;
    uint32_t UncompressedSize;
};

struct FCompressedChunkHeader
{
    uint32_t Signature;          /// equals to PACKAGE_FILE_TAG (0x9E2A83C1)
    uint32_t BlockSize;          /// maximum size of uncompressed block, always the same
    uint32_t CompressedSize;
    uint32_t UncompressedSize;
    uint32_t NumBlocks;
    std::vector<FCompressedChunkBlock> Blocks;
};

struct FCompressedChunk
{
    uint32_t UncompressedOffset;
    uint32_t UncompressedSize;
    uint32_t CompressedOffset;
    uint32_t CompressedSize;
};

struct UNameIndex
{
    uint32_t NameTableIdx;
    uint32_t Numeric;
};

struct FPackageFileSummary
{
    /// persistent
    uint32_t Signature;
    uint16_t Version;
    uint16_t LicenseeVersion;
    uint32_t HeaderSize;
    int32_t FolderNameLength;
    std::string FolderName;
    uint32_t PackageFlags;
    uint32_t NameCount;
    uint32_t NameOffset;
    uint32_t ExportCount;
    uint32_t ExportOffset;
    uint32_t ImportCount;
    uint32_t ImportOffset;
    uint32_t DependsOffset;
    uint32_t SerialOffset;
    uint32_t Unknown2;
    uint32_t Unknown3;
    uint32_t Unknown4;
    FGuid   GUID;
    uint32_t GenerationsCount;
    std::vector<FGenerationInfo> Generations;
    uint32_t EngineVersion;
    uint32_t CookerVersion;
    uint32_t CompressionFlags;
    uint32_t NumCompressedChunks;
    std::vector<FCompressedChunk> CompressedChunks;
    std::vector<char> UnknownDataChunk;
    /// memory
    uint32_t HeaderSizeOffset;
    uint32_t NameCountOffset;
    uint32_t UPKFileSize;
};

struct FNameEntry
{
    /// persistent
    int32_t     NameLength;
    std::string Name;
    uint32_t    NameFlagsL;
    uint32_t    NameFlagsH;
    /// memory
    uint32_t    EntryOffset;
    uint32_t    EntrySize;
};

struct FObjectImport
{
    /// persistent
    UNameIndex       PackageIdx;
    UNameIndex       TypeIdx;
    UObjectReference OwnerRef;
    UNameIndex       NameIdx;
    /// memory
    uint32_t         EntryOffset;
    uint32_t         EntrySize;
    std::string      Name;
    std::string      FullName;
    std::string      Type;
};

struct FObjectExport
{
    /// persistent
    UObjectReference TypeRef;
    UObjectReference ParentClassRef;
    UObjectReference OwnerRef;
    UNameIndex       NameIdx;
    UObjectReference ArchetypeRef;
    uint32_t         Unknown2;            /// batman city
    uint32_t         ObjectFlagsH;
    uint32_t         ObjectFlagsL;
    uint32_t         SerialSize;
    uint32_t         SerialOffset;
    uint32_t         ExportFlags;
    uint32_t         NetObjectCount;
    FGuid            GUID;
    uint32_t         Unknown1;
    std::vector<uint32_t> NetObjects;     /// 4 x NetObjectCount bytes of data
    /// memory
    uint32_t         EntryOffset;
    uint32_t         EntrySize;
    std::string      Name;
    std::string      FullName;
    std::string      Type;
};

class UPKInfo
{
    public:
        /// constructors
        UPKInfo(): Summary(), NoneIdx(0), ReadError(UPKReadErrors::NoErrors), Compressed(false), CompressedChunk(false), LastAccessedExportObjIdx(0) {};
        UPKInfo(std::istream& stream);
        /// destructor
        ~UPKInfo() {};
        /// read package header
        bool Read(std::istream& stream);
        bool ReadCompressedHeader(std::istream& stream);
        /// helpers
        std::string IndexToName(UNameIndex idx);
        UNameIndex NoneToUNameIndex();
        UNameIndex NameToUNameIndex(std::string name);
        std::string ObjRefToName(UObjectReference ObjRef);
        std::string ResolveFullName(UObjectReference ObjRef);
        UObjectReference GetOwnerRef(UObjectReference ObjRef);
        bool IsPropertiesObject(UObjectReference ObjRef);
        int FindName(std::string name);
        UObjectReference FindObject(std::string FullName, bool isExport = true);
        UObjectReference FindObjectOfType(std::string FullName, std::string Type, bool isExport = true);
        UObjectReference FindObjectByName(std::string Name, bool isExport = true);
        UObjectReference FindObjectByOffset(uint32_t offset);
        bool IsNoneIdx(UNameIndex idx) { return (idx.NameTableIdx == NoneIdx); }
        /// Getters
        const FPackageFileSummary& GetSummary() { return Summary; }
        const FObjectExport& GetExportEntry(uint32_t idx);
        const std::vector<FObjectExport>& GetExportTable() { return ExportTable; }
        const FObjectImport& GetImportEntry(uint32_t idx);
        const FNameEntry& GetNameEntry(uint32_t idx);
        FGuid GetGUID() { return Summary.GUID; }
        uint16_t GetVersion() { return Summary.Version; }
        bool IsLzoCompressed() { return Summary.CompressionFlags == (uint32_t)UCompressionFlags::LZO || IsFullyCompressed(); }
        bool IsCompressed() { return Compressed; }
        bool IsFullyCompressed() { return (Compressed && CompressedChunk); }
        UPKReadErrors GetError() { return ReadError; }
        uint32_t GetCompressionFlags() { return Summary.CompressionFlags; }
        UObjectReference GetLastAccessedExportObjIdx() { return LastAccessedExportObjIdx; }
        uint32_t GetNoneIdx() { return NoneIdx; }
        /// format header to text string
        std::string FormatCompressedHeader();
        std::string FormatSummary();
        std::string FormatNames(bool verbose = false);
        std::string FormatImports(bool verbose = false);
        std::string FormatExports(bool verbose = false);
        std::string FormatName(uint32_t idx, bool verbose = false);
        std::string FormatImport(uint32_t idx, bool verbose = false);
        std::string FormatExport(uint32_t idx, bool verbose = false);
    protected:
        FPackageFileSummary Summary;
        std::vector<FNameEntry> NameTable;
        std::vector<FObjectImport> ImportTable;
        std::vector<FObjectExport> ExportTable;
        std::vector<char> DependsBuf;
        uint32_t NoneIdx;
        UPKReadErrors ReadError;
        bool Compressed;
        bool CompressedChunk;
        FCompressedChunkHeader CompressedHeader;
        UObjectReference LastAccessedExportObjIdx;
};

std::string FormatHEX(FGuid GUID);
std::string FormatHEX(UNameIndex NameIndex);
std::string FormatReadErrors(UPKReadErrors ReadError);

#endif // UPKINFO_H
