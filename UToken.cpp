#include <sstream>
#include <map>
#include "UToken.h"
#include "UTokenFactory.h"

std::string MakeIndents(int indents)
{
    if (indents <= 0)
    {
        return "";
    }
    return std::string(indents, '\t');
}

int CountIndents(std::string str)
{
    size_t pos = str.find('\t');
    if (pos == std::string::npos)
    {
        return 0;
    }
    return str.rfind('\t') - pos + 1;
}

std::string CopyPositionsComment(std::string str)
{
    size_t pos = str.find("/*");
    if (pos == std::string::npos)
    {
        return "";
    }
    size_t len = str.rfind("*/") - pos + 3;
    return str.substr(pos, len);
}

std::string UScriptCode::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::map<uint16_t, std::string> ExprMap;
    std::map<uint16_t, int> JumpMap;
    int numIndents = 0;
    while (stream.good())
    {
        UScriptExpression ScrExpr;
        std::string ExprResult = ScrExpr.Deserialize(stream, info);
        if (JumpMap.count(MemorySize) > 0) /// reached jump label - remove indentation(s)
        {
            numIndents -= JumpMap[MemorySize];
            JumpMap[MemorySize] = 0;
        }
        ExprMap[MemorySize] = "/*(" + FormatHEX(MemorySize) + "/" + FormatHEX(SerialSize) + ")*/ "
                            + MakeIndents(numIndents) + ExprResult + "\n";
        if (ScrExpr.IsJump() && ScrExpr.GetJumpOffset() > MemorySize && ScrExpr.GetJumpOffset() != 0xFFFF) /// add indentation
        {
            numIndents += ScrExpr.GetType() != UToken::Jump;
            JumpMap[ScrExpr.GetJumpOffset()] += ScrExpr.GetType() != UToken::Jump;
        }
        SerialSize += ScrExpr.GetSerialSize();
        MemorySize += ScrExpr.GetMemorySize();
        if (ScrExpr.IsEOS()) /// end of script
        {
            break;
        }
    }
    for (std::map<uint16_t, int>::iterator it = JumpMap.begin(); it != JumpMap.end(); ++it)
    {
        if (it->first != 0xFFFF)
        {
            ExprMap[it->first] = CopyPositionsComment(ExprMap[it->first])
            + MakeIndents(CountIndents(ExprMap[it->first]))
            + "[#label_" + FormatHEX(it->first) + "]\n" + ExprMap[it->first];
        }
    }
    std::stringstream result;
    for (std::map<uint16_t, std::string>::iterator it = ExprMap.begin(); it != ExprMap.end(); ++it)
    {
        result << it->second;
    }
    return result.str();
}

std::string UScriptExpression::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    UScriptToken* ScrToken;
    Type = (UToken)stream.get();
    if (Type > UToken::NativeFunctionF)
    {
        stream.putback((uint8_t)Type);
        Type = UToken::ExtendedNative;
    }
    ScrToken = UTokenFactory::Create(Type);
    if (ScrToken == nullptr)
    {
        result << "Error! Unknown token: " << FormatHEX((uint8_t)Type) << "\n";
        return result.str();
    }
    std::string TokenResult = ScrToken->Deserialize(stream, info);
    result << TokenResult;
    SerialSize += ScrToken->GetSerialSize();
    MemorySize += ScrToken->GetMemorySize();
    JumpOffset = ScrToken->GetJumpOffset();
    if (Type == UToken::Switch)
    {
        JumpOffset = 0xFFFF;
    }
    delete ScrToken;
    return result.str();
}

std::string UScriptToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    SerialSize += 1;
    MemorySize += 1;
    return FormatType();
}

std::string UScriptToken::DeserializeObjRef(std::istream& stream, UPKInfo& info)
{
    SerialSize += 4;
    MemorySize += 8;
    return FormatObjRef(ReadObjRef(stream), info);
}

std::string UScriptToken::DeserializeNameIndex(std::istream& stream, UPKInfo& info)
{
    SerialSize += 8;
    MemorySize += 8;
    return FormatNameIndex(ReadNameIndex(stream), info);
}

std::string UScriptToken::DeserializeByte(std::istream& stream, UPKInfo& info)
{
    SerialSize += 1;
    MemorySize += 1;
    return FormatByte(ReadByte(stream));
}

std::string UScriptToken::DeserializeShort(std::istream& stream, UPKInfo& info)
{
    SerialSize += 2;
    MemorySize += 2;
    return FormatShort(ReadShort(stream));
}

std::string UScriptToken::DeserializeMemoryOffset(std::istream& stream, UPKInfo& info)
{
    SerialSize += 2;
    MemorySize += 2;
    JumpOffset = ReadShort(stream);
    if (JumpOffset != 0xFFFF)
    {
        return FormatMemOffset(JumpOffset);
    }
    return FormatShort(JumpOffset);
}

std::string UScriptToken::DeserializeMemorySize(std::istream& stream, UPKInfo& info)
{
    SerialSize += 2;
    MemorySize += 2;
    return FormatMemSize(ReadShort(stream));
}

std::string UScriptToken::DeserializeInt(std::istream& stream, UPKInfo& info)
{
    SerialSize += 4;
    MemorySize += 4;
    return FormatInt(ReadInt(stream));
}

std::string UScriptToken::DeserializeUInt(std::istream& stream, UPKInfo& info)
{
    SerialSize += 4;
    MemorySize += 4;
    return FormatUInt(ReadUInt(stream));
}

std::string UScriptToken::DeserializeFloat(std::istream& stream, UPKInfo& info)
{
    SerialSize += 4;
    MemorySize += 4;
    return FormatFloat(ReadFloat(stream));
}

std::string UScriptToken::DeserializeString(std::istream& stream, UPKInfo& info)
{
    std::string Str;
    getline(stream, Str, '\0');
    SerialSize += Str.length() + 1;
    MemorySize += Str.length() + 1;
    return FormatString(Str);
}

std::string UScriptToken::DeserializeUniString(std::istream& stream, UPKInfo& info)
{
    /// stub!
    return DeserializeString(stream, info);
}

std::string UScriptToken::DeserializeExpression(std::istream& stream, UPKInfo& info, int num)
{
    if (num == 0)
    {
        return "";
    }
    std::stringstream result;
    int cnt = 0;
    FoundSkip = false;
    while (stream.good())
    {
        UScriptExpression ScrExpr;
        result << ScrExpr.Deserialize(stream, info);
        SerialSize += ScrExpr.GetSerialSize();
        MemorySize += ScrExpr.GetMemorySize();
        ++cnt;
        if (ScrExpr.GetType() == UToken::Skip)
        {
            FoundSkip = true;
        }
        if ((num > 0 && cnt >= num) || (num < 0 && ScrExpr.IsEndFunction()))
        {
            break;
        }
    }
    return result.str();
}

std::string UScriptToken::DeserializeFunctionCall(std::istream& stream, UPKInfo& info)
{
    std::string result = DeserializeExpression(stream, info, -1);
    if (FoundSkip)
    {
        result += ") "; /// memory size marker
        FoundSkip = false;
    }
    return result;
}

std::string UScriptToken::FormatType()
{
    return FormatHEX((char*)&Type, 1);
}

UObjectReference UScriptToken::ReadObjRef(std::istream& stream)
{
    UObjectReference ObjRef;
    stream.read((char*)&ObjRef, 4);
    return ObjRef;
}

std::string UScriptToken::FormatObjRef(UObjectReference ObjRef, UPKInfo& info)
{
    std::stringstream result;
    if (ObjRef == 0)
    {
        result << "<NullRef> ";
    }
    else
    {
        if (ObjRef > 0)
        {
            if (info.GetExportEntry(ObjRef).Type == "Class")
            {
                result << "<Class." << info.GetExportEntry(ObjRef).FullName << "> ";
            }
            else
            {
                bool IsLocal = (info.GetLastAccessedExportObjIdx() == info.GetExportEntry(ObjRef).OwnerRef && info.GetExportEntry(ObjRef).OwnerRef != 0);
                bool IsMember = (info.GetExportEntry(info.GetLastAccessedExportObjIdx()).OwnerRef == info.GetExportEntry(ObjRef).OwnerRef && info.GetExportEntry(ObjRef).OwnerRef != 0);
                if (IsLocal)
                {
                    result << "<." << info.GetExportEntry(ObjRef).Name << "> ";
                }
                else if (IsMember)
                {
                    result << "<@" << info.GetExportEntry(ObjRef).Name << "> ";
                }
                else
                {
                    result << "<" << info.GetExportEntry(ObjRef).FullName << "> ";
                }
            }
        }
        else
        {
            result << "<" << info.GetImportEntry(-ObjRef).FullName << "> ";
        }
    }
    return result.str();
}

uint8_t UScriptToken::ReadByte(std::istream& stream)
{
    uint8_t Byte;
    stream.read((char*)&Byte, 1);
    return Byte;
}

std::string UScriptToken::FormatByte(uint8_t Byte)
{
    return FormatHEX((char*)&Byte, 1);
}

uint16_t UScriptToken::ReadShort(std::istream& stream)
{
    uint16_t Short;
    stream.read((char*)&Short, 2);
    return Short;
}

std::string UScriptToken::FormatShort(uint16_t Short)
{
    return FormatHEX((char*)&Short, 2);
}

std::string UScriptToken::FormatMemOffset(uint16_t MemOff)
{
    return "[@label_" + FormatHEX(MemOff) + "] ";
}

std::string UScriptToken::FormatMemSize(uint16_t MemOff)
{
    return "[@] ";
}

UNameIndex UScriptToken::ReadNameIndex(std::istream& stream)
{
    UNameIndex NameIdx;
    stream.read(reinterpret_cast<char*>(&NameIdx), sizeof(NameIdx));
    return NameIdx;
}

std::string UScriptToken::FormatNameIndex(UNameIndex NameIdx, UPKInfo& info)
{
    return "<" + info.IndexToName(NameIdx) + "> ";
}

int32_t UScriptToken::ReadInt(std::istream& stream)
{
    int32_t Int;
    stream.read((char*)&Int, 4);
    return Int;
}

std::string UScriptToken::FormatInt(int32_t Int)
{
    std::stringstream ss;
    ss << "<%i " << Int << "> ";
    return ss.str();
}

uint32_t UScriptToken::ReadUInt(std::istream& stream)
{
    uint32_t UInt;
    stream.read((char*)&UInt, 4);
    return UInt;
}

std::string UScriptToken::FormatUInt(uint32_t UInt)
{
    std::stringstream ss;
    ss << "<%u " << UInt << "> ";
    return ss.str();
}

float UScriptToken::ReadFloat(std::istream& stream)
{
    float Flo;
    stream.read((char*)&Flo, 4);
    return Flo;
}

std::string UScriptToken::FormatFloat(float Flo)
{
    std::stringstream ss;
    ss << "<%f " << Flo << "> ";
    return ss.str();
}

std::string UScriptToken::FormatString(std::string Str)
{
    return "<%t \"" + Str + "\"> ";
}

std::string UExpressionToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info, Count);
    return result.str();
}

std::string UObjRefToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeObjRef(stream, info);
    return result.str();
}

std::string UNameIndexToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeNameIndex(stream, info);
    return result.str();
}

std::string USwitchToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UObjRefToken::Deserialize(stream, info);
    result << DeserializeByte(stream, info);
    result << DeserializeExpression(stream, info);
    return result.str();
}

std::string UJumpToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeMemoryOffset(stream, info);
    return result.str();
}

std::string UJumpIfNotToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UJumpToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    return result.str();
}

std::string UAssertToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeShort(stream, info);
    result << DeserializeByte(stream, info);
    result << DeserializeExpression(stream, info);
    return result.str();
}

std::string UCaseToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UJumpToken::Deserialize(stream, info);
    if (JumpOffset != 0xFFFF)
    {
        result << DeserializeExpression(stream, info);
    }
    return result.str();
}

std::string ULabelTableToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    while (stream.good())
    {
        UNameIndex NameIdx = ReadNameIndex(stream);
        result << FormatNameIndex(NameIdx, info);
        SerialSize += 8;
        MemorySize += 8;
        result << FormatUInt(ReadUInt(stream));
        SerialSize += 4;
        MemorySize += 4;
        if (info.IsNoneIdx(NameIdx))
        {
            break;
        }
    }
    return result.str();
}

std::string UEatStringToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UObjRefToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    return result.str();
}

std::string UClassContextToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    result << DeserializeMemorySize(stream, info);
    result << DeserializeObjRef(stream, info);
    result << DeserializeByte(stream, info);
    result << "( "; /// memory size marker
    result << DeserializeExpression(stream, info);
    result << ") "; /// memory size marker
    return result.str();
}

std::string USkipToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeMemorySize(stream, info);
    result << "( "; /// memory size marker
    return result.str();
}

std::string UVirtualFunctionToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeNameIndex(stream, info);
    result << DeserializeFunctionCall(stream, info);
    return result.str();
}

std::string UFinalFunctionToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeObjRef(stream, info);
    result << DeserializeFunctionCall(stream, info);
    return result.str();
}

std::string UIntConstToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeInt(stream, info);
    return result.str();
}

std::string UFloatConstToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeFloat(stream, info);
    return result.str();
}

std::string UStringConstToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeString(stream, info);
    return result.str();
}

std::string URotatorConstToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeInt(stream, info) << DeserializeInt(stream, info) << DeserializeInt(stream, info);
    return result.str();
}

std::string UVectorConstToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeFloat(stream, info) << DeserializeFloat(stream, info) << DeserializeFloat(stream, info);
    return result.str();
}

std::string UByteConstToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeByte(stream, info);
    return result.str();
}

std::string UIteratorToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    result << DeserializeMemoryOffset(stream, info);
    return result.str();
}

std::string UStructMemberToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeObjRef(stream, info);
    result << DeserializeObjRef(stream, info);
    result << DeserializeByte(stream, info);
    result << DeserializeByte(stream, info);
    result << DeserializeExpression(stream, info);
    return result.str();
}

std::string UPrimitiveCastToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeByte(stream, info);
    result << DeserializeExpression(stream, info);
    return result.str();
}

std::string UDebugInfoToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeInt(stream, info) << DeserializeInt(stream, info) << DeserializeInt(stream, info);
    result << DeserializeByte(stream, info);
    return result.str();
}

std::string UDelegateFunctionToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeByte(stream, info);
    result << DeserializeObjRef(stream, info);
    result << DeserializeNameIndex(stream, info);
    result << DeserializeFunctionCall(stream, info);
    return result.str();
}

std::string UDelegatePropertyToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeNameIndex(stream, info);
    result << DeserializeObjRef(stream, info);
    return result.str();
}

std::string UTernaryConditionToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    result << DeserializeMemorySize(stream, info);
    result << "( "; /// memory size marker
    result << DeserializeExpression(stream, info);
    result << ") "; /// memory size marker
    result << DeserializeMemorySize(stream, info);
    result << "( "; /// memory size marker
    result << DeserializeExpression(stream, info);
    result << ") "; /// memory size marker
    return result.str();
}

std::string UDynArrFindToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    result << DeserializeMemorySize(stream, info);
    result << "( "; /// memory size marker
    result << DeserializeExpression(stream, info, 2);
    result << ") "; /// memory size marker
    return result.str();
}

std::string UDynArrayFindStructToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info);
    result << DeserializeMemorySize(stream, info);
    result << "( "; /// memory size marker
    result << DeserializeExpression(stream, info, 3);
    result << ") "; /// memory size marker
    return result.str();
}

std::string UDefaultParmValueToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeMemorySize(stream, info);
    result << "( "; /// memory size marker
    result << DeserializeExpression(stream, info, 2);
    result << ") "; /// memory size marker
    return result.str();
}

std::string UDynArrIteratorToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    result << UScriptToken::Deserialize(stream, info);
    result << DeserializeExpression(stream, info, 2);
    result << DeserializeByte(stream, info);
    result << DeserializeExpression(stream, info);
    result << DeserializeMemoryOffset(stream, info);
    return result.str();
}

std::string UNativeFunctionToken::Deserialize(std::istream& stream, UPKInfo& info)
{
    std::stringstream result;
    if (Type != UToken::ExtendedNative)
    {
        result << UScriptToken::Deserialize(stream, info);
    }
    result << DeserializeByte(stream, info);
    result << DeserializeFunctionCall(stream, info);
    return result.str();
}

