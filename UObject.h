#ifndef UOBJECT_H
#define UOBJECT_H

#include <vector>
#include <string>
#include <iostream>
#include <utility>

#include "UPKInfo.h"
#include "CustomTFC.h"

/// global type enumeration
enum class GlobalType
{
	None            =  0,
	UObject         =  1,
	UField          =  2,
	UConst          =  3,
	UEnum           =  4,
	UProperty       =  5,
	UByteProperty   =  6,
	UIntProperty    =  7,
	UBoolProperty   =  8,
	UFloatProperty  =  9,
	UObjectProperty = 10,
	UClassProperty  = 11,
	UNameProperty   = 12,
	UStructProperty = 13,
	UStrProperty    = 14,
	UArrayProperty  = 15,
	UStruct         = 16,
	UScriptStruct   = 17,
	UFunction       = 18,
	UState          = 19,
	UClass          = 20,
	UTextBuffer     = 21,
	UObjectUnknown  = 22,
	UFixedArrayProperty  = 23,
	UComponentProperty = 24,
	UDelegateProperty = 25,
	UInterfaceProperty = 26,
	UMapProperty = 27,
	ULevel = 28,
	USurface = 29,
	UTexture = 30,
	UTexture2D = 31
};

/*///pixel format enums (Texture2D)
enum class EPixelFormat
{
    PF_Unknown,
    PF_A32B32G32R32F,
    PF_A8R8G8B8,
    PF_G8,
    PF_G16,
    PF_DXT1,
    PF_DXT2,
    PF_DXT3,
    PF_DXT4,
    PF_DXT5,
    PF_UYVY,
    PF_FloatRGB,
    PF_FloatRGBA,
    PF_DepthStencil,
    PF_ShadowDepth,
    PF_FilteredShadowDepth,
    PF_R32F,
    PF_G16R16,
    PF_G16R16F,
    PF_G32R32F,
    PF_A2B10G10R10,
    PF_A16B16G16R16,
    PF_D24
};*/

class UBulkDataMirror
{
public:
    UBulkDataMirror() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner);
    std::string Serialize();
    std::string Serialize(size_t offset);
    bool ExportToExternalFile(CustomTFC& T2DFile, std::string ObjName);
    bool TryLzoCompression();
    uint32_t CalculateSerializedSize();
    ///flags
    bool IsDataCompressed();
    bool IsDataStoredElsewhere();
    bool IsDataEmpty();
    bool GetDataFlag(UBulkDataFlags flg);
    void SetDataFlag(UBulkDataFlags flg, bool val);
    void ToggleDataFlag(UBulkDataFlags flg);
    ///setters
    void SetEmpty();
    void SetBulkDataRaw(std::vector<char> Data);
    void SetBulkData(std::vector<char> Data) { BulkData = Data; }
    void SetFileOffset(size_t offset) { SavedBulkDataOffsetInFile = offset; }
    void SetSavedBulkDataFlags(uint32_t flags) { SavedBulkDataFlags = flags; }
    void SetSavedElementCount(uint32_t elementCount) { SavedElementCount = elementCount; }
    void SetExternalFileName(std::string filename) { externalFileName = filename; }
    ///getters
    std::vector<char> GetBulkData() { return BulkData; }
    size_t GetBulkDataRelOffset() { return 16; }
    uint32_t GetSavedBulkDataFlags() { return SavedBulkDataFlags; }
    uint32_t GetSavedElementCount() { return SavedElementCount; }
    uint32_t GetSavedBulkDataSizeOnDisk() { return SavedBulkDataSizeOnDisk; }
    uint32_t GetSavedBulkDataOffsetInFile() { return SavedBulkDataOffsetInFile; }
    std::string GetExternalFileName() { return externalFileName; }
    bool GetWasCompressed() { return WasCompressed; }
    bool GetWasInExternalFile() { return WasInExternalFile; }
protected:
    /// persistent
    uint32_t SavedBulkDataFlags = 0;
    uint32_t SavedElementCount = 0;
    uint32_t SavedBulkDataSizeOnDisk = 0;
    uint32_t SavedBulkDataOffsetInFile = 0xFFFFFFFF;
    std::vector<char> BulkData;
    /// memory
    std::string externalFileName;
    bool DoNotCompress = false;
    bool LockFileOffset = false;
    bool WasCompressed = false;
    bool WasInExternalFile = false;
    /// internal methods
    bool ReadDataChunkFromExternalFile();
};

class UTexture2DMipMap : public UBulkDataMirror
{
public:
    UTexture2DMipMap() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner);
    std::string Serialize(size_t offset);
    uint32_t CalculateSerializedSize();
    uint32_t GetSizeX() { return SizeX; }
    uint32_t GetSizeY() { return SizeY; }
    void SetSizeX(uint32_t x) { SizeX = x; }
    void SetSizeY(uint32_t y) { SizeY = y; }
protected:
    /// persistent
    uint32_t SizeX = 0;
    uint32_t SizeY = 0;
};

class UDefaultProperty
{
public:
    /// constructors and destructors
    UDefaultProperty(): Name("None"), Type("None"), OwnerRef(0), TryUnsafe(0), QuickMode(0) {}
    ~UDefaultProperty() {}
    void Init(UObjectReference owner, bool unsafe = false, bool quick = false) { OwnerRef = owner; TryUnsafe = unsafe; QuickMode = quick; }
    void MakeByteProperty(std::string name, std::string innerName, std::string value, UPKInfo& info);
    void MakeNameProperty(std::string name, std::string value, UPKInfo& info);
    void MakeIntProperty(std::string name, int32_t value, UPKInfo& info);
    void MakeBoolProperty(std::string name, bool value, UPKInfo& info);
    ///serialization and deserialization
    std::string Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner, bool unsafe = false, bool quick = false);
    std::string DeserializeValue(std::istream& stream, UPKInfo& info);
    std::string FindArrayType(std::string ArrName, std::istream& stream, UPKInfo& info);
    std::string GuessArrayType(std::string ArrName);
    std::string Serialize(UPKInfo& info);
    /// getters
    size_t GetInnerValueOffset(uint16_t ver = VER_XCOM);
    std::string GetName() { return Name; }
    std::string GetType() { return Type; }
    UNameIndex GetValueNameIdx() { return valueNameIdx; }
    uint32_t GetValueInt() { return valueInt; }
    uint32_t GetValueBool() { return BoolValue; }
protected:
    /// persistent
    UNameIndex NameIdx;
    UNameIndex TypeIdx;
    uint32_t PropertySize;
    uint32_t ArrayIdx;
    uint8_t  BoolValue;       /// for BoolProperty only
    uint32_t BoolValueOld;    /// older engine versions (batman)
    UNameIndex InnerNameIdx;  /// for StructProperty and ByteProperty only
    std::vector<char> InnerValue;
    /// memory
    std::string Name;
    std::string Type;
    UObjectReference OwnerRef;
    bool TryUnsafe;
    bool QuickMode;
    /// possible values
    int32_t valueInt = 0;
    float valueFloat = 0;
    UObjectReference valueObjRef;
    UNameIndex valueNameIdx;
    std::string valueString;
};

class UDefaultPropertiesList
{
public:
    UDefaultPropertiesList() {}
    ~UDefaultPropertiesList() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner, bool unsafe = false, bool quick = false);
    std::vector<UDefaultProperty> GetDefaultProperties() { return DefaultProperties; }
protected:
    std::vector<UDefaultProperty> DefaultProperties;
    size_t PropertyOffset;
    size_t PropertySize;
};

class BCArrayOfNames
{
public:
    BCArrayOfNames() {}
    ~BCArrayOfNames() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info, UObjectReference owner);
protected:
    std::vector<UNameIndex> Names;
    uint32_t NamesNum;
};

/// parent class of all Unreal objects
class UObject
{
public:
    UObject(): Type(GlobalType::UObject), ThisRef(0), FlagsOffset(0), TryUnsafe(0), QuickMode(0) {}
    virtual ~UObject() {}
    virtual std::string Deserialize(std::istream& stream, UPKInfo& info);
    void SetRef(UObjectReference thisRef) { ThisRef = thisRef; }
    void SetUnsafe(bool val) { TryUnsafe = val; }
    void SetQuickMode(bool val) { QuickMode = val; }
    virtual bool IsStructure() { return false; }
    virtual bool IsProperty() { return false; }
    virtual bool IsState() { return false; }
    void SetClassVarFromProperty(UDefaultProperty& property, UPKInfo& info) {}
protected:
    /// persistent
    uint16_t BCUnknown1;                /// batman city unknown var
    uint32_t BCUnknown2;                /// batman city unknown var
    UObjectReference ObjRef;            /// Next object (Linker-related)
    UDefaultPropertiesList DefaultProperties; /// for non-Class objects only
    BCArrayOfNames BCUnkNames;          /// batman city
    /// memory
    GlobalType Type;
    UObjectReference ThisRef;
    size_t FlagsOffset;
    bool TryUnsafe;
    bool QuickMode;
};

class UObjectNone: public UObject
{
public:
    UObjectNone() { Type = GlobalType::None; }
    ~UObjectNone() {}
protected:
};

class UField: public UObject
{
public:
    UField() { Type = GlobalType::UField; }
    ~UField() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    UObjectReference GetNextRef() { return NextRef; }
    size_t GetNextRefOffset() { return NextRefOffset; }
protected:
    /// persistent
    UObjectReference NextRef;
    UObjectReference ParentRef; /// for Struct objects only
    /// memory
    size_t FieldOffset;
    size_t FieldSize;
    size_t NextRefOffset;
};

class UStruct: public UField
{
public:
    UStruct() { Type = GlobalType::UStruct; }
    ~UStruct() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    bool IsStructure() { return true; }
    UObjectReference GetFirstChildRef() { return FirstChildRef; }
    uint32_t GetScriptSerialSize() { return ScriptSerialSize; }
    uint32_t GetScriptMemorySize() { return ScriptMemorySize; }
    size_t GetScriptOffset() { return ScriptOffset; }
    size_t GetFirstChildRefOffset() { return FirstChildRefOffset; }
protected:
    /// persistent
    UObjectReference ScriptTextRef;
    UObjectReference FirstChildRef;
    UObjectReference CppTextRef;
    uint32_t Line;
    uint32_t TextPos;
    uint32_t ScriptMemorySize;
    uint32_t ScriptSerialSize;
    std::vector<char> DataScript;
    /// memory
    size_t StructOffset;
    size_t StructSize;
    size_t ScriptOffset;
    size_t FirstChildRefOffset;
};

class UFunction: public UStruct
{
public:
    UFunction() { Type = GlobalType::UFunction; }
    ~UFunction() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    uint16_t NativeToken;
    uint8_t OperPrecedence;
    uint32_t FunctionFlags;
    uint16_t RepOffset;
    UNameIndex NameIdx;
    /// memory
    size_t FunctionOffset;
    size_t FunctionSize;
};

class UScriptStruct: public UStruct
{
public:
    UScriptStruct() { Type = GlobalType::UScriptStruct; }
    ~UScriptStruct() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    uint32_t StructFlags;
    UDefaultPropertiesList StructDefaultProperties;
    /// memory
    size_t ScriptStructOffset;
    size_t ScriptStructSize;
};

class UState: public UStruct
{
public:
    UState() { Type = GlobalType::UState; }
    ~UState() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    bool IsState() { return true; }
protected:
    /// persistent
    uint32_t ProbeMask;
    uint16_t LabelTableOffset;
    uint32_t StateFlags;
    uint32_t StateMapSize;
    std::vector<std::pair<UNameIndex, UObjectReference> > StateMap;
    /// memory
    size_t StateOffset;
    size_t StateSize;
};

class UClass: public UState
{
public:
    UClass() { Type = GlobalType::UClass; }
    ~UClass() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    uint32_t ClassFlags;
    UObjectReference WithinRef;
    UNameIndex ConfigNameIdx;
    uint32_t NumComponents;
    std::vector<std::pair<UNameIndex, UObjectReference> > Components;
    uint32_t NumInterfaces;
    std::vector<std::pair<UObjectReference, uint32_t> > Interfaces;
    uint32_t NumDontSortCategories;
    std::vector<UNameIndex> DontSortCategories;
    uint32_t NumHideCategories;
    std::vector<UNameIndex> HideCategories;
    uint32_t NumAutoExpandCategories;
    std::vector<UNameIndex> AutoExpandCategories;
    uint32_t NumAutoCollapseCategories;
    std::vector<UNameIndex> AutoCollapseCategories;
    uint32_t ForceScriptOrder;
    uint32_t NumClassGroups;
    std::vector<UNameIndex> ClassGroups;
    uint32_t NativeClassNameLength;
    std::string NativeClassName;
    UNameIndex DLLBindName;
    UObjectReference DefaultRef;
};

class UConst: public UField
{
public:
    UConst() { Type = GlobalType::UConst; }
    ~UConst() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    uint32_t ValueLength;
    std::string Value;
};

class UEnum: public UField
{
public:
    UEnum() { Type = GlobalType::UEnum; }
    ~UEnum() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    uint32_t NumNames;
    std::vector<UNameIndex> Names;
};

class UProperty: public UField
{
public:
    UProperty() { Type = GlobalType::UProperty; }
    ~UProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    bool IsProperty() { return true; }
protected:
    /// persistent
    uint16_t ArrayDim;
    uint16_t ElementSize;
    uint32_t PropertyFlagsL;
    uint32_t PropertyFlagsH;
    UNameIndex CategoryIndex;
    UObjectReference ArrayEnumRef;
    uint16_t RepOffset;
};

class UByteProperty: public UProperty
{
public:
    UByteProperty() { Type = GlobalType::UByteProperty; }
    ~UByteProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference EnumObjRef;
};

class UIntProperty: public UProperty
{
public:
    UIntProperty() { Type = GlobalType::UIntProperty; }
    ~UIntProperty() {}
protected:
};

class UBoolProperty: public UProperty
{
public:
    UBoolProperty() { Type = GlobalType::UBoolProperty; }
    ~UBoolProperty() {}
protected:
};

class UFloatProperty: public UProperty
{
public:
    UFloatProperty() { Type = GlobalType::UFloatProperty; }
    ~UFloatProperty() {}
protected:
};

class UObjectProperty: public UProperty
{
public:
    UObjectProperty() { Type = GlobalType::UObjectProperty; }
    ~UObjectProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference OtherObjRef;
};

class UClassProperty: public UObjectProperty
{
public:
    UClassProperty() { Type = GlobalType::UClassProperty; }
    ~UClassProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference ClassObjRef;
};

class UComponentProperty: public UObjectProperty
{
public:
    UComponentProperty() { Type = GlobalType::UComponentProperty; }
    ~UComponentProperty() {}
protected:
};

class UNameProperty: public UProperty
{
public:
    UNameProperty() { Type = GlobalType::UNameProperty; }
    ~UNameProperty() {}
protected:
};

class UStructProperty: public UProperty
{
public:
    UStructProperty() { Type = GlobalType::UStructProperty; }
    ~UStructProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference StructObjRef;
};

class UStrProperty: public UProperty
{
public:
    UStrProperty() { Type = GlobalType::UStrProperty; }
    ~UStrProperty() {}
protected:
};

class UFixedArrayProperty: public UProperty
{
public:
    UFixedArrayProperty() { Type = GlobalType::UFixedArrayProperty; }
    ~UFixedArrayProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference InnerObjRef;
    uint32_t Count;
};

class UArrayProperty: public UProperty
{
public:
    UArrayProperty() { Type = GlobalType::UArrayProperty; }
    ~UArrayProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    UObjectReference GetInner() { return InnerObjRef; }
protected:
    /// persistent
    UObjectReference InnerObjRef;
};

class UDelegateProperty: public UProperty
{
public:
    UDelegateProperty() { Type = GlobalType::UDelegateProperty; }
    ~UDelegateProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference FunctionObjRef;
    UObjectReference DelegateObjRef;
};

class UInterfaceProperty: public UProperty
{
public:
    UInterfaceProperty() { Type = GlobalType::UInterfaceProperty; }
    ~UInterfaceProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference InterfaceObjRef;
};

class UMapProperty: public UProperty
{
public:
    UMapProperty() { Type = GlobalType::UMapProperty; }
    ~UMapProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// persistent
    UObjectReference KeyObjRef;
    UObjectReference ValueObjRef;
};

class ULevel: public UObject
{
public:
    ULevel() { Type = GlobalType::ULevel; }
    ~ULevel() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    /// database
    std::vector<UObjectReference> Actors;
};

class USurface: public UObject
{
public:
    USurface() {}
    ~USurface() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
};

class UTexture: public USurface
{
public:
    UTexture() {}
    ~UTexture() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
};

class UTexture2D: public UTexture
{
public:
    ///constructors and destructors
    UTexture2D() {}
    ~UTexture2D() {}
    ///serialization
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    std::string SerializeTexture2DData(size_t offset);
    bool TryLzoCompression(int minResolution = -1);
    bool ExportToExternalFile(CustomTFC& T2DFile, UPKInfo& info, bool compressedOnly = true);
    uint32_t CalculateTexture2DDataSize();
    ///getters
    std::vector<UTexture2DMipMap> GetMipMaps() { return MipMaps; }
    uint32_t GetHeight() { return Height; }
    uint32_t GetWidth() { return Width; }
    uint32_t GetMipMapCount() { return MipMapCount; }
    uint32_t GetPitchOrLinearSize() { return PitchOrLinearSize; }
    std::string GetPixelFormat() { return Format; }
    std::string GetTextureFileCacheName() { return TextureFileCacheName; }
    bool GetNeverStream() { return NeverStream; }
    bool GetDoNotReadTFC() { return DoNotReadTFC; }
    ///setters
    void SetMipMaps(std::vector<UTexture2DMipMap> mmaps) { MipMaps = mmaps; }
    void SetHeight(uint32_t h) { Height = h; }
    void SetWidth(uint32_t w) { Width = w; }
    void SetMipMapCount(uint32_t cnt) { MipMapCount = cnt; }
    void SetPitchOrLinearSize(uint32_t sz) { PitchOrLinearSize = sz; }
    void SetPixelFormat(std::string fmt) { Format = fmt; }
    void SetTextureFileCacheName(std::string tfcName) { TextureFileCacheName = tfcName; }
    void SetNeverStream(bool ns) { NeverStream = ns; }
    void SetDoNotReadTFC(bool val) { DoNotReadTFC = val; }
protected:
    uint32_t        Height = 0;
    uint32_t        Width = 0;
    uint32_t        MipTailBaseIdx = 0;
    std::string     Format;                 ///actually an enum
    std::string     LODGroup;               ///actually an enum
    int             LODBias = 0;
    bool            NeverStream = false;
    std::string     TextureFileCacheName;
    uint32_t        PitchOrLinearSize = 0;
    uint32_t        MipMapCount = 0;
    std::vector<UTexture2DMipMap> MipMaps;
    ///unknown data
    uint32_t        Unknown1 = 0;
    uint32_t        Unknown2 = 0;
    uint32_t        Unknown3 = 0;
    std::vector<char> UnknownData;
    ///memory vars
    bool            DoNotReadTFC = false;
    ///internal methods
    void SetClassVarFromProperty(UDefaultProperty& property, UPKInfo& info);
};

class UObjectUnknown: public UObject
{
public:
    UObjectUnknown() { Type = GlobalType::UObjectUnknown; }
    ~UObjectUnknown() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
};

#endif // UOBJECT_H
