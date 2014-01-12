#ifndef UOBJECT_H
#define UOBJECT_H

#include <vector>
#include <string>
#include <iostream>
#include <utility>

#include "UPKInfo.h"

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
	UMapProperty = 27
};

class UDefaultProperty
{
public:
    UDefaultProperty(): OwnerRef(0) {}
    ~UDefaultProperty() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info, size_t maxOffset);
    std::string Format(std::istream& stream, UPKInfo& info);
    void SetOwner(UObjectReference Owner) { OwnerRef = Owner; }
    std::string GetName() { return Name; }
    std::string DeserializeValue(std::istream& stream, UPKInfo& info);
    std::string FindArrayType(std::string ArrName, std::istream& stream, UPKInfo& info);
    std::string GuessArrayType(std::string ArrName);
protected:
    /// persistent
    UNameIndex NameIdx;
    UNameIndex TypeIdx;
    uint32_t PropertySize;
    uint32_t ArrayIdx;
    uint8_t  BoolValue;       /// for BoolProperty only
    UNameIndex InnerNameIdx;  /// for StructProperty and ByteProperty only
    std::vector<char> InnerValue;
    /// memory
    UObjectReference OwnerRef;
    std::string Name;
    std::string Type;
};

class UDefaultPropertiesList
{
public:
    UDefaultPropertiesList(): OwnerRef(0) {}
    ~UDefaultPropertiesList() {}
    UDefaultPropertiesList(UObjectReference owner): OwnerRef(owner) {}
    void SetOwner(UObjectReference owner) { OwnerRef = owner; }
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    std::vector<UDefaultProperty> DefaultProperties;
    UObjectReference OwnerRef;
    size_t PropertyOffset;
    size_t PropertySize;
};

/// parent class of all Unreal objects
class UObject
{
public:
    UObject(): Type(GlobalType::UObject), ThisRef(0), FlagsOffset(0) {}
    virtual ~UObject() {}
    virtual std::string Deserialize(std::istream& stream, UPKInfo& info);
    void SetRef(UObjectReference thisRef) { ThisRef = thisRef; }
    virtual bool IsStructure() { return false; }
    virtual bool IsProperty() { return false; }
    virtual bool IsState() { return false; }
protected:
    /// persistent
    UObjectReference ObjRef;            /// Next object (Linker-related)
    UDefaultPropertiesList DefaultProperties; /// for non-Class objects only
    /// memory
    GlobalType Type;
    UObjectReference ThisRef;
    size_t FlagsOffset;
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
protected:
    /// persistent
    UObjectReference NextRef;
    UObjectReference ParentRef; /// for Struct objects only
    /// memory
    size_t FieldOffset;
    size_t FieldSize;
};

class UStruct: public UField
{
public:
    UStruct() { Type = GlobalType::UStruct; }
    ~UStruct() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    bool IsStructure() { return true; }
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
    size_t ScriptOffset;
    size_t StructSize;
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
    UDefaultPropertiesList DefaultProperties;
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

class UObjectUnknown: public UObject
{
public:
    UObjectUnknown() { Type = GlobalType::UObjectUnknown; }
    ~UObjectUnknown() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
};

#endif // UOBJECT_H
