#include "UObject.h"
#include "LzoUtils.h"

#include <sstream>
#include <fstream>
#include <cstring>

bool UBulkDataMirror::IsDataCompressed()
{
    return SavedBulkDataFlags & (uint32_t)UBulkDataFlags::CompressedLzo || SavedBulkDataFlags & (uint32_t)UBulkDataFlags::CompressedLzx || SavedBulkDataFlags & (uint32_t)UBulkDataFlags::CompressedZlib;
}

bool UBulkDataMirror::IsDataStoredElsewhere()
{
    return SavedBulkDataFlags & (uint32_t)UBulkDataFlags::StoredInSeparateFile || SavedBulkDataFlags & (uint32_t)UBulkDataFlags::StoredAsSeparateData;
}

bool UBulkDataMirror::IsDataEmpty()
{
    return SavedBulkDataFlags & (uint32_t)UBulkDataFlags::EmptyData;
}

bool UBulkDataMirror::GetDataFlag(UBulkDataFlags flg)
{
    return SavedBulkDataFlags & (uint32_t)flg;
}

void UBulkDataMirror::SetDataFlag(UBulkDataFlags flg, bool val)
{
    if (val)
        SavedBulkDataFlags |= (uint32_t)flg;
    else
        SavedBulkDataFlags &= ~((uint32_t)flg);
}

void UBulkDataMirror::ToggleDataFlag(UBulkDataFlags flg)
{
    SavedBulkDataFlags ^= (uint32_t)flg;
}

std::string UBulkDataMirror::Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner)
{
    std::ostringstream ss;
    stream.read(reinterpret_cast<char*>(&SavedBulkDataFlags), sizeof(SavedBulkDataFlags));
    ss << "\tSavedBulkDataFlags = " << FormatHEX(SavedBulkDataFlags) << std::endl;
    ss << FormatBulkDataFlags(SavedBulkDataFlags);
    stream.read(reinterpret_cast<char*>(&SavedElementCount), 4);
    ss << "\tSavedElementCount = " << FormatHEX(SavedElementCount) << " (" << SavedElementCount << ")" << std::endl;
    stream.read(reinterpret_cast<char*>(&SavedBulkDataSizeOnDisk), 4);
    ss << "\tSavedBulkDataSizeOnDisk = " << FormatHEX(SavedBulkDataSizeOnDisk) << " (" << SavedBulkDataSizeOnDisk << ")" << std::endl;
    stream.read(reinterpret_cast<char*>(&SavedBulkDataOffsetInFile), 4);
    ss << "\tSavedBulkDataOffsetInFile = " << FormatHEX(SavedBulkDataOffsetInFile) << std::endl;
    if (IsDataEmpty())
    {
        ss << "\tBulk data is empty!\n";
        BulkData.clear();
    }
    else if (GetDataFlag(UBulkDataFlags::StoredInSeparateFile))
    {
        ToggleDataFlag(UBulkDataFlags::StoredInSeparateFile);
        WasInExternalFile = true;
        if (externalFileName.size() > 0)
        {
            ss << "\tExternal file name " << externalFileName << "\n";
            bool readRes = ReadDataChunkFromExternalFile();
            if (!readRes)
            {
                ss << "\tBulk data: external file read error!\n";
                BulkData.clear();
                //BulkData.assign(SavedBulkDataSizeOnDisk, 0xCC);
                SetDataFlag(UBulkDataFlags::EmptyData, true);
            }
        }
        else if (!IsDataEmpty())
        {
            if (GetDataFlag(UBulkDataFlags::CompressedLzo))
            {
                BulkData.resize(SavedElementCount, 0xCC);
                ToggleDataFlag(UBulkDataFlags::CompressedLzo);
                WasCompressed = true;
                SavedBulkDataSizeOnDisk = SavedElementCount;
            }
            else
                BulkData.resize(SavedBulkDataSizeOnDisk, 0xCC);
        }
    }
    else if (GetDataFlag(UBulkDataFlags::StoredAsSeparateData))
    {
        ToggleDataFlag(UBulkDataFlags::StoredAsSeparateData);
        size_t currOffset = stream.tellg(); ///save curr offset
        stream.seekg(SavedBulkDataOffsetInFile);
        BulkData.resize(SavedBulkDataSizeOnDisk);
        stream.read(BulkData.data(), BulkData.size());
        stream.seekg(currOffset); ///go back
        if (!stream.good())
        {
            ss << "Error deserializing bulk data: bad offset or data size!\n";
            BulkData.clear();
            SetDataFlag(UBulkDataFlags::EmptyData, true);
            return ss.str();
        }
    }
    else
    {
        size_t maxOffset = info.GetExportEntry(owner).SerialOffset + info.GetExportEntry(owner).SerialSize;
        if (SavedBulkDataOffsetInFile + SavedBulkDataSizeOnDisk > maxOffset)
        {
            ss << "Error deserializing bulk data: bad offset or data size!\n";
            BulkData.clear();
            SetDataFlag(UBulkDataFlags::EmptyData, true);
            return ss.str();
        }
        BulkData.resize(SavedBulkDataSizeOnDisk);
        stream.read(BulkData.data(), BulkData.size());
    }
    ss << "\tBulk data (" << SavedBulkDataSizeOnDisk << " bytes)\n";
    //ss << "\tBulk data: " << FormatHEX(BulkData) << std::endl;
    if (BulkData.size() > 0)
    {
        if (GetDataFlag(UBulkDataFlags::CompressedLzo))
        {
            ToggleDataFlag(UBulkDataFlags::CompressedLzo);
            WasCompressed = true;
            std::vector<char> decompessedData;
            uint32_t decompressedSize = DecompressLzoCompressedRawData(BulkData, decompessedData);
            ss << "\tDecompressed bulk data (" << decompressedSize << " bytes)\n";
            if (decompressedSize == 0)
            {
                ss << "\tDecompression error: no decompressed data!\n";
                BulkData.clear();
                //BulkData.assign(SavedElementCount, 0xCC);
                SetDataFlag(UBulkDataFlags::EmptyData, true);
            }
            else if (decompressedSize != SavedElementCount)
            {
                ss << "\tDecompressed data size mismatch!\n";
                //BulkData = decompessedData;
                //BulkData.resize(SavedElementCount, 0xCC);
                BulkData.clear();
                SetDataFlag(UBulkDataFlags::EmptyData, true);
            }
            else
            {
                ss << "\tDecompressed successfully!\n";
                BulkData = decompessedData;
            }
            ///resetting sizes after decompression
            SavedElementCount = BulkData.size();
            SavedBulkDataSizeOnDisk = SavedElementCount;
        }
        else if (GetDataFlag(UBulkDataFlags::CompressedZlib) || GetDataFlag(UBulkDataFlags::CompressedLzx))
        {
            ss << "\tBulk data: Zlib/Lzx compression is not supported!\n";
            BulkData.clear();
            SetDataFlag(UBulkDataFlags::EmptyData, true);
        }
    }
    return ss.str();
}

bool UBulkDataMirror::ReadDataChunkFromExternalFile()
{
    std::ifstream in(externalFileName, std::ios_base::binary);
    if (!in.is_open())
    {
        std::cerr << "Bulk data: Cannot open external file!\n";
        return false;
    }
    in.seekg(SavedBulkDataOffsetInFile);
    BulkData.resize(SavedBulkDataSizeOnDisk);
    in.read(reinterpret_cast<char*>(BulkData.data()), BulkData.size());
    if (!in.good())
    {
        std::cerr << "Bulk data: External file read error!\n";
        return false;
    }
    return true;
}

void UBulkDataMirror::SetEmpty()
{
    BulkData.clear();
    SetDataFlag(UBulkDataFlags::EmptyData, true);
    SavedElementCount = 0;
    SavedBulkDataSizeOnDisk = 0xFFFFFFFF;
    SavedBulkDataOffsetInFile = 0xFFFFFFFF;
    LockFileOffset = false;
}

void UBulkDataMirror::SetBulkDataRaw(std::vector<char> Data)
{
    BulkData = Data;
    ///assuming uncompressed embedded data
    SavedBulkDataFlags = 0;
    SavedElementCount = SavedBulkDataSizeOnDisk = BulkData.size();
    SavedBulkDataOffsetInFile = 0xFFFFFFFF;
    LockFileOffset = false;
}

std::string UBulkDataMirror::Serialize()
{
    std::stringstream ss;
    if (BulkData.size() <= 0 && !IsDataEmpty())
        SetEmpty();
    ss.write(reinterpret_cast<char*>(&SavedBulkDataFlags), 4);
    ss.write(reinterpret_cast<char*>(&SavedElementCount), 4);
    ss.write(reinterpret_cast<char*>(&SavedBulkDataSizeOnDisk), 4);
    ss.write(reinterpret_cast<char*>(&SavedBulkDataOffsetInFile), 4);
    if (BulkData.size() > 0 && !IsDataStoredElsewhere())
    {
        ss.write(BulkData.data(), BulkData.size());
    }
    return ss.str();
}

std::string UBulkDataMirror::Serialize(size_t offset)
{
    if (!LockFileOffset)
        SavedBulkDataOffsetInFile = offset + GetBulkDataRelOffset();
    return Serialize();
}

bool UBulkDataMirror::ExportToExternalFile(CustomTFC& T2DFile, std::string ObjName)
{
    if (externalFileName == "" || /*T2DFile.GetFilename() != externalFileName ||*/ !T2DFile.IsLoaded())
        return false;

    TFCInventoryEntry DataDescr;
    DataDescr.SavedBulkDataSizeOnDisk = SavedBulkDataSizeOnDisk;
    DataDescr.ObjectName = ObjName;
    if (T2DFile.WriteData(DataDescr, BulkData))
    {
        SetDataFlag(UBulkDataFlags::StoredInSeparateFile, true);
        SavedBulkDataOffsetInFile = DataDescr.SavedBulkDataOffsetInFile;
        LockFileOffset = true;
        return true;
    }
    else
        return false;
    /*std::ofstream extFile(externalFileName, std::ios_base::binary | std::ios_base::app | std::ios_base::ate);
    if (!extFile.is_open())
    {
        std::cerr << "Bulk data: Cannot open external file!\n";
        return false;
    }
    SetDataFlag(UBulkDataFlags::StoredInSeparateFile, true);
    extFile.write(reinterpret_cast<char*>(BulkData.data()), BulkData.size());
    SavedBulkDataOffsetInFile = (uint32_t)extFile.tellp() - (uint32_t)BulkData.size();
    //std::cerr << "(uint32_t)extFile.tellp() = " << (uint32_t)extFile.tellp() << std::endl;
    //std::cerr << "(uint32_t)BulkData.size() = " << (uint32_t)BulkData.size() << std::endl;
    //std::cerr << "SavedBulkDataOffsetInFile = " << SavedBulkDataOffsetInFile << std::endl;
    LockFileOffset = true;
    if (!extFile.good())
    {
        std::cerr << "Bulk data: External file write error!\n";
        SetEmpty();
        return false;
    }
    ///zeros for alignment
    uint32_t alignmentBlockSize = 0x8000;
    if (extFile.tellp() % alignmentBlockSize)
    {
        std::vector<char> alignmentZeros(alignmentBlockSize - extFile.tellp() % alignmentBlockSize, 0x00);
        extFile.write(reinterpret_cast<char*>(alignmentZeros.data()), alignmentZeros.size());
    }
    extFile.close();
    return true;*/
}

bool UBulkDataMirror::TryLzoCompression()
{
    bool success = false;
    if (BulkData.size() > 0 && !IsDataCompressed() && !DoNotCompress)
    {
        std::vector<char> CompressedBulkData;
        if (LzoCompress(BulkData, CompressedBulkData) > 0)
        {
            SetDataFlag(UBulkDataFlags::CompressedLzo, true);
            SavedElementCount = BulkData.size();
            SavedBulkDataSizeOnDisk = CompressedBulkData.size();
            BulkData = CompressedBulkData;
            success = true;
        }
    }
    return success;
}

uint32_t UBulkDataMirror::CalculateSerializedSize()
{
    uint32_t res = 0;
    res += 16; /// at least 4 * 4 bytes of flags, element count, data size and data offset
    if (!(SavedBulkDataFlags & (uint32_t)UBulkDataFlags::EmptyData ||
          SavedBulkDataFlags & (uint32_t)UBulkDataFlags::StoredInSeparateFile ||
          SavedBulkDataFlags & (uint32_t)UBulkDataFlags::StoredAsSeparateData))
    {
        res += BulkData.size();
    }
    return res;
}

std::string UTexture2DMipMap::Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner)
{
    std::ostringstream ss;
    ss << UBulkDataMirror::Deserialize(stream, info, owner);
    stream.read(reinterpret_cast<char*>(&SizeX), sizeof(SizeX));
    ss << "\tSizeX: " << FormatHEX(SizeX) << " (" << SizeX << ")" << std::endl;
    stream.read(reinterpret_cast<char*>(&SizeY), sizeof(SizeY));
    ss << "\tSizeY: " << FormatHEX(SizeY) << " (" << SizeY << ")" << std::endl;
    return ss.str();
}

std::string UTexture2DMipMap::Serialize(size_t offset)
{
    std::ostringstream ss;
    ss << UBulkDataMirror::Serialize(offset);
    ss.write(reinterpret_cast<char*>(&SizeX), sizeof(SizeX));
    ss.write(reinterpret_cast<char*>(&SizeY), sizeof(SizeY));
    return ss.str();
}

uint32_t UTexture2DMipMap::CalculateSerializedSize()
{
    return sizeof(SizeX) + sizeof(SizeY) + UBulkDataMirror::CalculateSerializedSize();
}

std::string UDefaultPropertiesList::Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner, bool unsafe, bool quick)
{
    std::ostringstream ss;
    ss << "UDefaultPropertiesList:\n";
    PropertyOffset = stream.tellg();
    DefaultProperties.clear();
    size_t maxOffset = info.GetExportEntry(owner).SerialOffset + info.GetExportEntry(owner).SerialSize;
    UDefaultProperty Property;
    do
    {
        Property = UDefaultProperty{};
        ss << Property.Deserialize(stream, info, owner, unsafe, quick);
        DefaultProperties.push_back(Property);
    } while (Property.GetName() != "None" && stream.good() && (size_t)stream.tellg() < maxOffset);
    PropertySize = (unsigned)stream.tellg() - (unsigned)PropertyOffset;
    return ss.str();
}

std::string BCArrayOfNames::Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner)
{
    std::ostringstream ss;
    ss << "BCArrayOfNames:\n";
    stream.read(reinterpret_cast<char*>(&NamesNum), sizeof(NamesNum));
    ss << "\tNamesNum = " << NamesNum << std::endl;
    Names.resize(NamesNum);
    for (unsigned i = 0; i < NamesNum; ++i)
    {
        UNameIndex nameIdx;
        stream.read(reinterpret_cast<char*>(&nameIdx), sizeof(nameIdx));
        Names[i] = nameIdx;
        ss << "\tNameIdx: " << FormatHEX(nameIdx) << " -> " << info.IndexToName(nameIdx) << std::endl;
    }
    return ss.str();
}

std::string UDefaultProperty::Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner, bool unsafe, bool quick)
{
    Init(owner, unsafe, quick);
    size_t maxOffset = info.GetExportEntry(owner).SerialOffset + info.GetExportEntry(owner).SerialSize;
    std::ostringstream ss;
    ss << "UDefaultProperty:\n";
    stream.read(reinterpret_cast<char*>(&NameIdx), sizeof(NameIdx));
    Name = info.IndexToName(NameIdx);
    ss << "\tNameIdx: " << FormatHEX(NameIdx) << " -> " << info.IndexToName(NameIdx) << std::endl;
    if (Name != "None")
    {
        stream.read(reinterpret_cast<char*>(&TypeIdx), sizeof(TypeIdx));
        ss << "\tTypeIdx: " << FormatHEX(TypeIdx) << " -> " << info.IndexToName(TypeIdx) << std::endl;
        stream.read(reinterpret_cast<char*>(&PropertySize), sizeof(PropertySize));
        ss << "\tPropertySize: " << FormatHEX(PropertySize) << std::endl;
        /// prevent long loop, caused by bad data
        if ((int)stream.tellg() + (int)PropertySize > (int)maxOffset)
            return ss.str();
        stream.read(reinterpret_cast<char*>(&ArrayIdx), sizeof(ArrayIdx));
        ss << "\tArrayIdx: " << FormatHEX(ArrayIdx) << std::endl;
        Type = info.IndexToName(TypeIdx);
        if (Type == "BoolProperty")
        {
            if (info.GetVersion() >= VER_XCOM)
            {
                stream.read(reinterpret_cast<char*>(&BoolValue), sizeof(BoolValue));
            }
            else
            {
                stream.read(reinterpret_cast<char*>(&BoolValueOld), sizeof(BoolValueOld));
                BoolValue = (uint8_t)BoolValueOld;
            }
            ss << "\tBoolean value: " << FormatHEX(BoolValue) << " = ";
            if (BoolValue == 0)
                ss << "false\n";
            else
                ss << "true\n";
        }
        if (info.GetVersion() >= VER_XCOM && (Type == "StructProperty" || Type == "ByteProperty"))
        {
            stream.read(reinterpret_cast<char*>(&InnerNameIdx), sizeof(InnerNameIdx));
            ss << "\tInnerNameIdx: " << FormatHEX(InnerNameIdx) << " -> " << info.IndexToName(InnerNameIdx) << std::endl;
            if (Type == "StructProperty")
                Type = info.IndexToName(InnerNameIdx);
        }
        if (PropertySize > 0)
        {
            size_t offset = stream.tellg();
            InnerValue.resize(PropertySize);
            stream.read(InnerValue.data(), InnerValue.size());
            size_t offset2 = stream.tellg();
            if (QuickMode == false)
            {
                stream.seekg(offset);
                ss << DeserializeValue(stream, info);
            }
            stream.seekg(offset2);
        }
    }
    return ss.str();
}

std::string UDefaultProperty::DeserializeValue(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    if (Type == "IntProperty")
    {
        stream.read(reinterpret_cast<char*>(&valueInt), sizeof(valueInt));
        ss << "\tInteger: " << FormatHEX((uint32_t)valueInt) << " = " << valueInt << std::endl;
    }
    else if (Type == "FloatProperty")
    {
        stream.read(reinterpret_cast<char*>(&valueFloat), sizeof(valueFloat));
        ss << "\tFloat: " << FormatHEX(valueFloat) << " = " << valueFloat << std::endl;
    }
    else if (Type == "ObjectProperty" ||
             Type == "InterfaceProperty" ||
             Type == "ComponentProperty" ||
             Type == "ClassProperty")
    {
        stream.read(reinterpret_cast<char*>(&valueObjRef), sizeof(valueObjRef));
        ss << "\tObject: " << FormatHEX((uint32_t)valueObjRef) << " = ";
        if (valueObjRef == 0)
            ss << "none\n";
        else
            ss << info.ObjRefToName(valueObjRef) << std::endl;
    }
    else if (Type == "NameProperty" || Type == "ByteProperty")
    {
        stream.read(reinterpret_cast<char*>(&valueNameIdx), sizeof(valueNameIdx));
        ss << "\tName: " << FormatHEX(valueNameIdx) << " = " << info.IndexToName(valueNameIdx) << std::endl;
    }
    else if (Type == "StrProperty")
    {
        uint32_t StrLength;
        stream.read(reinterpret_cast<char*>(&StrLength), sizeof(StrLength));
        ss << "\tStrLength = " << FormatHEX(StrLength) << " = " << StrLength << std::endl;
        if (StrLength > 0)
        {
            getline(stream, valueString, '\0');
            ss << "\tString = " << valueString << std::endl;
        }
    }
    else if (Type == "ArrayProperty")
    {
        uint32_t NumElements;
        stream.read(reinterpret_cast<char*>(&NumElements), sizeof(NumElements));
        ss << "\tNumElements = " << FormatHEX(NumElements) << " = " << NumElements << std::endl;
        if ((NumElements > 0) && (PropertySize > 4))
        {
            std::string ArrayInnerType = FindArrayType(Name, stream, info);
            /*if (TryUnsafe == true && ArrayInnerType == "None")
            {
                ArrayInnerType = GuessArrayType(Name);
                if (ArrayInnerType != "None")
                    ss << "\tUnsafe type guess:\n";
            }*/
            ss << "\tArrayInnerType = " << ArrayInnerType << std::endl;
            UDefaultProperty InnerProperty;
            InnerProperty.OwnerRef = OwnerRef;
            InnerProperty.TryUnsafe = TryUnsafe;
            InnerProperty.Type = ArrayInnerType;
            InnerProperty.PropertySize = PropertySize - 4;
            if (ArrayInnerType == "None")
            {
                if (TryUnsafe == true &&
                    (InnerProperty.PropertySize/NumElements > 24 ||
                     InnerProperty.PropertySize/NumElements == 16 ||
                     InnerProperty.PropertySize/NumElements == 8 ||
                     InnerProperty.PropertySize/NumElements == 4))
                {
                    InnerProperty.PropertySize /= NumElements;
                    for (unsigned i = 0; i < NumElements; ++i)
                    {
                        ss << "\t" << Name << "[" << i << "]:\n";
                        ss << InnerProperty.DeserializeValue(stream, info);
                    }
                }
                else
                {
                    ss << InnerProperty.DeserializeValue(stream, info);
                }
            }
            else
            {
                InnerProperty.PropertySize /= NumElements;
                for (unsigned i = 0; i < NumElements; ++i)
                {
                    ss << "\t" << Name << "[" << i << "]:\n";
                    ss << InnerProperty.DeserializeValue(stream, info);
                }
            }
        }
    }
    else if (Type == "Vector")
    {
        float X, Y, Z;
        stream.read(reinterpret_cast<char*>(&X), sizeof(X));
        stream.read(reinterpret_cast<char*>(&Y), sizeof(Y));
        stream.read(reinterpret_cast<char*>(&Z), sizeof(Z));
        ss << "\tVector (X, Y, Z) = ("
           << FormatHEX(X) << ", " << FormatHEX(Y) << ", " << FormatHEX(Z) << ") = ("
           << X << ", " << Y << ", " << Z << ")" << std::endl;
    }
    else if (Type == "Rotator")
    {
        int32_t P, Y, R;
        stream.read(reinterpret_cast<char*>(&P), sizeof(P));
        stream.read(reinterpret_cast<char*>(&Y), sizeof(Y));
        stream.read(reinterpret_cast<char*>(&R), sizeof(R));
        ss << "\tRotator (Pitch, Yaw, Roll) = ("
           << FormatHEX((uint32_t)P) << ", " << FormatHEX((uint32_t)Y) << ", " << FormatHEX((uint32_t)R) << ") = ("
           << P << ", " << Y << ", " << R << ")" << std::endl;
    }
    else if (Type == "Vector2D")
    {
        float X, Y;
        stream.read(reinterpret_cast<char*>(&X), sizeof(X));
        stream.read(reinterpret_cast<char*>(&Y), sizeof(Y));
        ss << "\tVector2D (X, Y) = ("
           << FormatHEX(X) << ", " << FormatHEX(Y) << ") = ("
           << X << ", " << Y << ")" << std::endl;
    }
    else if (Type == "Guid")
    {
        FGuid GUID;
        stream.read(reinterpret_cast<char*>(&GUID), sizeof(GUID));
        ss << "\tGUID = " << FormatHEX(GUID) << std::endl;
    }
    else if (Type == "Color")
    {
        uint8_t R, G, B, A;
        stream.read(reinterpret_cast<char*>(&R), sizeof(R));
        stream.read(reinterpret_cast<char*>(&G), sizeof(G));
        stream.read(reinterpret_cast<char*>(&B), sizeof(B));
        stream.read(reinterpret_cast<char*>(&A), sizeof(A));
        ss << "\tColor (R, G, B, A) = ("
           << FormatHEX(R) << ", " << FormatHEX(G) << ", " << FormatHEX(B) << ", " << FormatHEX(A) << ") = ("
           << (unsigned)R << ", " << (unsigned)G << ", " << (unsigned)B << ", " << (unsigned)A << ")" << std::endl;
    }
    else if (Type == "LinearColor")
    {
        float R, G, B, A;
        stream.read(reinterpret_cast<char*>(&R), sizeof(R));
        stream.read(reinterpret_cast<char*>(&G), sizeof(G));
        stream.read(reinterpret_cast<char*>(&B), sizeof(B));
        stream.read(reinterpret_cast<char*>(&A), sizeof(A));
        ss << "\tLinearColor (R, G, B, A) = ("
           << FormatHEX(R) << ", " << FormatHEX(G) << ", " << FormatHEX(B) << ", " << FormatHEX(A) << ") = ("
           << R << ", " << G << ", " << B << ", " << A << ")" << std::endl;
    }
    else if (Type == "Box")
    {
        float X, Y, Z;
        stream.read(reinterpret_cast<char*>(&X), sizeof(X));
        stream.read(reinterpret_cast<char*>(&Y), sizeof(Y));
        stream.read(reinterpret_cast<char*>(&Z), sizeof(Z));
        ss << "\tVector Min (X, Y, Z) = ("
           << FormatHEX(X) << ", " << FormatHEX(Y) << ", " << FormatHEX(Z) << ") = ("
           << X << ", " << Y << ", " << Z << ")" << std::endl;
        stream.read(reinterpret_cast<char*>(&X), sizeof(X));
        stream.read(reinterpret_cast<char*>(&Y), sizeof(Y));
        stream.read(reinterpret_cast<char*>(&Z), sizeof(Z));
        ss << "\tVector Max (X, Y, Z) = ("
           << FormatHEX(X) << ", " << FormatHEX(Y) << ", " << FormatHEX(Z) << ") = ("
           << X << ", " << Y << ", " << Z << ")" << std::endl;
        uint8_t byteVal = 0;
        stream.read(reinterpret_cast<char*>(&byteVal), sizeof(byteVal));
        ss << "\tIsValid: " << FormatHEX(byteVal) << " = ";
        if (byteVal == 0)
            ss << "false\n";
        else
            ss << "true\n";
    }
    /// if it is big, it might be inner property list
    else if(TryUnsafe == true && PropertySize > 24)
    {
        UDefaultPropertiesList SomeProperties;
        ss << "Unsafe guess (it's a Property List):\n" << SomeProperties.Deserialize(stream, info, OwnerRef, true, false);
    }
    /// Guid?
    else if(TryUnsafe == true && PropertySize == 16)
    {
        FGuid GUID;
        stream.read(reinterpret_cast<char*>(&GUID), sizeof(GUID));
        ss << "\tUnsafe guess: GUID = " << FormatHEX(GUID) << std::endl;
    }
    /// if it is small, it might be NameIndex
    else if(TryUnsafe == true && PropertySize == 8)
    {
        UNameIndex value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        ss << "\tUnsafe guess:\n";
        ss << "\tName: " << FormatHEX(value) << " = " << info.IndexToName(value) << std::endl;
    }
    /// if it is even smaller, it might be integer (or float) or object reference
    else if(TryUnsafe == true && PropertySize == 4)
    {
        int32_t value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        ss << "\tUnsafe guess: "
           << "It's an Integer: " << FormatHEX((uint32_t)value) << " = " << value
           << " or a Reference: " << FormatHEX((uint32_t)value) << " -> " << info.ObjRefToName(value) << std::endl;
    }
    else
    {
        //stream.seekg(PropertySize, std::ios::cur);
        //ss << "\tUnknown property!\n";
        if (PropertySize <= info.GetExportEntry(OwnerRef).SerialSize)
        {
            std::vector<char> unk(PropertySize);
            stream.read(unk.data(), unk.size());
            ss << "\tUnknown property: " << FormatHEX(unk) << std::endl;
        }
    }
    return ss.str();
}

std::string UDefaultProperty::FindArrayType(std::string ArrName, std::istream& stream, UPKInfo& info)
{
    if (OwnerRef <= 0)
        return "None";
    FObjectExport OwnerEntry = info.GetExportEntry(OwnerRef);
    std::string OwnerName = OwnerEntry.FullName;
    size_t pos = OwnerName.find("Default__");
    if (pos != std::string::npos)
    {
        OwnerName = OwnerName.substr(pos + 9);
    }
    std::string FullName = OwnerName + "." + ArrName;
    UObjectReference ObjRef = info.FindObject(FullName);
    if (ObjRef <= 0)
        return "None";
    FObjectExport ArrayEntry = info.GetExportEntry(ObjRef);
    if (ArrayEntry.Type == "ArrayProperty")
    {
        UArrayProperty ArrProperty;
        size_t StreamPos = stream.tellg();
        stream.seekg(ArrayEntry.SerialOffset);
        /// quick-deserialize property, as we need only it's inner type info
        ArrProperty.SetRef(ObjRef);
        ArrProperty.SetUnsafe(false);
        ArrProperty.SetQuickMode(true);
        ArrProperty.Deserialize(stream, info);
        stream.seekg(StreamPos);
        if (ArrProperty.GetInner() <= 0)
            return "None";
        FObjectExport InnerEntry = info.GetExportEntry(ArrProperty.GetInner());
        return InnerEntry.Type;
    }
    return "None";
}

std::string UDefaultProperty::GuessArrayType(std::string ArrName)
{
    if (ArrName.find("Component") != std::string::npos)
    {
        return "ComponentProperty";
    }
    else if (ArrName.find("Class") != std::string::npos)
    {
        return "ClassProperty";
    }
    else if (ArrName.find("Interface") != std::string::npos)
    {
        return "InterfaceProperty";
    }
    else if (ArrName.find("Object") != std::string::npos)
    {
        return "ObjectProperty";
    }
    else if (ArrName.find("Name") != std::string::npos)
    {
        return "StringProperty";
    }
    return "None";
}

void UDefaultProperty::MakeByteProperty(std::string name, std::string innerName, std::string value, UPKInfo& info)
{
    Name = name;
    NameIdx = info.NameToUNameIndex(Name);
    Type = "ByteProperty";
    TypeIdx = info.NameToUNameIndex(Type);
    PropertySize = 8;
    ArrayIdx = 0;
    if (info.GetVersion() >= VER_XCOM)
        InnerNameIdx = info.NameToUNameIndex(innerName);
    valueNameIdx = info.NameToUNameIndex(value);
    InnerValue.resize(PropertySize);
    memcpy(InnerValue.data(), reinterpret_cast<char*>(&valueNameIdx), sizeof(valueNameIdx));
}

void UDefaultProperty::MakeNameProperty(std::string name, std::string value, UPKInfo& info)
{
    Name = name;
    NameIdx = info.NameToUNameIndex(Name);
    Type = "NameProperty";
    TypeIdx = info.NameToUNameIndex(Type);
    PropertySize = 8;
    ArrayIdx = 0;
    valueNameIdx = info.NameToUNameIndex(value);
    InnerValue.resize(PropertySize);
    memcpy(InnerValue.data(), reinterpret_cast<char*>(&valueNameIdx), sizeof(valueNameIdx));
}

void UDefaultProperty::MakeIntProperty(std::string name, int32_t value, UPKInfo& info)
{
    Name = name;
    NameIdx = info.NameToUNameIndex(Name);
    Type = "IntProperty";
    TypeIdx = info.NameToUNameIndex(Type);
    PropertySize = 4;
    ArrayIdx = 0;
    valueInt = value;
    InnerValue.resize(PropertySize);
    memcpy(InnerValue.data(), reinterpret_cast<char*>(&valueInt), sizeof(valueInt));
}

void UDefaultProperty::MakeBoolProperty(std::string name, bool value, UPKInfo& info)
{
    Name = name;
    NameIdx = info.NameToUNameIndex(Name);
    Type = "BoolProperty";
    TypeIdx = info.NameToUNameIndex(Type);
    PropertySize = 0;
    ArrayIdx = 0;
    if (info.GetVersion() >= VER_XCOM)
    {
        BoolValue = (uint8_t)value;
    }
    else
    {
        BoolValueOld = (uint32_t)value;
        BoolValue = (uint8_t)BoolValueOld;
    }
    InnerValue.clear();
}

std::string UDefaultProperty::Serialize(UPKInfo& info)
{
    ///not all the properties are yet added!
    std::ostringstream ss;
    ss.write(reinterpret_cast<char*>(&NameIdx), sizeof(NameIdx));
    if (Name != "None")
    {
        ss.write(reinterpret_cast<char*>(&TypeIdx), sizeof(TypeIdx));
        ss.write(reinterpret_cast<char*>(&PropertySize), sizeof(PropertySize));
        ss.write(reinterpret_cast<char*>(&ArrayIdx), sizeof(ArrayIdx));
        if (Type == "BoolProperty")
        {
            if (info.GetVersion() >= VER_XCOM)
                ss.write(reinterpret_cast<char*>(&BoolValue), sizeof(BoolValue));
            else
                ss.write(reinterpret_cast<char*>(&BoolValueOld), sizeof(BoolValueOld));
        }
        else
        {
            if (Type == "StructProperty" || Type == "ByteProperty")
            {
                if (info.GetVersion() >= VER_XCOM)
                    ss.write(reinterpret_cast<char*>(&InnerNameIdx), sizeof(InnerNameIdx));
            }
            if (InnerValue.size() > 0)
                ss.write(InnerValue.data(), InnerValue.size());
        }
    }
    return ss.str();
}

size_t UDefaultProperty::GetInnerValueOffset(uint16_t ver)
{
    if (Name == "None")
    {
        return 8;
    }
    else if (Type == "ByteProperty")
    {
        if (ver >= VER_XCOM)
            return 32; ///8+8+4+4+8
        return 24;
    }
    else
    {
        return 24; ///8+8+4+4
    }
}

std::string UObject::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << "UObject:\n";
    if (info.GetVersion() == VER_BATMAN_CITY)
    {
        stream.read(reinterpret_cast<char*>(&BCUnknown1), sizeof(BCUnknown1));
        ss << "\tUnknown1 = " << FormatHEX((uint32_t)BCUnknown1) << std::endl;
        stream.read(reinterpret_cast<char*>(&BCUnknown2), sizeof(BCUnknown2));
        ss << "\tUnknown2 = " << FormatHEX((uint32_t)BCUnknown2) << std::endl;
    }
    stream.read(reinterpret_cast<char*>(&ObjRef), sizeof(ObjRef));
    ss << "\tPrevObjRef = " << FormatHEX((uint32_t)ObjRef) << " -> " << info.ObjRefToName(ObjRef) << std::endl;
    if (info.GetVersion() == VER_BATMAN_CITY)
    {
        ss << BCUnkNames.Deserialize(stream, info, ThisRef);
        uint32_t pos = ((unsigned)stream.tellg() - info.GetExportEntry(ThisRef).SerialOffset);
        if (pos == info.GetExportEntry(ThisRef).SerialSize)
            return ss.str();
    }
    if (Type != GlobalType::UClass)
    {
        FObjectExport ThisTableEntry = info.GetExportEntry(ThisRef);
        if (TryUnsafe == true && ThisRef > 0 && (ThisTableEntry.ObjectFlagsL & (uint32_t)UObjectFlagsL::HasStack))
        {
            stream.seekg(22, std::ios::cur);
            ss << "Can't deserialize stack: skipping!\n";
        }
        ss << DefaultProperties.Deserialize(stream, info, ThisRef, TryUnsafe, QuickMode);
    }
    return ss.str();
}

std::string UField::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UObject::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UField:\n";
    FieldOffset = NextRefOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&NextRef), sizeof(NextRef));
    ss << "\tNextRef = " << FormatHEX((uint32_t)NextRef) << " -> " << info.ObjRefToName(NextRef) << std::endl;
    if (IsStructure())
    {
        stream.read(reinterpret_cast<char*>(&ParentRef), sizeof(ParentRef));
        ss << "\tParentRef = " << FormatHEX((uint32_t)ParentRef) << " -> " << info.ObjRefToName(ParentRef) << std::endl;
    }
    FieldSize = (unsigned)stream.tellg() - (unsigned)FieldOffset;
    return ss.str();
}

std::string UStruct::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UField::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UStruct:\n";
    StructOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&ScriptTextRef), sizeof(ScriptTextRef));
    ss << "\tScriptTextRef = " << FormatHEX((uint32_t)ScriptTextRef) << " -> " << info.ObjRefToName(ScriptTextRef) << std::endl;
    FirstChildRefOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&FirstChildRef), sizeof(FirstChildRef));
    ss << "\tFirstChildRef = " << FormatHEX((uint32_t)FirstChildRef) << " -> " << info.ObjRefToName(FirstChildRef) << std::endl;
    stream.read(reinterpret_cast<char*>(&CppTextRef), sizeof(CppTextRef));
    ss << "\tCppTextRef = " << FormatHEX((uint32_t)CppTextRef) << " -> " << info.ObjRefToName(CppTextRef) << std::endl;
    stream.read(reinterpret_cast<char*>(&Line), sizeof(Line));
    ss << "\tLine = " << FormatHEX(Line) << std::endl;
    stream.read(reinterpret_cast<char*>(&TextPos), sizeof(TextPos));
    ss << "\tTextPos = " << FormatHEX(TextPos) << std::endl;
    stream.read(reinterpret_cast<char*>(&ScriptMemorySize), sizeof(ScriptMemorySize));
    ss << "\tScriptMemorySize = " << FormatHEX(ScriptMemorySize) << std::endl;
    stream.read(reinterpret_cast<char*>(&ScriptSerialSize), sizeof(ScriptSerialSize));
    ss << "\tScriptSerialSize = " << FormatHEX(ScriptSerialSize) << std::endl;
    /// prevent allocation errors, caused by bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    DataScript.resize(ScriptSerialSize);
    ScriptOffset = stream.tellg();
    stream.read(DataScript.data(), DataScript.size());
    ss << "\tScript decompiler is not implemented!\n";
    StructSize = (unsigned)stream.tellg() - (unsigned)StructOffset;
    return ss.str();
}

std::string UFunction::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UStruct::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UFunction:\n";
    FunctionOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&NativeToken), sizeof(NativeToken));
    ss << "\tNativeToken = " << FormatHEX(NativeToken) << std::endl;
    stream.read(reinterpret_cast<char*>(&OperPrecedence), sizeof(OperPrecedence));
    ss << "\tOperPrecedence = " << FormatHEX(OperPrecedence) << std::endl;
    FlagsOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&FunctionFlags), sizeof(FunctionFlags));
    ss << "\tFunctionFlags = " << FormatHEX(FunctionFlags) << std::endl;
    ss << FormatFunctionFlags(FunctionFlags);
    if (FunctionFlags & (uint32_t)UFunctionFlags::Net)
    {
        stream.read(reinterpret_cast<char*>(&RepOffset), sizeof(RepOffset));
        ss << "\tRepOffset = " << FormatHEX(RepOffset) << std::endl;
    }
    stream.read(reinterpret_cast<char*>(&NameIdx), sizeof(NameIdx));
    ss << "\tNameIdx = " << FormatHEX(NameIdx) << " -> " << info.IndexToName(NameIdx) << std::endl;
    FunctionSize = (unsigned)stream.tellg() - (unsigned)FunctionOffset;
    return ss.str();
}

std::string UScriptStruct::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UStruct::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UScriptStruct:\n";
    ScriptStructOffset = stream.tellg();
    FlagsOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&StructFlags), sizeof(StructFlags));
    ss << "\tStructFlags = " << FormatHEX(StructFlags) << std::endl;
    ss << FormatStructFlags(StructFlags);
    ss << StructDefaultProperties.Deserialize(stream, info, ThisRef, TryUnsafe, QuickMode);
    ScriptStructSize = (unsigned)stream.tellg() - (unsigned)ScriptStructOffset;
    return ss.str();
}

std::string UState::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UStruct::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UState:\n";
    StateOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&ProbeMask), sizeof(ProbeMask));
    ss << "\tProbeMask = " << FormatHEX(ProbeMask) << std::endl;
    stream.read(reinterpret_cast<char*>(&LabelTableOffset), sizeof(LabelTableOffset));
    ss << "\tLabelTableOffset = " << FormatHEX(LabelTableOffset) << std::endl;
    FlagsOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&StateFlags), sizeof(StateFlags));
    ss << "\tStateFlags = " << FormatHEX(StateFlags) << std::endl;
    ss << FormatStateFlags(StateFlags);
    stream.read(reinterpret_cast<char*>(&StateMapSize), sizeof(StateMapSize));
    ss << "\tStateMapSize = " << FormatHEX(StateMapSize) << " (" << StateMapSize << ")" << std::endl;
    StateMap.clear();
    if (StateMapSize > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        StateMapSize = 0;
    for (unsigned i = 0; i < StateMapSize; ++i)
    {
        std::pair<UNameIndex, UObjectReference> MapElement;
        stream.read(reinterpret_cast<char*>(&MapElement), sizeof(MapElement));
        ss << "\tStateMap[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(MapElement.first) << " -> " << info.IndexToName(MapElement.first) << std::endl;
        ss << "\t\t" << FormatHEX((uint32_t)MapElement.second) << " -> " << info.ObjRefToName(MapElement.second) << std::endl;
        StateMap.push_back(MapElement);
    }
    StateSize = (unsigned)stream.tellg() - (unsigned)StateOffset;
    return ss.str();
}

std::string UClass::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UState::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UClass:\n";
    FlagsOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&ClassFlags), sizeof(ClassFlags));
    ss << "\tClassFlags = " << FormatHEX(ClassFlags) << std::endl;
    ss << FormatClassFlags(ClassFlags);
    stream.read(reinterpret_cast<char*>(&WithinRef), sizeof(WithinRef));
    ss << "\tWithinRef = " << FormatHEX((uint32_t)WithinRef) << " -> " << info.ObjRefToName(WithinRef) << std::endl;
    stream.read(reinterpret_cast<char*>(&ConfigNameIdx), sizeof(ConfigNameIdx));
    ss << "\tConfigNameIdx = " << FormatHEX(ConfigNameIdx) << " -> " << info.IndexToName(ConfigNameIdx) << std::endl;
    stream.read(reinterpret_cast<char*>(&NumComponents), sizeof(NumComponents));
    ss << "\tNumComponents = " << FormatHEX(NumComponents) << " (" << NumComponents << ")" << std::endl;
    Components.clear();
    if (NumComponents > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumComponents = 0;
    for (unsigned i = 0; i < NumComponents; ++i)
    {
        std::pair<UNameIndex, UObjectReference> MapElement;
        stream.read(reinterpret_cast<char*>(&MapElement), sizeof(MapElement));
        ss << "\tComponents[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(MapElement.first) << " -> " << info.IndexToName(MapElement.first) << std::endl;
        ss << "\t\t" << FormatHEX((uint32_t)MapElement.second) << " -> " << info.ObjRefToName(MapElement.second) << std::endl;
        Components.push_back(MapElement);
    }
    stream.read(reinterpret_cast<char*>(&NumInterfaces), sizeof(NumInterfaces));
    ss << "\tNumInterfaces = " << FormatHEX(NumInterfaces) << " (" << NumInterfaces << ")" << std::endl;
    Interfaces.clear();
    if (NumInterfaces > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumInterfaces = 0;
    for (unsigned i = 0; i < NumInterfaces; ++i)
    {
        std::pair<UObjectReference, uint32_t> MapElement;
        stream.read(reinterpret_cast<char*>(&MapElement), sizeof(MapElement));
        ss << "\tInterfaces[" << i << "]:\n";
        ss << "\t\t" << FormatHEX((uint32_t)MapElement.first) << " -> " << info.ObjRefToName(MapElement.first) << std::endl;
        ss << "\t\t" << FormatHEX(MapElement.second) << std::endl;
        Interfaces.push_back(MapElement);
    }
    stream.read(reinterpret_cast<char*>(&NumDontSortCategories), sizeof(NumDontSortCategories));
    ss << "\tNumDontSortCategories = " << FormatHEX(NumDontSortCategories) << " (" << NumDontSortCategories << ")" << std::endl;
    DontSortCategories.clear();
    if (NumDontSortCategories > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumDontSortCategories = 0;
    for (unsigned i = 0; i < NumDontSortCategories; ++i)
    {
        UNameIndex Element;
        stream.read(reinterpret_cast<char*>(&Element), sizeof(Element));
        ss << "\tDontSortCategories[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(Element) << " -> " << info.IndexToName(Element) << std::endl;
        DontSortCategories.push_back(Element);
    }
    stream.read(reinterpret_cast<char*>(&NumHideCategories), sizeof(NumHideCategories));
    ss << "\tNumHideCategories = " << FormatHEX(NumHideCategories) << " (" << NumHideCategories << ")" << std::endl;
    HideCategories.clear();
    if (NumHideCategories > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumHideCategories = 0;
    for (unsigned i = 0; i < NumHideCategories; ++i)
    {
        UNameIndex Element;
        stream.read(reinterpret_cast<char*>(&Element), sizeof(Element));
        ss << "\tHideCategories[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(Element) << " -> " << info.IndexToName(Element) << std::endl;
        HideCategories.push_back(Element);
    }
    stream.read(reinterpret_cast<char*>(&NumAutoExpandCategories), sizeof(NumAutoExpandCategories));
    ss << "\tNumAutoExpandCategories = " << FormatHEX(NumAutoExpandCategories) << " (" << NumAutoExpandCategories << ")" << std::endl;
    AutoExpandCategories.clear();
    if (NumAutoExpandCategories > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumAutoExpandCategories = 0;
    for (unsigned i = 0; i < NumAutoExpandCategories; ++i)
    {
        UNameIndex Element;
        stream.read(reinterpret_cast<char*>(&Element), sizeof(Element));
        ss << "\tAutoExpandCategories[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(Element) << " -> " << info.IndexToName(Element) << std::endl;
        AutoExpandCategories.push_back(Element);
    }
    stream.read(reinterpret_cast<char*>(&NumAutoCollapseCategories), sizeof(NumAutoCollapseCategories));
    ss << "\tNumAutoCollapseCategories = " << FormatHEX(NumAutoCollapseCategories) << " (" << NumAutoCollapseCategories << ")" << std::endl;
    AutoCollapseCategories.clear();
    if (NumAutoCollapseCategories > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumAutoCollapseCategories = 0;
    for (unsigned i = 0; i < NumAutoCollapseCategories; ++i)
    {
        UNameIndex Element;
        stream.read(reinterpret_cast<char*>(&Element), sizeof(Element));
        ss << "\tAutoCollapseCategories[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(Element) << " -> " << info.IndexToName(Element) << std::endl;
        AutoCollapseCategories.push_back(Element);
    }
    stream.read(reinterpret_cast<char*>(&ForceScriptOrder), sizeof(ForceScriptOrder));
    ss << "\tForceScriptOrder = " << FormatHEX(ForceScriptOrder) << std::endl;
    stream.read(reinterpret_cast<char*>(&NumClassGroups), sizeof(NumClassGroups));
    ss << "\tNumClassGroups = " << FormatHEX(NumClassGroups) << " (" << NumClassGroups << ")" << std::endl;
    ClassGroups.clear();
    if (NumClassGroups > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NumClassGroups = 0;
    for (unsigned i = 0; i < NumClassGroups; ++i)
    {
        UNameIndex Element;
        stream.read(reinterpret_cast<char*>(&Element), sizeof(Element));
        ss << "\tClassGroups[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(Element) << " -> " << info.IndexToName(Element) << std::endl;
        ClassGroups.push_back(Element);
    }
    stream.read(reinterpret_cast<char*>(&NativeClassNameLength), sizeof(NativeClassNameLength));
    ss << "\tNativeClassNameLength = " << FormatHEX(NativeClassNameLength) << std::endl;
    if (NativeClassNameLength > info.GetExportEntry(ThisRef).SerialSize) /// bad data malloc error prevention
        NativeClassNameLength = 0;
    if (NativeClassNameLength > 0)
    {
        getline(stream, NativeClassName, '\0');
        ss << "\tNativeClassName = " << NativeClassName << std::endl;
    }
    stream.read(reinterpret_cast<char*>(&DLLBindName), sizeof(DLLBindName));
    ss << "\tDLLBindName = " << FormatHEX(DLLBindName) << " -> " << info.IndexToName(DLLBindName) << std::endl;
    stream.read(reinterpret_cast<char*>(&DefaultRef), sizeof(DefaultRef));
    ss << "\tDefaultRef = " << FormatHEX((uint32_t)DefaultRef) << " -> " << info.ObjRefToName(DefaultRef) << std::endl;
    return ss.str();
}

std::string UConst::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UField::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UConst:\n";
    stream.read(reinterpret_cast<char*>(&ValueLength), sizeof(ValueLength));
    ss << "\tValueLength = " << FormatHEX(ValueLength) << std::endl;
    if (ValueLength > 0)
    {
        getline(stream, Value, '\0');
        ss << "\tValue = " << Value << std::endl;
    }
    return ss.str();
}

std::string UEnum::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UField::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UEnum:\n";
    stream.read(reinterpret_cast<char*>(&NumNames), sizeof(NumNames));
    ss << "\tNumNames = " << FormatHEX(NumNames) << " (" << NumNames << ")" << std::endl;
    Names.clear();
    for (unsigned i = 0; i < NumNames; ++i)
    {
        UNameIndex Element;
        stream.read(reinterpret_cast<char*>(&Element), sizeof(Element));
        ss << "\tNames[" << i << "]:\n";
        ss << "\t\t" << FormatHEX(Element) << " -> " << info.IndexToName(Element) << std::endl;
        Names.push_back(Element);
    }
    return ss.str();
}

std::string UProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UField::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UProperty:\n";
    uint32_t tmpVal;
    stream.read(reinterpret_cast<char*>(&tmpVal), sizeof(tmpVal));
    ArrayDim = tmpVal % (1 << 16);
    ElementSize = tmpVal >> 16;
    ss << "\tArrayDim = " << FormatHEX(ArrayDim) << " (" << ArrayDim << ")" << std::endl;
    ss << "\tElementSize = " << FormatHEX(ElementSize) << " (" << ElementSize << ")" << std::endl;
    FlagsOffset = stream.tellg();
    stream.read(reinterpret_cast<char*>(&PropertyFlagsL), sizeof(PropertyFlagsL));
    ss << "\tPropertyFlagsL = " << FormatHEX(PropertyFlagsL) << std::endl;
    ss << FormatPropertyFlagsL(PropertyFlagsL);
    stream.read(reinterpret_cast<char*>(&PropertyFlagsH), sizeof(PropertyFlagsH));
    ss << "\tPropertyFlagsH = " << FormatHEX(PropertyFlagsH) << std::endl;
    ss << FormatPropertyFlagsH(PropertyFlagsH);
    stream.read(reinterpret_cast<char*>(&CategoryIndex), sizeof(CategoryIndex));
    ss << "\tCategoryIndex = " << FormatHEX(CategoryIndex) << " -> " << info.IndexToName(CategoryIndex) << std::endl;
    stream.read(reinterpret_cast<char*>(&ArrayEnumRef), sizeof(ArrayEnumRef));
    ss << "\tArrayEnumRef = " << FormatHEX((uint32_t)ArrayEnumRef) << " -> " << info.ObjRefToName(ArrayEnumRef) << std::endl;
    if (PropertyFlagsL & (uint32_t)UPropertyFlagsL::Net)
    {
        stream.read(reinterpret_cast<char*>(&RepOffset), sizeof(RepOffset));
        ss << "\tRepOffset = " << FormatHEX(RepOffset) << std::endl;
    }
    return ss.str();
}

std::string UByteProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UByteProperty:\n";
    stream.read(reinterpret_cast<char*>(&EnumObjRef), sizeof(EnumObjRef));
    ss << "\tEnumObjRef = " << FormatHEX((uint32_t)EnumObjRef) << " -> " << info.ObjRefToName(EnumObjRef) << std::endl;
    return ss.str();
}

std::string UObjectProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UObjectProperty:\n";
    stream.read(reinterpret_cast<char*>(&OtherObjRef), sizeof(OtherObjRef));
    ss << "\tOtherObjRef = " << FormatHEX((uint32_t)OtherObjRef) << " -> " << info.ObjRefToName(OtherObjRef) << std::endl;
    return ss.str();
}

std::string UClassProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UObjectProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UClassProperty:\n";
    stream.read(reinterpret_cast<char*>(&ClassObjRef), sizeof(ClassObjRef));
    ss << "\tClassObjRef = " << FormatHEX((uint32_t)ClassObjRef) << " -> " << info.ObjRefToName(ClassObjRef) << std::endl;
    return ss.str();
}

std::string UStructProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UStructProperty:\n";
    stream.read(reinterpret_cast<char*>(&StructObjRef), sizeof(StructObjRef));
    ss << "\tStructObjRef = " << FormatHEX((uint32_t)StructObjRef) << " -> " << info.ObjRefToName(StructObjRef) << std::endl;
    return ss.str();
}

std::string UFixedArrayProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UFixedArrayProperty:\n";
    stream.read(reinterpret_cast<char*>(&InnerObjRef), sizeof(InnerObjRef));
    ss << "\tInnerObjRef = " << FormatHEX((uint32_t)InnerObjRef) << " -> " << info.ObjRefToName(InnerObjRef) << std::endl;
    stream.read(reinterpret_cast<char*>(&Count), sizeof(Count));
    ss << "\tCount = " << FormatHEX(Count) << " (" << Count << ")" << std::endl;
    return ss.str();
}

std::string UArrayProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UArrayProperty:\n";
    stream.read(reinterpret_cast<char*>(&InnerObjRef), sizeof(InnerObjRef));
    ss << "\tInnerObjRef = " << FormatHEX((uint32_t)InnerObjRef) << " -> " << info.ObjRefToName(InnerObjRef) << std::endl;
    return ss.str();
}

std::string UDelegateProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UDelegateProperty:\n";
    stream.read(reinterpret_cast<char*>(&FunctionObjRef), sizeof(FunctionObjRef));
    ss << "\tFunctionObjRef = " << FormatHEX((uint32_t)FunctionObjRef) << " -> " << info.ObjRefToName(FunctionObjRef) << std::endl;
    stream.read(reinterpret_cast<char*>(&DelegateObjRef), sizeof(DelegateObjRef));
    ss << "\tDelegateObjRef = " << FormatHEX((uint32_t)DelegateObjRef) << " -> " << info.ObjRefToName(DelegateObjRef) << std::endl;
    return ss.str();
}

std::string UInterfaceProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UInterfaceProperty:\n";
    stream.read(reinterpret_cast<char*>(&InterfaceObjRef), sizeof(InterfaceObjRef));
    ss << "\tInterfaceObjRef = " << FormatHEX((uint32_t)InterfaceObjRef) << " -> " << info.ObjRefToName(InterfaceObjRef) << std::endl;
    return ss.str();
}

std::string UMapProperty::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UProperty::Deserialize(stream, info);
    /// check for bad data
    if ((unsigned)stream.tellg() > info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize)
        return ss.str();
    ss << "UMapProperty:\n";
    stream.read(reinterpret_cast<char*>(&KeyObjRef), sizeof(KeyObjRef));
    ss << "\tKeyObjRef = " << FormatHEX((uint32_t)KeyObjRef) << " -> " << info.ObjRefToName(KeyObjRef) << std::endl;
    stream.read(reinterpret_cast<char*>(&ValueObjRef), sizeof(ValueObjRef));
    ss << "\tValueObjRef = " << FormatHEX((uint32_t)ValueObjRef) << " -> " << info.ObjRefToName(ValueObjRef) << std::endl;
    return ss.str();
}

std::string ULevel::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    UObjectReference A;
    uint32_t NumActors;
    ss << UObject::Deserialize(stream, info);
    uint32_t pos = ((unsigned)stream.tellg() - info.GetExportEntry(ThisRef).SerialOffset);
    ss << "ULevel:\n";
    stream.read(reinterpret_cast<char*>(&A), sizeof(A));
    ss << "\tLevel object: " << FormatHEX((uint32_t)A) << " -> " << info.ObjRefToName(A) << std::endl;
    stream.read(reinterpret_cast<char*>(&NumActors), sizeof(NumActors));
    ss << "\tNum actors: " << FormatHEX(NumActors) << " = " << NumActors << std::endl;
    stream.read(reinterpret_cast<char*>(&A), sizeof(A));
    ss << "\tWorldInfo object: " << FormatHEX((uint32_t)A) << " -> " << info.ObjRefToName(A) << std::endl;
    ss << "\tActors:\n";
    for (unsigned i = 0; i < NumActors; ++i)
    {
        stream.read(reinterpret_cast<char*>(&A), sizeof(A));
        Actors.push_back(A);
        ss << "\t\t" << FormatHEX((char*)&A, sizeof(A)) << "\t//\t" << FormatHEX((uint32_t)A) << " -> " << info.ObjRefToName(A) << std::endl;
    }
    pos = ((unsigned)stream.tellg() - info.GetExportEntry(ThisRef).SerialOffset);
    ss << "Stream relative position (debug info): " << FormatHEX(pos) << " (" << pos << ")\n";
    ss << "Object unknown, can't deserialize!\n";
    return ss.str();
}

std::string USurface::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UObject::Deserialize(stream, info);
    return ss.str();
}

std::string UTexture::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << USurface::Deserialize(stream, info);
    return ss.str();
}

std::string UTexture2D::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << UTexture::Deserialize(stream, info);

    ///attempt to init vars from default property list
    std::vector<UDefaultProperty> props = DefaultProperties.GetDefaultProperties();
    for (unsigned i = 0; i < props.size(); ++i)
    {
        SetClassVarFromProperty(props[i], info);
    }

    ss << "UTexture2D:\n";

    stream.read(reinterpret_cast<char*>(&Unknown1), sizeof(Unknown1));
    ss << "\tUnknown1: " << FormatHEX(Unknown1) << std::endl;
    stream.read(reinterpret_cast<char*>(&Unknown2), sizeof(Unknown2));
    ss << "\tUnknown2: " << FormatHEX(Unknown2) << std::endl;
    stream.read(reinterpret_cast<char*>(&Unknown3), sizeof(Unknown3));
    ss << "\tUnknown3: " << FormatHEX(Unknown3) << std::endl;

    size_t maxOffset = info.GetExportEntry(ThisRef).SerialOffset + info.GetExportEntry(ThisRef).SerialSize;

    uint32_t absOffset;
    stream.read(reinterpret_cast<char*>(&absOffset), sizeof(absOffset));
    ss << "\tBulk data absolute file offset: " << FormatHEX(absOffset) << " (" << absOffset << ")" << std::endl;
    if (absOffset > maxOffset)
    {
        ss << "Error deserializing Texture2D: bulk data file offset out of bounds!\n";
        return ss.str();
    }

    stream.read(reinterpret_cast<char*>(&MipMapCount), sizeof(MipMapCount));
    ss << "\tNum of mipmaps stored: " << FormatHEX(MipMapCount) << " (" << MipMapCount << ")" << std::endl;

    MipMaps.clear();
    MipMaps.reserve(MipMapCount);
    for (unsigned i = 0; i < MipMapCount; ++i)
    {
        UTexture2DMipMap nextData;
        ss << "UTexture2DMipMap[" << i << "]:\n";
        if (!DoNotReadTFC && !QuickMode)
            nextData.SetExternalFileName(TextureFileCacheName + ".tfc");
        ss << nextData.Deserialize(stream, info, ThisRef);
        if (stream.tellg() > maxOffset)
        {
            ss << "Error deserializing Texture2D: offset out of bounds!\n";
            return ss.str();
        }
        MipMaps.push_back(nextData);
    }
    MipMapCount = MipMaps.size();
    if (MipMaps.size() > 0)
    {
        if (Width == 0)
            Width = MipMaps[0].GetSizeX();
        if (Height == 0)
            Height = MipMaps[0].GetSizeY();
        PitchOrLinearSize = MipMaps[0].GetSavedElementCount();
    }
    if (stream.tellg() < maxOffset)
    {
        UnknownData.resize(maxOffset - stream.tellg());
        stream.read(UnknownData.data(), UnknownData.size());
        ss << "\tUnknown data size: " << UnknownData.size() << std::endl;
        ss << "\tUnknown data: " << FormatHEX(UnknownData) << std::endl;
    }

    return ss.str();
}

std::string UTexture2D::SerializeTexture2DData(size_t offset)
{
    std::stringstream ss;

    ///Texture2D internal data format:
    ///an empty bulk data chunk (12 zero bytes + 4 bytes of absolute file offset)
    ///MipMapCount (4 bytes)
    ///serialized MipMaps[MipMapCount]
    ///unknown data

    ss.write(reinterpret_cast<char*>(&Unknown1), 4);
    ss.write(reinterpret_cast<char*>(&Unknown2), 4);
    ss.write(reinterpret_cast<char*>(&Unknown3), 4);
    ///absolute file offset for the data following this absolute file offset
    size_t nextDataOffset = offset + ss.tellp() + 4;
    ss.write(reinterpret_cast<char*>(&nextDataOffset), 4);
    ss.write(reinterpret_cast<char*>(&MipMapCount), 4);

    ///serialize MipMaps
    for (unsigned i = 0; i < MipMapCount; ++i)
    {
        ss << MipMaps[i].Serialize(offset + ss.tellp());
    }

    ///serialize unknown data
    if (UnknownData.size() > 0)
        ss.write(UnknownData.data(), UnknownData.size());

    return ss.str();
}

bool UTexture2D::ExportToExternalFile(CustomTFC& T2DFile, UPKInfo& info, bool compressedOnly)
{
    bool success = false;
    for (unsigned i = 0; i < MipMapCount; ++i)
    {
        if (MipMaps[i].IsDataCompressed() || !compressedOnly)
        {
            MipMaps[i].SetExternalFileName(TextureFileCacheName + ".tfc");
            success |= MipMaps[i].ExportToExternalFile(T2DFile, info.GetExportEntry(ThisRef).FullName);
        }
    }
    return success;
}

bool UTexture2D::TryLzoCompression(int minResolution)
{
    bool success = false;
    for (unsigned i = 0; i < MipMapCount; ++i)
    {
        if (minResolution < 0 || (int)MipMaps[i].GetSizeX() >= minResolution || (int)MipMaps[i].GetSizeY() >= minResolution)
            success |= MipMaps[i].TryLzoCompression();
    }
    return success;
}

uint32_t UTexture2D::CalculateTexture2DDataSize()
{
    uint32_t res = 0;
    res += 12 + 4 + 4; ///12 zero bytes + 4 bytes of absolute file offset + 4 bytes of MipMapCount
    for (unsigned i = 0; i < MipMapCount; ++i)
    {
        res += MipMaps[i].CalculateSerializedSize();
    }
    res += UnknownData.size();
    return res;
}

void UTexture2D::SetClassVarFromProperty(UDefaultProperty& property, UPKInfo& info)
{
    if (property.GetName() == "Format" && property.GetType() == "ByteProperty")
        Format = info.IndexToName(property.GetValueNameIdx());
    else if (property.GetName() == "LODGroup" && property.GetType() == "ByteProperty")
        LODGroup = info.IndexToName(property.GetValueNameIdx());
    else if (property.GetName() == "TextureFileCacheName" && property.GetType() == "NameProperty")
        TextureFileCacheName = info.IndexToName(property.GetValueNameIdx());
    else if (property.GetName() == "SizeX" && property.GetType() == "IntProperty")
        Width = property.GetValueInt();
    else if (property.GetName() == "SizeY" && property.GetType() == "IntProperty")
        Height = property.GetValueInt();
    else if (property.GetName() == "MipTailBaseIdx" && property.GetType() == "IntProperty")
        MipTailBaseIdx = property.GetValueInt();
    else if (property.GetName() == "LODBias" && property.GetType() == "IntProperty")
        LODBias = property.GetValueInt();
    else if (property.GetName() == "NeverStream" && property.GetType() == "BoolProperty")
        NeverStream = property.GetValueBool();
    else
        UTexture::SetClassVarFromProperty(property, info);
}

std::string UObjectUnknown::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    /// prevent crashes while deserializing components
    if (info.GetExportEntry(ThisRef).Type.find("Component") != std::string::npos)
    {
        ss << "Can't deserialize Components!\n";
        return ss.str();
    }
    /// prevent crashes while deserializing FX_
    if (info.GetExportEntry(ThisRef).Type.find("BodySetup") != std::string::npos)
    {
        ss << "Can't deserialize BodySetup!\n";
        return ss.str();
    }
    /// to be on a safe side: don't deserialize unknown objects
    if (TryUnsafe == true)
    {
        ss << UObject::Deserialize(stream, info);
        uint32_t pos = ((unsigned)stream.tellg() - info.GetExportEntry(ThisRef).SerialOffset);
        ss << "Stream relative position (debug info): " << FormatHEX(pos) << " (" << pos << ")\n";
        if (pos == info.GetExportEntry(ThisRef).SerialSize)
            return ss.str();
    }
    ss << "UObjectUnknown:\n";
    ss << "\tObject unknown, can't deserialize!\n";
    return ss.str();
}
