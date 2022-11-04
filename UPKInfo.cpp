#include "UPKInfo.h"

#include <cstdio>
#include <sstream>
#include <cstring>

UPKInfo::UPKInfo(std::istream& stream): Summary(), NoneIdx(0), ReadError(UPKReadErrors::NoErrors), Compressed(false), CompressedChunk(false)
{
    ///on read errors assume the package might be fully compressed and try reading compressed header
    if (!Read(stream) && ReadError != UPKReadErrors::IsCompressed)
        ReadCompressedHeader(stream);
}

bool UPKInfo::ReadCompressedHeader(std::istream& stream)
{
    if (!stream.good())
    {
        ReadError = UPKReadErrors::FileError;
        return false;
    }
    stream.seekg(0, std::ios::end);
    uint32_t Size = stream.tellg();
    stream.seekg(0);
    stream.read(reinterpret_cast<char*>(&CompressedHeader.Signature), 4);
    if (CompressedHeader.Signature != 0x9E2A83C1)
    {
        ReadError = UPKReadErrors::BadSignature;
        return false;
    }
    stream.read(reinterpret_cast<char*>(&CompressedHeader.BlockSize), 4);
    stream.read(reinterpret_cast<char*>(&CompressedHeader.CompressedSize), 4);
    stream.read(reinterpret_cast<char*>(&CompressedHeader.UncompressedSize), 4);
    CompressedHeader.NumBlocks = (CompressedHeader.UncompressedSize + CompressedHeader.BlockSize - 1) / CompressedHeader.BlockSize; // Gildor
    uint32_t CompHeadSize = 16 + CompressedHeader.NumBlocks * 8;
    Size -= CompHeadSize; /// actual compressed file size
    if (CompressedHeader.CompressedSize != Size ||
        CompressedHeader.UncompressedSize < Size ||
        CompressedHeader.UncompressedSize < CompressedHeader.CompressedSize)
    {
        ReadError = UPKReadErrors::BadVersion;
        return false;
    }
    CompressedHeader.Blocks.clear();
    for (unsigned i = 0; i < CompressedHeader.NumBlocks; ++i)
    {
        FCompressedChunkBlock Block;
        stream.read(reinterpret_cast<char*>(&Block.CompressedSize), 4);
        stream.read(reinterpret_cast<char*>(&Block.UncompressedSize), 4);
        CompressedHeader.Blocks.push_back(Block);
    }
    Compressed = true;
    CompressedChunk = true;
    ReadError = UPKReadErrors::IsCompressed;
    return false;
}

bool UPKInfo::Read(std::istream& stream)
{
    uint32_t headerEndOffset = 4;
    CompressedHeader = FCompressedChunkHeader{};
    if (!stream.good())
    {
        ReadError = UPKReadErrors::FileError;
        return false;
    }
    stream.seekg(0);
    stream.read(reinterpret_cast<char*>(&Summary.Signature), 4);
    if (Summary.Signature != 0x9E2A83C1)
    {
        ReadError = UPKReadErrors::BadSignature;
        return false;
    }
    int32_t tmpVer;
    stream.read(reinterpret_cast<char*>(&tmpVer), 4);
    Summary.Version = tmpVer % (1 << 16);
    Summary.LicenseeVersion = tmpVer >> 16;
    if (Summary.Version != VER_XCOM && Summary.Version != VER_BATMAN && Summary.Version != VER_BATMAN_CITY)
    {
        ReadError = UPKReadErrors::BadVersion;
        return false;
    }
    Summary.HeaderSizeOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&Summary.HeaderSize), 4);
    headerEndOffset += Summary.HeaderSize;
    stream.read(reinterpret_cast<char*>(&Summary.FolderNameLength), 4);
    if (Summary.FolderNameLength > 0)
    {
        getline(stream, Summary.FolderName, '\0');
    }
    else
    {
        Summary.FolderName = "";
    }
    stream.read(reinterpret_cast<char*>(&Summary.PackageFlags), 4);
    Summary.NameCountOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&Summary.NameCount), 4);
    stream.read(reinterpret_cast<char*>(&Summary.NameOffset), 4);
    stream.read(reinterpret_cast<char*>(&Summary.ExportCount), 4);
    stream.read(reinterpret_cast<char*>(&Summary.ExportOffset), 4);
    stream.read(reinterpret_cast<char*>(&Summary.ImportCount), 4);
    stream.read(reinterpret_cast<char*>(&Summary.ImportOffset), 4);
    stream.read(reinterpret_cast<char*>(&Summary.DependsOffset), 4);
    if (Summary.Version >= VER_BATMAN_CITY)
    {
        stream.read(reinterpret_cast<char*>(&Summary.SerialOffset), 4);
        stream.read(reinterpret_cast<char*>(&Summary.Unknown2), 4);
        stream.read(reinterpret_cast<char*>(&Summary.Unknown3), 4);
        stream.read(reinterpret_cast<char*>(&Summary.Unknown4), 4);
    }
    if (!Summary.SerialOffset)
        Summary.SerialOffset = Summary.HeaderSize;
    stream.read(reinterpret_cast<char*>(&Summary.GUID), sizeof(Summary.GUID));
    stream.read(reinterpret_cast<char*>(&Summary.GenerationsCount), 4);
    if ((uint32_t)stream.tellg() + Summary.GenerationsCount * 12 > (uint32_t)headerEndOffset)
    {
        ReadError = UPKReadErrors::BadVersion;
        return false;
    }
    Summary.Generations.clear();
    for (unsigned i = 0; i < Summary.GenerationsCount; ++i)
    {
        FGenerationInfo EntryToRead;
        stream.read(reinterpret_cast<char*>(&EntryToRead.ExportCount), 4);
        stream.read(reinterpret_cast<char*>(&EntryToRead.NameCount), 4);
        stream.read(reinterpret_cast<char*>(&EntryToRead.NetObjectCount), 4);
        Summary.Generations.push_back(EntryToRead);
    }
    stream.read(reinterpret_cast<char*>(&Summary.EngineVersion), 4);
    stream.read(reinterpret_cast<char*>(&Summary.CookerVersion), 4);
    stream.read(reinterpret_cast<char*>(&Summary.CompressionFlags), 4);
    stream.read(reinterpret_cast<char*>(&Summary.NumCompressedChunks), 4);
    if ((uint32_t)stream.tellg() + Summary.NumCompressedChunks * 16 > (uint32_t)headerEndOffset)
    {
        ReadError = UPKReadErrors::BadVersion;
        return false;
    }
    Compressed = ((Summary.NumCompressedChunks > 0) || (Summary.CompressionFlags != 0));
    Summary.CompressedChunks.clear();
    for (unsigned i = 0; i < Summary.NumCompressedChunks; ++i)
    {
        FCompressedChunk CompressedChunk;
        stream.read(reinterpret_cast<char*>(&CompressedChunk.UncompressedOffset), 4);
        stream.read(reinterpret_cast<char*>(&CompressedChunk.UncompressedSize), 4);
        stream.read(reinterpret_cast<char*>(&CompressedChunk.CompressedOffset), 4);
        stream.read(reinterpret_cast<char*>(&CompressedChunk.CompressedSize), 4);
        Summary.CompressedChunks.push_back(CompressedChunk);
    }
    Summary.UnknownDataChunk.clear();
    /// for uncompressed packages unknown data is located between NumCompressedChunks and NameTable
    if (Summary.NumCompressedChunks < 1 && Summary.NameOffset - stream.tellg() > 0)
    {
        Summary.UnknownDataChunk.resize(Summary.NameOffset - stream.tellg());
    }
    /// for compressed packages unknown data is located between last CompressedChunk entry and first compressed data
    else if (Summary.NumCompressedChunks > 0 && Summary.CompressedChunks[0].CompressedOffset - stream.tellg() > 0 && Summary.CompressedChunks[0].CompressedOffset - stream.tellg() <= headerEndOffset)
    {
        Summary.UnknownDataChunk.resize(Summary.CompressedChunks[0].CompressedOffset - stream.tellg());
    }
    if (Summary.UnknownDataChunk.size() > 0)
    {
        stream.read(Summary.UnknownDataChunk.data(), Summary.UnknownDataChunk.size());
    }
    if (Compressed == true)
    {
        ReadError = UPKReadErrors::IsCompressed;
        return false;
    }
    NameTable.clear();
    stream.seekg(Summary.NameOffset);
    for (unsigned i = 0; i < Summary.NameCount; ++i)
    {
        FNameEntry EntryToRead;
        EntryToRead.EntryOffset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&EntryToRead.NameLength), 4);
        if (EntryToRead.NameLength > 0)
        {
            getline(stream, EntryToRead.Name, '\0');
        }
        else
        {
            EntryToRead.Name = "";
        }
        stream.read(reinterpret_cast<char*>(&EntryToRead.NameFlagsL), 4);
        stream.read(reinterpret_cast<char*>(&EntryToRead.NameFlagsH), 4);
        EntryToRead.EntrySize = (unsigned)stream.tellg() - EntryToRead.EntryOffset;
        NameTable.push_back(EntryToRead);
        if (EntryToRead.Name == "None")
            NoneIdx = i;
    }
    ImportTable.clear();
    stream.seekg(Summary.ImportOffset);
    ImportTable.push_back(FObjectImport()); /// null object (default zero-initialization)
    for (unsigned i = 0; i < Summary.ImportCount; ++i)
    {
        FObjectImport EntryToRead;
        EntryToRead.EntryOffset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&EntryToRead.PackageIdx), sizeof(EntryToRead.PackageIdx));
        stream.read(reinterpret_cast<char*>(&EntryToRead.TypeIdx), sizeof(EntryToRead.TypeIdx));
        stream.read(reinterpret_cast<char*>(&EntryToRead.OwnerRef), sizeof(EntryToRead.OwnerRef));
        stream.read(reinterpret_cast<char*>(&EntryToRead.NameIdx), sizeof(EntryToRead.NameIdx));
        EntryToRead.EntrySize = (unsigned)stream.tellg() - EntryToRead.EntryOffset;
        ImportTable.push_back(EntryToRead);
    }
    ExportTable.clear();
    stream.seekg(Summary.ExportOffset);
    ExportTable.push_back(FObjectExport()); /// null-object
    for (unsigned i = 0; i < Summary.ExportCount; ++i)
    {
        FObjectExport EntryToRead;
        EntryToRead.EntryOffset = stream.tellg();
        stream.read(reinterpret_cast<char*>(&EntryToRead.TypeRef), sizeof(EntryToRead.TypeRef));
        stream.read(reinterpret_cast<char*>(&EntryToRead.ParentClassRef), sizeof(EntryToRead.ParentClassRef));
        stream.read(reinterpret_cast<char*>(&EntryToRead.OwnerRef), sizeof(EntryToRead.OwnerRef));
        stream.read(reinterpret_cast<char*>(&EntryToRead.NameIdx), sizeof(EntryToRead.NameIdx));
        stream.read(reinterpret_cast<char*>(&EntryToRead.ArchetypeRef), sizeof(EntryToRead.ArchetypeRef));
        if (Summary.Version == VER_BATMAN_CITY)
        {
            stream.read(reinterpret_cast<char*>(&EntryToRead.Unknown2), sizeof(EntryToRead.Unknown2));
        }
        stream.read(reinterpret_cast<char*>(&EntryToRead.ObjectFlagsH), sizeof(EntryToRead.ObjectFlagsH));
        stream.read(reinterpret_cast<char*>(&EntryToRead.ObjectFlagsL), sizeof(EntryToRead.ObjectFlagsL));
        stream.read(reinterpret_cast<char*>(&EntryToRead.SerialSize), sizeof(EntryToRead.SerialSize));
        stream.read(reinterpret_cast<char*>(&EntryToRead.SerialOffset), sizeof(EntryToRead.SerialOffset));
        stream.read(reinterpret_cast<char*>(&EntryToRead.ExportFlags), sizeof(EntryToRead.ExportFlags));
        stream.read(reinterpret_cast<char*>(&EntryToRead.NetObjectCount), sizeof(EntryToRead.NetObjectCount));
        stream.read(reinterpret_cast<char*>(&EntryToRead.GUID), sizeof(EntryToRead.GUID));
        stream.read(reinterpret_cast<char*>(&EntryToRead.Unknown1), sizeof(EntryToRead.Unknown1));
        EntryToRead.NetObjects.resize(EntryToRead.NetObjectCount);
        if (EntryToRead.NetObjectCount > 0)
        {
            stream.read(reinterpret_cast<char*>(EntryToRead.NetObjects.data()), EntryToRead.NetObjects.size()*4);
        }
        EntryToRead.EntrySize = (unsigned)stream.tellg() - EntryToRead.EntryOffset;
        ExportTable.push_back(EntryToRead);
    }
    DependsBuf.clear();
    if (Summary.SerialOffset && Summary.DependsOffset && Summary.DependsOffset < Summary.SerialOffset)
    {
        DependsBuf.resize(Summary.SerialOffset - Summary.DependsOffset);
        if (DependsBuf.size() > 0)
        {
            stream.read(DependsBuf.data(), DependsBuf.size());
        }
        if (!stream.good())
        {
            ReadError = UPKReadErrors::FileError;
            return false;
        }
    }
    /// resolve names
    for (unsigned i = 1; i < ImportTable.size(); ++i)
    {
        ImportTable[i].Name = IndexToName(ImportTable[i].NameIdx);
        ImportTable[i].FullName = ResolveFullName(-i);
        ImportTable[i].Type = IndexToName(ImportTable[i].TypeIdx);
        if (ImportTable[i].Type == "")
        {
            ImportTable[i].Type = "Class";
        }
    }
    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        ExportTable[i].Name = IndexToName(ExportTable[i].NameIdx);
        ExportTable[i].FullName = ResolveFullName(i);
        ExportTable[i].Type = ObjRefToName(ExportTable[i].TypeRef);
        if (ExportTable[i].Type == "")
        {
            ExportTable[i].Type = "Class";
        }
    }
    return true;
}

std::string UPKInfo::IndexToName(UNameIndex idx)
{
    std::ostringstream ss;
    ss << GetNameEntry(idx.NameTableIdx).Name;
    if (idx.Numeric > 0 && ss.str() != "None")
        ss << "_" << int(idx.Numeric - 1);
    return ss.str();
}

UNameIndex UPKInfo::NoneToUNameIndex()
{
    UNameIndex NameIdx;
    NameIdx.NameTableIdx = NoneIdx;
    NameIdx.Numeric = 0;
    return NameIdx;
}

UNameIndex UPKInfo::NameToUNameIndex(std::string name)
{
    std::string nameStr, numericStr;
    size_t pos = name.find("_");
    if (pos != std::string::npos)
    {
        nameStr = name.substr(0, pos);
        numericStr = name.substr(pos + 1, name.size() - (pos + 1));
    }
    else
    {
        nameStr = name;
        numericStr = "";
    }
    UNameIndex NameIdx;
    NameIdx.NameTableIdx = FindName(nameStr);
    if (NameIdx.NameTableIdx < 0)
        NameIdx.NameTableIdx = NoneIdx;
    if (numericStr != "")
    {
        std::stringstream ss(numericStr);
        ss >> NameIdx.Numeric;
        NameIdx.Numeric += 1;
    }
    else
    {
        NameIdx.Numeric = 0;
    }
    return NameIdx;
}

std::string UPKInfo::ObjRefToName(UObjectReference ObjRef)
{
    if (ObjRef == 0 || -ObjRef >= (int)ImportTable.size() || ObjRef >= (int)ExportTable.size())
    {
        return "";
    }
    else if (ObjRef > 0)
    {
        return IndexToName(GetExportEntry(ObjRef).NameIdx);
    }
    else if (ObjRef < 0)
    {
        return IndexToName(GetImportEntry(-ObjRef).NameIdx);
    }
    return "";
}

std::string UPKInfo::ResolveFullName(UObjectReference ObjRef)
{
    std::string name;
    name = ObjRefToName(ObjRef);
    UObjectReference next = GetOwnerRef(ObjRef);
    while (next != 0)
    {
        name = ObjRefToName(next) + "." + name;
        next = GetOwnerRef(next);
    }
    return name;
}

UObjectReference UPKInfo::GetOwnerRef(UObjectReference ObjRef)
{
    if (ObjRef == 0)
    {
        return 0;
    }
    else if (ObjRef > 0)
    {
        return GetExportEntry(ObjRef).OwnerRef;
    }
    else if (ObjRef < 0)
    {
        return GetImportEntry(-ObjRef).OwnerRef;
    }
    return 0;
}

bool UPKInfo::IsPropertiesObject(UObjectReference ObjRef)
{
    if (ObjRef < 0 || ObjRef > (int32_t)ExportTable.size())
        return false;
    return (ExportTable[ObjRef].ObjectFlagsH & (uint32_t)UObjectFlagsH::PropertiesObject);
}

int UPKInfo::FindName(std::string name)
{
    for (unsigned i = 0; i < NameTable.size(); ++i)
    {
        if (NameTable[i].Name == name)
            return i;
    }
    return -1;
}

UObjectReference UPKInfo::FindObject(std::string FullName, bool isExport)
{
    /// Import object
    if (isExport == false)
    {
        for (unsigned i = 1; i < ImportTable.size(); ++i)
        {
            if (ImportTable[i].FullName == FullName)
                return -i;
        }
    }
    /// Export object
    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        if (ExportTable[i].FullName == FullName)
            return i;
    }
    /// Object not found
    return 0;
}

UObjectReference UPKInfo::FindObjectOfType(std::string FullName, std::string Type, bool isExport)
{
    /// Import object
    if (isExport == false)
    {
        for (unsigned i = 1; i < ImportTable.size(); ++i)
        {
            if (ImportTable[i].FullName == FullName && ImportTable[i].Type == Type)
                return -i;
        }
    }
    /// Export object
    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        if (ExportTable[i].FullName == FullName && ExportTable[i].Type == Type)
            return i;
    }
    /// Object not found
    return 0;
}

UObjectReference UPKInfo::FindObjectByName(std::string Name, bool isExport)
{
    /// Import object
    if (isExport == false)
    {
        for (unsigned i = 1; i < ImportTable.size(); ++i)
        {
            if (ImportTable[i].Name == Name)
                return -i;
        }
    }
    /// Export object
    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        if (ExportTable[i].Name == Name)
            return i;
    }
    /// Object not found
    return 0;
}

UObjectReference UPKInfo::FindObjectByOffset(uint32_t offset)
{
    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        if (offset >= ExportTable[i].SerialOffset && offset < ExportTable[i].SerialOffset + ExportTable[i].SerialSize)
            return i;
    }
    return 0;
}

const FObjectExport& UPKInfo::GetExportEntry(uint32_t idx)
{
    if (idx < ExportTable.size())
        return ExportTable[idx];
    else
        return ExportTable[0];
}

const FObjectImport& UPKInfo::GetImportEntry(uint32_t idx)
{
    if (idx < ImportTable.size())
        return ImportTable[idx];
    else
        return ImportTable[0];
}

const FNameEntry& UPKInfo::GetNameEntry(uint32_t idx)
{
    if (idx < NameTable.size())
        return NameTable[idx];
    else
        return NameTable[NoneIdx];
}

std::string UPKInfo::FormatCompressedHeader()
{
    std::ostringstream ss;
    ss << "Signature: " << FormatHEX(CompressedHeader.Signature) << std::endl
       << "BlockSize: " << CompressedHeader.BlockSize << std::endl
       << "CompressedSize: " << CompressedHeader.CompressedSize << std::endl
       << "UncompressedSize: " << CompressedHeader.UncompressedSize << std::endl
       << "NumBlocks: " << CompressedHeader.NumBlocks << std::endl;
    for (unsigned i = 0; i < CompressedHeader.Blocks.size(); ++i)
    {
        ss << "Blocks[" << i << "]:" << std::endl
           << "\tCompressedSize: " << CompressedHeader.Blocks[i].CompressedSize << std::endl
           << "\tUncompressedSize: " << CompressedHeader.Blocks[i].UncompressedSize << std::endl;
    }
    return ss.str();
}

std::string UPKInfo::FormatSummary()
{
    if (CompressedChunk == true)
        return FormatCompressedHeader();
    std::ostringstream ss;
    ss << "Signature: " << FormatHEX(Summary.Signature) << std::endl
       << "Version: " << Summary.Version << std::endl
       << "LicenseeVersion: " << Summary.LicenseeVersion << std::endl
       << "HeaderSize: " << Summary.HeaderSize << " (" << FormatHEX(Summary.HeaderSize) << ")" << std::endl
       << "Folder: " << Summary.FolderName << std::endl
       << "PackageFlags: " << FormatHEX(Summary.PackageFlags) << std::endl
       << FormatPackageFlags(Summary.PackageFlags)
       << "NameCount: " << Summary.NameCount << std::endl
       << "NameOffset: " << FormatHEX(Summary.NameOffset) << std::endl
       << "ExportCount: " << Summary.ExportCount << std::endl
       << "ExportOffset: " << FormatHEX(Summary.ExportOffset) << std::endl
       << "ImportCount: " << Summary.ImportCount << std::endl
       << "ImportOffset: " << FormatHEX(Summary.ImportOffset) << std::endl
       << "DependsOffset: " << FormatHEX(Summary.DependsOffset) << std::endl
       << "SerialOffset: " << FormatHEX(Summary.SerialOffset) << std::endl
       << "Unknown2: " << FormatHEX(Summary.Unknown2) << std::endl
       << "Unknown3: " << FormatHEX(Summary.Unknown3) << std::endl
       << "Unknown4: " << FormatHEX(Summary.Unknown4) << std::endl
       << "GUID: " << FormatHEX(Summary.GUID) << std::endl
       << "GenerationsCount: " << Summary.GenerationsCount << std::endl;
    for (unsigned i = 0; i < Summary.Generations.size(); ++i)
    {
        ss << "Generations[" << i << "]:" << std::endl
           << "\tExportCount: " << Summary.Generations[i].ExportCount << std::endl
           << "\tNameCount: " << Summary.Generations[i].NameCount << std::endl
           << "\tNetObjectCount: " << Summary.Generations[i].NetObjectCount << std::endl;
    }
    ss << "EngineVersion: " << Summary.EngineVersion << std::endl
       << "CookerVersion: " << Summary.CookerVersion << std::endl
       << "CompressionFlags: " << FormatHEX(Summary.CompressionFlags) << std::endl
       << FormatCompressionFlags(Summary.CompressionFlags)
       << "NumCompressedChunks: " << Summary.NumCompressedChunks << std::endl;
    for (unsigned i = 0; i < Summary.CompressedChunks.size(); ++i)
    {
        ss << "CompressedChunks[" << i << "]:" << std::endl
           << "\tUncompressedOffset: " << FormatHEX(Summary.CompressedChunks[i].UncompressedOffset) << " (" << Summary.CompressedChunks[i].UncompressedOffset << ")" << std::endl
           << "\tUncompressedSize: " << Summary.CompressedChunks[i].UncompressedSize << std::endl
           << "\tCompressedOffset: " << FormatHEX(Summary.CompressedChunks[i].CompressedOffset) << "(" << Summary.CompressedChunks[i].CompressedOffset << ")" << std::endl
           << "\tCompressedSize: " << Summary.CompressedChunks[i].CompressedSize << std::endl;
    }
    if (Summary.UnknownDataChunk.size() > 0)
    {
        ss << "Unknown data size: " << Summary.UnknownDataChunk.size() << std::endl;
        ss << "Unknown data: " << FormatHEX(Summary.UnknownDataChunk) << std::endl;
    }
    return ss.str();
}

std::string UPKInfo::FormatNames(bool verbose)
{
    std::ostringstream ss;
    ss << "NameTable:" << std::endl;
    for (unsigned i = 0; i < NameTable.size(); ++i)
    {
        ss << FormatName(i, verbose);
    }
    return ss.str();
}

std::string UPKInfo::FormatImports(bool verbose)
{
    std::ostringstream ss;
    ss << "ImportTable:" << std::endl;
    for (unsigned i = 1; i < ImportTable.size(); ++i)
    {
        ss << FormatImport(i, verbose);
    }
    return ss.str();
}

std::string UPKInfo::FormatExports(bool verbose)
{
    std::ostringstream ss;
    ss << "ExportTable:" << std::endl;
    for (unsigned i = 1; i < ExportTable.size(); ++i)
    {
        ss << FormatExport(i, verbose);
    }
    return ss.str();
}

std::string UPKInfo::FormatName(uint32_t idx, bool verbose)
{
    std::ostringstream ss;
    FNameEntry Entry = GetNameEntry(idx);
    ss << FormatHEX((uint32_t)idx) << " (" << idx << ") ( "
       << FormatHEX((char*)&idx, sizeof(idx)) << "): "
       << Entry.Name << std::endl;
    if (verbose == true)
    {
        ss << "\tNameFlagsL: " << FormatHEX(Entry.NameFlagsL) << std::endl
           << "\tNameFlagsH: " << FormatHEX(Entry.NameFlagsH) << std::endl;
    }
    return ss.str();
}

std::string UPKInfo::FormatImport(uint32_t idx, bool verbose)
{
    std::ostringstream ss;
    int32_t invIdx = -idx;
    FObjectImport Entry = GetImportEntry(idx);
    ss << FormatHEX((uint32_t)(-idx)) << " (" << (-(int)idx) << ") ( "
       << FormatHEX((char*)&invIdx, sizeof(invIdx)) << "): "
       << Entry.Type << "\'"
       << Entry.FullName << "\'" << std::endl;
    if (verbose == true)
    {
        ss << "\tPackageIdx: " << FormatHEX(Entry.PackageIdx) << " -> " << IndexToName(Entry.PackageIdx) << std::endl
           << "\tTypeIdx: " << FormatHEX(Entry.TypeIdx) << " -> " << IndexToName(Entry.TypeIdx) << std::endl
           << "\tOwnerRef: " << FormatHEX((uint32_t)Entry.OwnerRef) << " -> " << ObjRefToName(Entry.OwnerRef) << std::endl
           << "\tNameIdx: " << FormatHEX(Entry.NameIdx) << " -> " << IndexToName(Entry.NameIdx) << std::endl;
    }
    return ss.str();
}

std::string UPKInfo::FormatExport(uint32_t idx, bool verbose)
{
    std::ostringstream ss;
    FObjectExport Entry = GetExportEntry(idx);
    ss << FormatHEX((uint32_t)idx) << " (" << idx << ") ( "
       << FormatHEX((char*)&idx, sizeof(idx)) << "): "
       << Entry.Type << "\'"
       << Entry.FullName << "\'" << std::endl;
    if (verbose == true)
    {
        ss << "\tTypeRef: " << FormatHEX((uint32_t)Entry.TypeRef) << " -> " << ObjRefToName(Entry.TypeRef) << std::endl
           << "\tParentClassRef: " << FormatHEX((uint32_t)Entry.ParentClassRef) << " -> " << ObjRefToName(Entry.ParentClassRef) << std::endl
           << "\tOwnerRef: " << FormatHEX((uint32_t)Entry.OwnerRef) << " -> " << ObjRefToName(Entry.OwnerRef) << std::endl
           << "\tNameIdx: " << FormatHEX(Entry.NameIdx) << " -> " << IndexToName(Entry.NameIdx) << std::endl
           << "\tArchetypeRef: " << FormatHEX((uint32_t)Entry.ArchetypeRef) << " -> " << ObjRefToName(Entry.ArchetypeRef) << std::endl;
        if (Summary.Version == VER_BATMAN_CITY)
           ss << "\tUnknown2: " << FormatHEX(Entry.Unknown2) << std::endl;
        ss << "\tObjectFlagsH: " << FormatHEX(Entry.ObjectFlagsH) << std::endl
           << FormatObjectFlagsH(Entry.ObjectFlagsH)
           << "\tObjectFlagsL: " << FormatHEX(Entry.ObjectFlagsL) << std::endl
           << FormatObjectFlagsL(Entry.ObjectFlagsL)
           << "\tSerialSize: " << FormatHEX(Entry.SerialSize) << " (" << Entry.SerialSize << ")" << std::endl
           << "\tSerialOffset: " << FormatHEX(Entry.SerialOffset) << std::endl
           << "\tExportFlags: " << FormatHEX(Entry.ExportFlags) << std::endl
           << FormatExportFlags(Entry.ExportFlags)
           << "\tNetObjectCount: " << Entry.NetObjectCount << std::endl
           << "\tGUID: " << FormatHEX(Entry.GUID) << std::endl
           << "\tUnknown1: " << FormatHEX(Entry.Unknown1) << std::endl;
        for (unsigned i = 0; i < Entry.NetObjects.size(); ++i)
        {
            ss << "\tNetObjects[" << i << "]: " << FormatHEX(Entry.NetObjects[i]) << std::endl;
        }
    }
    return ss.str();
}

std::string FormatHEX(FGuid GUID)
{
    char ch[255];
    sprintf(ch, "%08X%08X%08X%08X", GUID.GUID_A, GUID.GUID_B, GUID.GUID_C, GUID.GUID_D);
    return std::string(ch);
}

std::string FormatHEX(UNameIndex NameIndex)
{
    char ch[255];
    sprintf(ch, "0x%08X (Index) 0x%08X (Numeric)", NameIndex.NameTableIdx, NameIndex.Numeric);
    return std::string(ch);
}

std::string FormatReadErrors(UPKReadErrors ReadError)
{
    std::ostringstream ss;
    switch(ReadError)
    {
    case UPKReadErrors::FileError:
        ss << "Bad package file!\n";
        break;
    case UPKReadErrors::BadSignature:
        ss << "Bad package signature!\n";
        break;
    case UPKReadErrors::BadVersion:
        ss << "Bad package version!\n";
        break;
    case UPKReadErrors::IsCompressed:
        ss << "Package is compressed, must decompress first!\n";
        break;
    default:
        break;
    }
    return ss.str();
}
