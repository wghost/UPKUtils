#ifndef UTOKEN_H
#define UTOKEN_H

#include "UPKInfo.h"

enum class UToken
{
	LocalVariable = 0,
	InstanceVariable = 1,
	DefaultVariable = 2,
	StateVariable = 3,
	Return = 4,
	Switch = 5,
	Jump = 6,
	JumpIfNot = 7,
	Stop = 8,
	Assert = 9,
	Case = 10,
	Nothing = 11,
	LabelTable = 12,
	GotoLabel = 13,
	EatString = 14,
	Let = 15,
	DynArrayElement = 16,
	New = 17,
	ClassContext = 18,
	MetaCast = 19,
	LetBool = 20,
	EndParmValue = 21,
	EndFunctionParms = 22,
	Self = 23,
	Skip = 24,
	Context = 25,
	ArrayElement = 26,
	VirtualFunction = 27,
	FinalFunction = 28,
	IntConst = 29,
	FloatConst = 30,
	StringConst = 31,
	ObjectConst = 32,
	NameConst = 33,
	RotatorConst = 34,
	VectorConst = 35,
	ByteConst = 36,
	IntZero = 37,
	IntOne = 38,
	True = 39,
	False = 40,
	NativeParm = 41,
	NoObject = 42,
	UnknownDeprecated = 43,
	IntConstByte = 44,
	BoolVariable = 45,
	DynamicCast = 46,
	Iterator = 47,
	IteratorPop = 48,
	IteratorNext = 49,
	StructCmpEq = 50,
	StructCmpNe = 51,
	UniStringConst = 52,
	StructMember = 53,
	DynArrayLen = 54,
	GlobalFunction = 55,
	PrimitiveCast = 56,
	DynArrayInsert = 57,
	ReturnNothing = 58,
	DelegateCmpEq = 59,
	DelegateCmpNe = 60,
	DelegateFunctionCmpEq = 61,
	DelegateFunctionCmpNE = 62,
	NoDelegate = 63,
	DynArrayRemove = 64,
	DebugInfo = 65,
	DelegateFunction = 66,
	DelegateProperty = 67,
	LetDelegate = 68,
	TernaryCondition = 69,
	DynArrFind = 70,
	DynArrayFindStruct = 71,
	OutVariable = 72,
	DefaultParmValue = 73,
	NoParm = 74,
	InstanceDelegate = 75,
	UnknownDynamicVariable1 = 76,
	UnknownDynamicVariable2 = 77,
	UnknownDynamicVariable3 = 78,
	UnknownDynamicVariable4 = 79,
	UnknownDynamicVariable5 = 80,
	InterfaceContext = 81,
	InterfaceCast = 82,
	EndOfScript = 83,
	DynArrAdd = 84,
	DynArrAddItem = 85,
	DynArrRemoveItem = 86,
	DynArrInsertItem = 87,
	DynArrIterator = 88,
	DynArrSort = 89,
	UnknownFilterEditorOnly1 = 90,
	UnknownFilterEditorOnly2 = 91,
	UnknownFilterEditorOnly3 = 92,
	UnknownFilterEditorOnly4 = 93,
	UnknownFilterEditorOnly5 = 94,
	UnknownFilterEditorOnly6 = 95,
	ExtendedNative = 96,
	NativeFunction1 = 97,
	NativeFunction2 = 98,
	NativeFunction3 = 99,
	NativeFunction4 = 100,
	NativeFunction5 = 101,
	NativeFunction6 = 102,
	NativeFunction7 = 103,
	NativeFunction8 = 104,
	NativeFunction9 = 105,
	NativeFunctionA = 106,
	NativeFunctionB = 107,
	NativeFunctionC = 108,
	NativeFunctionD = 109,
	NativeFunctionE = 110,
	NativeFunctionF = 111
};

enum class UCastToken
{
	InterfaceToBool = 54,
	InterfaceToString = 55,
	InterfaceToObject = 56,
	RotatorToVector = 57,
	ByteToInt = 58,
	ByteToBool = 59,
	ByteToFloat = 60,
	IntToByte = 61,
	IntToBool = 62,
	IntToFloat = 63,
	BoolToByte = 64,
	BoolToInt = 65,
	BoolToFloat = 66,
	FloatToByte = 67,
	FloatToInt = 68,
	FloatToBool = 69,
	ObjectToInterface = 70,
	ObjectToBool = 71,
	NameToBool = 72,
	StringToByte = 73,
	StringToInt = 74,
	StringToBool = 75,
	StringToFloat = 76,
	StringToVector = 77,
	StringToRotator = 78,
	VectorToBool = 79,
	VectorToRotator = 80,
	RotatorToBool = 81,
	ByteToString = 82,
	IntToString = 83,
	BoolToString = 84,
	FloatToString = 85,
	ObjectToString = 86,
	NameToString = 87,
	VectorToString = 88,
	RotatorToString = 89,
	DelegateToString = 90,
	StringToName = 96
};

class UScriptBase
{
public:
    UScriptBase(): Type(UToken(0)), SerialSize(0), MemorySize(0), JumpOffset(0) {}
    virtual ~UScriptBase() {}
    virtual std::string Deserialize(std::istream& stream, UPKInfo& info) = 0;
    uint32_t GetSerialSize() { return SerialSize; }
    uint32_t GetMemorySize() { return MemorySize; }
    uint16_t GetJumpOffset() { return JumpOffset; }
protected:
    UToken Type;
    uint16_t SerialSize;
    uint16_t MemorySize;
    uint16_t JumpOffset;
};

class UScriptCode : public UScriptBase
{
public:
    UScriptCode() {}
    ~UScriptCode() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UScriptExpression : public UScriptBase
{
public:
    UScriptExpression() {}
    ~UScriptExpression() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    UToken GetType() { return Type; }
    bool IsEOS() { return (Type == UToken::EndOfScript); }
    bool IsEndParm() { return (Type == UToken::EndParmValue); }
    bool IsEndFunction() { return (Type == UToken::EndFunctionParms); }
    bool IsJump() { return (Type == UToken::Jump || Type == UToken::JumpIfNot || Type == UToken::Case || Type == UToken::Iterator || Type == UToken::DynArrIterator); }
};

class UScriptToken : public UScriptBase
{
public:
    UScriptToken(): FoundSkip(false) {}
    virtual ~UScriptToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
    std::string DeserializeObjRef(std::istream& stream, UPKInfo& info);
    std::string DeserializeNameIndex(std::istream& stream, UPKInfo& info);
    std::string DeserializeByte(std::istream& stream, UPKInfo& info);
    std::string DeserializeShort(std::istream& stream, UPKInfo& info);
    std::string DeserializeMemoryOffset(std::istream& stream, UPKInfo& info);
    std::string DeserializeMemorySize(std::istream& stream, UPKInfo& info);
    std::string DeserializeInt(std::istream& stream, UPKInfo& info);
    std::string DeserializeUInt(std::istream& stream, UPKInfo& info);
    std::string DeserializeFloat(std::istream& stream, UPKInfo& info);
    std::string DeserializeString(std::istream& stream, UPKInfo& info);
    std::string DeserializeUniString(std::istream& stream, UPKInfo& info);
    std::string DeserializeExpression(std::istream& stream, UPKInfo& info, int num = 1);
    std::string DeserializeFunctionCall(std::istream& stream, UPKInfo& info);
    bool HasSkipToken() { return FoundSkip; }
protected:
    /// helper functions
    std::string FormatType();
    UObjectReference ReadObjRef(std::istream& stream);
    std::string FormatObjRef(UObjectReference ObjRef, UPKInfo& info);
    uint8_t ReadByte(std::istream& stream);
    std::string FormatByte(uint8_t Byte);
    uint16_t ReadShort(std::istream& stream);
    std::string FormatShort(uint16_t Short);
    std::string FormatMemOffset(uint16_t MemOff);
    std::string FormatMemSize(uint16_t MemOff);
    UNameIndex ReadNameIndex(std::istream& stream);
    std::string FormatNameIndex(UNameIndex NameIdx, UPKInfo& info);
    int32_t ReadInt(std::istream& stream);
    std::string FormatInt(int32_t Int);
    uint32_t ReadUInt(std::istream& stream);
    std::string FormatUInt(uint32_t UInt);
    float ReadFloat(std::istream& stream);
    std::string FormatFloat(float Flo);
    std::string FormatString(std::string Str);

    bool FoundSkip;
};

class UExpressionToken : public UScriptToken
{
public:
    UExpressionToken() { Count = 1; }
    virtual ~UExpressionToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
protected:
    int Count;
};

class UObjRefToken : public UScriptToken
{
public:
    UObjRefToken() {}
    virtual ~UObjRefToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UNameIndexToken : public UScriptToken
{
public:
    UNameIndexToken() {}
    virtual ~UNameIndexToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class ULocalVariableToken : public UObjRefToken
{
public:
    ULocalVariableToken() { Type = UToken::LocalVariable; }
    ~ULocalVariableToken() {}
};

class UInstanceVariableToken : public UObjRefToken
{
public:
    UInstanceVariableToken() { Type = UToken::InstanceVariable; }
    ~UInstanceVariableToken() {}
};

class UDefaultVariableToken : public UObjRefToken
{
public:
    UDefaultVariableToken() { Type = UToken::DefaultVariable; }
    ~UDefaultVariableToken() {}
};

class UStateVariableToken : public UObjRefToken
{
public:
    UStateVariableToken() { Type = UToken::StateVariable; }
    ~UStateVariableToken() {}
};

class UReturnToken : public UExpressionToken
{
public:
    UReturnToken() { Type = UToken::Return; }
    ~UReturnToken() {}
};

class USwitchToken : public UObjRefToken
{
public:
    USwitchToken() { Type = UToken::Switch; }
    ~USwitchToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UJumpToken : public UScriptToken
{
public:
    UJumpToken() { Type = UToken::Jump; }
    ~UJumpToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UJumpIfNotToken : public UJumpToken
{
public:
    UJumpIfNotToken() { Type = UToken::JumpIfNot; }
    ~UJumpIfNotToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UStopToken : public UScriptToken
{
public:
    UStopToken() { Type = UToken::Stop; }
    ~UStopToken() {}
};

class UAssertToken : public UScriptToken
{
public:
    UAssertToken() { Type = UToken::Assert; }
    ~UAssertToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UCaseToken : public UJumpToken
{
public:
    UCaseToken() { Type = UToken::Case; }
    ~UCaseToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UNothingToken : public UScriptToken
{
public:
    UNothingToken() { Type = UToken::Nothing; }
    ~UNothingToken() {}
};

class ULabelTableToken : public UScriptToken
{
public:
    ULabelTableToken() { Type = UToken::LabelTable; }
    ~ULabelTableToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UGotoLabelToken : public UExpressionToken
{
public:
    UGotoLabelToken() { Type = UToken::GotoLabel; }
    ~UGotoLabelToken() {}
};

class UEatStringToken : public UObjRefToken
{
public:
    UEatStringToken() { Type = UToken::EatString; }
    ~UEatStringToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class ULetToken : public UExpressionToken
{
public:
    ULetToken() { Type = UToken::Let; Count = 2; }
    ~ULetToken() {}
};

class UDynArrayElementToken : public UExpressionToken
{
public:
    UDynArrayElementToken() { Type = UToken::DynArrayElement; Count = 2; }
    ~UDynArrayElementToken() {}
};

class UNewToken : public UExpressionToken
{
public:
    UNewToken() { Type = UToken::New; Count = 5; }
    ~UNewToken() {}
};

class UClassContextToken : public UScriptToken
{
public:
    UClassContextToken() { Type = UToken::ClassContext; }
    ~UClassContextToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UMetaCastToken : public UEatStringToken
{
public:
    UMetaCastToken() { Type = UToken::MetaCast; }
    ~UMetaCastToken() {}
};

class ULetBoolToken : public ULetToken
{
public:
    ULetBoolToken() { Type = UToken::LetBool; }
    ~ULetBoolToken() {}
};

class UEndParmValueToken : public UScriptToken
{
public:
    UEndParmValueToken() { Type = UToken::EndParmValue; }
    ~UEndParmValueToken() {}
};

class UEndFunctionParmsToken : public UScriptToken
{
public:
    UEndFunctionParmsToken() { Type = UToken::EndFunctionParms; }
    ~UEndFunctionParmsToken() {}
};

class USelfToken : public UScriptToken
{
public:
    USelfToken() { Type = UToken::Self; }
    ~USelfToken() {}
};

class USkipToken : public UScriptToken
{
public:
    USkipToken() { Type = UToken::Skip; }
    ~USkipToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UContextToken : public UClassContextToken
{
public:
    UContextToken() { Type = UToken::Context; }
    ~UContextToken() {}
};

class UArrayElementToken : public UDynArrayElementToken
{
public:
    UArrayElementToken() { Type = UToken::ArrayElement; }
    ~UArrayElementToken() {}
};

class UVirtualFunctionToken : public UScriptToken
{
public:
    UVirtualFunctionToken() { Type = UToken::VirtualFunction; }
    ~UVirtualFunctionToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UFinalFunctionToken : public UScriptToken
{
public:
    UFinalFunctionToken() { Type = UToken::FinalFunction; }
    ~UFinalFunctionToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UIntConstToken : public UScriptToken
{
public:
    UIntConstToken() { Type = UToken::IntConst; }
    ~UIntConstToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UFloatConstToken : public UScriptToken
{
public:
    UFloatConstToken() { Type = UToken::FloatConst; }
    ~UFloatConstToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UStringConstToken : public UScriptToken
{
public:
    UStringConstToken() { Type = UToken::StringConst; }
    ~UStringConstToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UObjectConstToken : public UObjRefToken
{
public:
    UObjectConstToken() { Type = UToken::ObjectConst; }
    ~UObjectConstToken() {}
};

class UNameConstToken : public UNameIndexToken
{
public:
    UNameConstToken() { Type = UToken::NameConst; }
    ~UNameConstToken() {}
};

class URotatorConstToken : public UScriptToken
{
public:
    URotatorConstToken() { Type = UToken::RotatorConst; }
    ~URotatorConstToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UVectorConstToken : public UScriptToken
{
public:
    UVectorConstToken() { Type = UToken::VectorConst; }
    ~UVectorConstToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UByteConstToken : public UScriptToken
{
public:
    UByteConstToken() { Type = UToken::ByteConst; }
    ~UByteConstToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UIntZeroToken : public UScriptToken
{
public:
    UIntZeroToken() { Type = UToken::IntZero; }
    ~UIntZeroToken() {}
};

class UIntOneToken : public UScriptToken
{
public:
    UIntOneToken() { Type = UToken::IntOne; }
    ~UIntOneToken() {}
};

class UTrueToken : public UScriptToken
{
public:
    UTrueToken() { Type = UToken::True; }
    ~UTrueToken() {}
};

class UFalseToken : public UScriptToken
{
public:
    UFalseToken() { Type = UToken::False; }
    ~UFalseToken() {}
};

class UNativeParmToken : public UScriptToken
{
public:
    UNativeParmToken() { Type = UToken::NativeParm; }
    ~UNativeParmToken() {}
};

class UNoObjectToken : public UScriptToken
{
public:
    UNoObjectToken() { Type = UToken::NoObject; }
    ~UNoObjectToken() {}
};

class UIntConstByteToken : public UByteConstToken
{
public:
    UIntConstByteToken() { Type = UToken::IntConstByte; }
    ~UIntConstByteToken() {}
};

class UBoolVariableToken : public UExpressionToken
{
public:
    UBoolVariableToken() { Type = UToken::BoolVariable; }
    ~UBoolVariableToken() {}
};

class UDynamicCastToken : public UMetaCastToken
{
public:
    UDynamicCastToken() { Type = UToken::DynamicCast; }
    ~UDynamicCastToken() {}
};

class UIteratorToken : public UScriptToken
{
public:
    UIteratorToken() { Type = UToken::Iterator; }
    ~UIteratorToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UIteratorPopToken : public UScriptToken
{
public:
    UIteratorPopToken() { Type = UToken::IteratorPop; }
    ~UIteratorPopToken() {}
};

class UIteratorNextToken : public UScriptToken
{
public:
    UIteratorNextToken() { Type = UToken::IteratorNext; }
    ~UIteratorNextToken() {}
};

class UStructCmpEqToken : public UExpressionToken
{
public:
    UStructCmpEqToken() { Type = UToken::StructCmpEq; Count = 2; }
    ~UStructCmpEqToken() {}
};

class UStructCmpNeToken : public UExpressionToken
{
public:
    UStructCmpNeToken() { Type = UToken::StructCmpNe; Count = 2; }
    ~UStructCmpNeToken() {}
};

class UUniStringConstToken : public UStringConstToken /// stub!
{
public:
    UUniStringConstToken() { Type = UToken::UniStringConst; }
    ~UUniStringConstToken() {}
};

class UStructMemberToken : public UScriptToken
{
public:
    UStructMemberToken() { Type = UToken::StructMember; }
    ~UStructMemberToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDynArrayLenToken : public UExpressionToken
{
public:
    UDynArrayLenToken() { Type = UToken::DynArrayLen; }
    ~UDynArrayLenToken() {}
};

class UGlobalFunctionToken : public UExpressionToken
{
public:
    UGlobalFunctionToken() { Type = UToken::GlobalFunction; }
    ~UGlobalFunctionToken() {}
};

class UPrimitiveCastToken : public UScriptToken
{
public:
    UPrimitiveCastToken() { Type = UToken::PrimitiveCast; }
    ~UPrimitiveCastToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDynArrayInsertToken : public UExpressionToken
{
public:
    UDynArrayInsertToken() { Type = UToken::DynArrayInsert; Count = 4; }
    ~UDynArrayInsertToken() {}
};

class UReturnNothingToken : public UObjRefToken
{
public:
    UReturnNothingToken() { Type = UToken::ReturnNothing; }
    ~UReturnNothingToken() {}
};

class UDelegateCmpEqToken : public UExpressionToken
{
public:
    UDelegateCmpEqToken() { Type = UToken::DelegateCmpEq; Count = 3; }
    ~UDelegateCmpEqToken() {}
};

class UDelegateCmpNeToken : public UExpressionToken
{
public:
    UDelegateCmpNeToken() { Type = UToken::DelegateCmpNe; Count = 3; }
    ~UDelegateCmpNeToken() {}
};

class UDelegateFunctionCmpEqToken : public UExpressionToken
{
public:
    UDelegateFunctionCmpEqToken() { Type = UToken::DelegateFunctionCmpEq; Count = 3; }
    ~UDelegateFunctionCmpEqToken() {}
};

class UDelegateFunctionCmpNeToken : public UExpressionToken
{
public:
    UDelegateFunctionCmpNeToken() { Type = UToken::DelegateFunctionCmpNE; Count = 3; }
    ~UDelegateFunctionCmpNeToken() {}
};

class UNoDelegateToken : public UScriptToken
{
public:
    UNoDelegateToken() { Type = UToken::NoDelegate; }
    ~UNoDelegateToken() {}
};

class UDynArrayRemoveToken : public UExpressionToken
{
public:
    UDynArrayRemoveToken() { Type = UToken::DynArrayRemove; Count = 4; }
    ~UDynArrayRemoveToken() {}
};

class UDebugInfoToken : public UScriptToken
{
public:
    UDebugInfoToken() { Type = UToken::DebugInfo; }
    ~UDebugInfoToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDelegateFunctionToken : public UScriptToken
{
public:
    UDelegateFunctionToken() { Type = UToken::DelegateFunction; }
    ~UDelegateFunctionToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDelegatePropertyToken : public UScriptToken
{
public:
    UDelegatePropertyToken() { Type = UToken::DelegateProperty; }
    ~UDelegatePropertyToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class ULetDelegateToken : public ULetToken
{
public:
    ULetDelegateToken() { Type = UToken::LetDelegate; }
    ~ULetDelegateToken() {}
};

class UTernaryConditionToken : public UScriptToken
{
public:
    UTernaryConditionToken() { Type = UToken::TernaryCondition; }
    ~UTernaryConditionToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDynArrFindToken : public UScriptToken
{
public:
    UDynArrFindToken() { Type = UToken::DynArrFind; }
    ~UDynArrFindToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDynArrayFindStructToken : public UScriptToken
{
public:
    UDynArrayFindStructToken() { Type = UToken::DynArrayFindStruct; }
    ~UDynArrayFindStructToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UOutVariableToken : public UObjRefToken
{
public:
    UOutVariableToken() { Type = UToken::OutVariable; }
    ~UOutVariableToken() {}
};

class UDefaultParmValueToken : public UScriptToken
{
public:
    UDefaultParmValueToken() { Type = UToken::DefaultParmValue; }
    ~UDefaultParmValueToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UNoParmToken : public UScriptToken
{
public:
    UNoParmToken() { Type = UToken::NoParm; }
    ~UNoParmToken() {}
};

class UInstanceDelegateToken : public UNameIndexToken
{
public:
    UInstanceDelegateToken() { Type = UToken::InstanceDelegate; }
    ~UInstanceDelegateToken() {}
};

class UInterfaceContextToken : public UExpressionToken
{
public:
    UInterfaceContextToken() { Type = UToken::InterfaceContext; }
    ~UInterfaceContextToken() {}
};

class UInterfaceCastToken : public UMetaCastToken
{
public:
    UInterfaceCastToken() { Type = UToken::InterfaceCast; }
    ~UInterfaceCastToken() {}
};

class UEndOfScriptToken : public UScriptToken
{
public:
    UEndOfScriptToken() { Type = UToken::EndOfScript; }
    ~UEndOfScriptToken() {}
};

class UDynArrAddToken : public UExpressionToken
{
public:
    UDynArrAddToken() { Type = UToken::DynArrAdd; Count = 2; }
    ~UDynArrAddToken() {}
};

class UDynArrAddItemToken : public UDynArrFindToken
{
public:
    UDynArrAddItemToken() { Type = UToken::DynArrAddItem; }
    ~UDynArrAddItemToken() {}
};

class UDynArrRemoveItemToken : public UDynArrAddItemToken
{
public:
    UDynArrRemoveItemToken() { Type = UToken::DynArrRemoveItem; }
    ~UDynArrRemoveItemToken() {}
};

class UDynArrInsertItemToken : public UDynArrayFindStructToken
{
public:
    UDynArrInsertItemToken() { Type = UToken::DynArrInsertItem; }
    ~UDynArrInsertItemToken() {}
};

class UDynArrIteratorToken : public UScriptToken
{
public:
    UDynArrIteratorToken() { Type = UToken::DynArrIterator; }
    ~UDynArrIteratorToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

class UDynArrSortToken : public UDynArrFindToken
{
public:
    UDynArrSortToken() { Type = UToken::DynArrSort; }
    ~UDynArrSortToken() {}
};

class UUnknownDeprecatedToken : public UScriptToken
{
public:
    UUnknownDeprecatedToken(UToken T) { Type = T; }
    UUnknownDeprecatedToken() { Type = UToken::UnknownDeprecated; }
    ~UUnknownDeprecatedToken() {}
};

class UUnknownDynamicVariableToken : public UScriptToken
{
public:
    UUnknownDynamicVariableToken(UToken T) { Type = T; }
    UUnknownDynamicVariableToken() { Type = UToken::UnknownDynamicVariable1; }
    ~UUnknownDynamicVariableToken() {}
};

class UUnknownFilterEditorOnlyToken : public UScriptToken
{
public:
    UUnknownFilterEditorOnlyToken(UToken T) { Type = T; }
    UUnknownFilterEditorOnlyToken() { Type = UToken::UnknownFilterEditorOnly1; }
    ~UUnknownFilterEditorOnlyToken() {}
};

class UNativeFunctionToken : public UScriptToken
{
public:
    UNativeFunctionToken(UToken T) { Type = T; }
    UNativeFunctionToken() { Type = UToken::ExtendedNative; }
    ~UNativeFunctionToken() {}
    std::string Deserialize(std::istream& stream, UPKInfo& info);
};

#endif // UTOKEN_H
