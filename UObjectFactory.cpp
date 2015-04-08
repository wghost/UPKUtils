#include "UObjectFactory.h"

GlobalType UObjectFactory::NameToType(std::string name)
{
    if (name == "None")
    {
        return GlobalType::None;
    }
    else if (name == "Object")
    {
        return GlobalType::UObject;
    }
    else if (name == "Field")
    {
        return GlobalType::UField;
    }
	else if (name == "Const")
    {
        return GlobalType::UConst;
    }
	else if (name == "Enum")
    {
        return GlobalType::UEnum;
    }
	else if (name == "Property")
    {
        return GlobalType::UProperty;
    }
	else if (name == "ByteProperty")
    {
        return GlobalType::UByteProperty;
    }
	else if (name == "IntProperty")
    {
        return GlobalType::UIntProperty;
    }
	else if (name == "BoolProperty")
    {
        return GlobalType::UBoolProperty;
    }
	else if (name == "FloatProperty")
    {
        return GlobalType::UFloatProperty;
    }
	else if (name == "ObjectProperty")
    {
        return GlobalType::UObjectProperty;
    }
	else if (name == "ClassProperty")
    {
        return GlobalType::UClassProperty;
    }
	else if (name == "ComponentProperty")
    {
        return GlobalType::UComponentProperty;
    }
	else if (name == "NameProperty")
    {
        return GlobalType::UNameProperty;
    }
	else if (name == "StructProperty")
    {
        return GlobalType::UStructProperty;
    }
	else if (name == "StrProperty")
    {
        return GlobalType::UStrProperty;
    }
	else if (name == "ArrayProperty")
    {
        return GlobalType::UArrayProperty;
    }
	else if (name == "FixedArrayProperty")
    {
        return GlobalType::UFixedArrayProperty;
    }
	else if (name == "DelegateProperty")
    {
        return GlobalType::UDelegateProperty;
    }
	else if (name == "InterfaceProperty")
    {
        return GlobalType::UInterfaceProperty;
    }
	else if (name == "MapProperty")
    {
        return GlobalType::UMapProperty;
    }
	else if (name == "Struct")
    {
        return GlobalType::UStruct;
    }
	else if (name == "ScriptStruct")
    {
        return GlobalType::UScriptStruct;
    }
	else if (name == "Function")
    {
        return GlobalType::UFunction;
    }
	else if (name == "State")
    {
        return GlobalType::UState;
    }
	else if (name == "Class")
    {
        return GlobalType::UClass;
    }
	else if (name == "TextBuffer")
    {
        return GlobalType::UTextBuffer;
    }
	else if (name == "Level")
    {
        return GlobalType::ULevel;
    }
    else
    {
        return GlobalType::UObjectUnknown;
    }
}

UObject* UObjectFactory::Create(std::string name)
{
    return Create(NameToType(name));
}

UObject* UObjectFactory::Create(GlobalType Type)
{
    if (Type == GlobalType::None)
    {
        return new UObjectNone;                     /// special UE null-object
    }
    else if (Type == GlobalType::UObject)
    {
        return new UObject;
    }
    else if (Type == GlobalType::UField)
    {
        return new UField;
    }
	else if (Type == GlobalType::UConst)
    {
        return new UConst;
    }
	else if (Type == GlobalType::UEnum)
    {
        return new UEnum;
    }
	else if (Type == GlobalType::UProperty)
    {
        return new UProperty;
    }
	else if (Type == GlobalType::UByteProperty)
    {
        return new UByteProperty;
    }
	else if (Type == GlobalType::UIntProperty)
    {
        return new UIntProperty;
    }
	else if (Type == GlobalType::UBoolProperty)
    {
        return new UBoolProperty;
    }
	else if (Type == GlobalType::UFloatProperty)
    {
        return new UFloatProperty;
    }
	else if (Type == GlobalType::UObjectProperty)
    {
        return new UObjectProperty;
    }
	else if (Type == GlobalType::UClassProperty)
    {
        return new UClassProperty;
    }
	else if (Type == GlobalType::UComponentProperty)
    {
        return new UComponentProperty;
    }
	else if (Type == GlobalType::UNameProperty)
    {
        return new UNameProperty;
    }
	else if (Type == GlobalType::UStructProperty)
    {
        return new UStructProperty;
    }
	else if (Type == GlobalType::UStrProperty)
    {
        return new UStrProperty;
    }
	else if (Type == GlobalType::UArrayProperty)
    {
        return new UArrayProperty;
    }
	else if (Type == GlobalType::UFixedArrayProperty)
    {
        return new UFixedArrayProperty;
    }
	else if (Type == GlobalType::UDelegateProperty)
    {
        return new UDelegateProperty;
    }
	else if (Type == GlobalType::UInterfaceProperty)
    {
        return new UInterfaceProperty;
    }
	else if (Type == GlobalType::UMapProperty)
    {
        return new UMapProperty;
    }
	else if (Type == GlobalType::UStruct)
    {
        return new UStruct;
    }
	else if (Type == GlobalType::UScriptStruct)
    {
        return new UScriptStruct;
    }
	else if (Type == GlobalType::UFunction)
    {
        return new UFunction;
    }
	else if (Type == GlobalType::UState)
    {
        return new UState;
    }
	else if (Type == GlobalType::UClass)
    {
        return new UClass;
    }
	else if (Type == GlobalType::ULevel)
    {
        return new ULevel;
    }
    else
    {
        return new UObjectUnknown;                   /// special unknown object
    }
}
