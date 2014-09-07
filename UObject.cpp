#include "UObject.h"

#include <sstream>

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
            stream.read(reinterpret_cast<char*>(&BoolValue), sizeof(BoolValue));
            ss << "\tBoolean value: " << FormatHEX(BoolValue) << " = ";
            if (BoolValue == 0)
                ss << "false\n";
            else
                ss << "true\n";
        }
        if (Type == "StructProperty" || Type == "ByteProperty")
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
        int32_t value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        ss << "\tInteger: " << FormatHEX((uint32_t)value) << " = " << value << std::endl;
    }
    else if (Type == "FloatProperty")
    {
        float value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        ss << "\tFloat: " << FormatHEX(value) << " = " << value << std::endl;
    }
    else if (Type == "ObjectProperty" ||
             Type == "InterfaceProperty" ||
             Type == "ComponentProperty" ||
             Type == "ClassProperty")
    {
        UObjectReference value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        ss << "\tObject: " << FormatHEX((uint32_t)value) << " = ";
        if (value == 0)
            ss << "none\n";
        else
            ss << info.ObjRefToName(value) << std::endl;
    }
    else if (Type == "NameProperty" || Type == "ByteProperty")
    {
        UNameIndex value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(value));
        ss << "\tName: " << FormatHEX(value) << " = " << info.IndexToName(value) << std::endl;
    }
    else if (Type == "StrProperty")
    {
        uint32_t StrLength;
        stream.read(reinterpret_cast<char*>(&StrLength), sizeof(StrLength));
        ss << "\tStrLength = " << FormatHEX(StrLength) << " = " << StrLength << std::endl;
        if (StrLength > 0)
        {
            std::string str;
            getline(stream, str, '\0');
            ss << "\tString = " << str << std::endl;
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
        uint32_t P, Y, R;
        stream.read(reinterpret_cast<char*>(&P), sizeof(P));
        stream.read(reinterpret_cast<char*>(&Y), sizeof(Y));
        stream.read(reinterpret_cast<char*>(&R), sizeof(R));
        ss << "\tRotator (Pitch, Yaw, Roll) = ("
           << FormatHEX(P) << ", " << FormatHEX(Y) << ", " << FormatHEX(R) << ") = ("
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
        std::vector<char> unk(PropertySize);
        stream.read(unk.data(), unk.size());
        ss << "\tUnknown property: " << FormatHEX(unk) << std::endl;
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
    return "None";
}

std::string UObject::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::ostringstream ss;
    ss << "UObject:\n";
    stream.read(reinterpret_cast<char*>(&ObjRef), sizeof(ObjRef));
    ss << "\tPrevObjRef = " << FormatHEX((uint32_t)ObjRef) << " -> " << info.ObjRefToName(ObjRef) << std::endl;
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
    if (StateMapSize > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumComponents > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumInterfaces > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumDontSortCategories > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumHideCategories > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumAutoExpandCategories > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumAutoCollapseCategories > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NumClassGroups > ScriptSerialSize) /// bad data malloc error prevention
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
    if (NativeClassNameLength > ScriptSerialSize) /// bad data malloc error prevention
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
