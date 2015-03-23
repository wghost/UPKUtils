#include "UTokenFactory.h"

UScriptToken* UTokenFactory::Create(UToken Type)
{
	if (Type == UToken::LocalVariable)
	{
		return new ULocalVariableToken;
	}
	else if (Type == UToken::InstanceVariable)
	{
		return new UInstanceVariableToken;
	}
	else if (Type == UToken::DefaultVariable)
	{
		return new UDefaultVariableToken;
	}
	else if (Type == UToken::StateVariable)
	{
		return new UStateVariableToken;
	}
	else if (Type == UToken::Return)
	{
		return new UReturnToken;
	}
	else if (Type == UToken::Switch)
	{
		return new USwitchToken;
	}
	else if (Type == UToken::Jump)
	{
		return new UJumpToken;
	}
	else if (Type == UToken::JumpIfNot)
	{
		return new UJumpIfNotToken;
	}
	else if (Type == UToken::Stop)
	{
		return new UStopToken;
	}
	else if (Type == UToken::Assert)
	{
		return new UAssertToken;
	}
	else if (Type == UToken::Case)
	{
		return new UCaseToken;
	}
	else if (Type == UToken::Nothing)
	{
		return new UNothingToken;
	}
	else if (Type == UToken::LabelTable)
	{
		return new ULabelTableToken;
	}
	else if (Type == UToken::GotoLabel)
	{
		return new UGotoLabelToken;
	}
	else if (Type == UToken::EatString)
	{
		return new UEatStringToken;
	}
	else if (Type == UToken::Let)
	{
		return new ULetToken;
	}
	else if (Type == UToken::DynArrayElement)
	{
		return new UDynArrayElementToken;
	}
	else if (Type == UToken::New)
	{
		return new UNewToken;
	}
	else if (Type == UToken::ClassContext)
	{
		return new UClassContextToken;
	}
	else if (Type == UToken::MetaCast)
	{
		return new UMetaCastToken;
	}
	else if (Type == UToken::LetBool)
	{
		return new ULetBoolToken;
	}
	else if (Type == UToken::EndParmValue)
	{
		return new UEndParmValueToken;
	}
	else if (Type == UToken::EndFunctionParms)
	{
		return new UEndFunctionParmsToken;
	}
	else if (Type == UToken::Self)
	{
		return new USelfToken;
	}
	else if (Type == UToken::Skip)
	{
		return new USkipToken;
	}
	else if (Type == UToken::Context)
	{
		return new UContextToken;
	}
	else if (Type == UToken::ArrayElement)
	{
		return new UArrayElementToken;
	}
	else if (Type == UToken::VirtualFunction)
	{
		return new UVirtualFunctionToken;
	}
	else if (Type == UToken::FinalFunction)
	{
		return new UFinalFunctionToken;
	}
	else if (Type == UToken::IntConst)
	{
		return new UIntConstToken;
	}
	else if (Type == UToken::FloatConst)
	{
		return new UFloatConstToken;
	}
	else if (Type == UToken::StringConst)
	{
		return new UStringConstToken;
	}
	else if (Type == UToken::ObjectConst)
	{
		return new UObjectConstToken;
	}
	else if (Type == UToken::NameConst)
	{
		return new UNameConstToken;
	}
	else if (Type == UToken::RotatorConst)
	{
		return new URotatorConstToken;
	}
	else if (Type == UToken::VectorConst)
	{
		return new UVectorConstToken;
	}
	else if (Type == UToken::ByteConst)
	{
		return new UByteConstToken;
	}
	else if (Type == UToken::IntZero)
	{
		return new UIntZeroToken;
	}
	else if (Type == UToken::IntOne)
	{
		return new UIntOneToken;
	}
	else if (Type == UToken::True)
	{
		return new UTrueToken;
	}
	else if (Type == UToken::False)
	{
		return new UFalseToken;
	}
	else if (Type == UToken::NativeParm)
	{
		return new UNativeParmToken;
	}
	else if (Type == UToken::NoObject)
	{
		return new UNoObjectToken;
	}
	else if (Type == UToken::UnknownDeprecated)
	{
		return new UUnknownDeprecatedToken(Type);
	}
	else if (Type == UToken::IntConstByte)
	{
		return new UIntConstByteToken;
	}
	else if (Type == UToken::BoolVariable)
	{
		return new UBoolVariableToken;
	}
	else if (Type == UToken::DynamicCast)
	{
		return new UDynamicCastToken;
	}
	else if (Type == UToken::Iterator)
	{
		return new UIteratorToken;
	}
	else if (Type == UToken::IteratorPop)
	{
		return new UIteratorPopToken;
	}
	else if (Type == UToken::IteratorNext)
	{
		return new UIteratorNextToken;
	}
	else if (Type == UToken::StructCmpEq)
	{
		return new UStructCmpEqToken;
	}
	else if (Type == UToken::StructCmpNe)
	{
		return new UStructCmpNeToken;
	}
	else if (Type == UToken::UniStringConst)
	{
		return new UUniStringConstToken;
	}
	else if (Type == UToken::StructMember)
	{
		return new UStructMemberToken;
	}
	else if (Type == UToken::DynArrayLen)
	{
		return new UDynArrayLenToken;
	}
	else if (Type == UToken::GlobalFunction)
	{
		return new UGlobalFunctionToken;
	}
	else if (Type == UToken::PrimitiveCast)
	{
		return new UPrimitiveCastToken;
	}
	else if (Type == UToken::DynArrayInsert)
	{
		return new UDynArrayInsertToken;
	}
	else if (Type == UToken::ReturnNothing)
	{
		return new UReturnNothingToken;
	}
	else if (Type == UToken::DelegateCmpEq)
	{
		return new UDelegateCmpEqToken;
	}
	else if (Type == UToken::DelegateCmpNe)
	{
		return new UDelegateCmpNeToken;
	}
	else if (Type == UToken::DelegateFunctionCmpEq)
	{
		return new UDelegateFunctionCmpEqToken;
	}
	else if (Type == UToken::DelegateFunctionCmpNE)
	{
		return new UDelegateFunctionCmpNeToken;
	}
	else if (Type == UToken::NoDelegate)
	{
		return new UNoDelegateToken;
	}
	else if (Type == UToken::DynArrayRemove)
	{
		return new UDynArrayRemoveToken;
	}
	else if (Type == UToken::DebugInfo)
	{
		return new UDebugInfoToken;
	}
	else if (Type == UToken::DelegateFunction)
	{
		return new UDelegateFunctionToken;
	}
	else if (Type == UToken::DelegateProperty)
	{
		return new UDelegatePropertyToken;
	}
	else if (Type == UToken::LetDelegate)
	{
		return new ULetDelegateToken;
	}
	else if (Type == UToken::TernaryCondition)
	{
		return new UTernaryConditionToken;
	}
	else if (Type == UToken::DynArrFind)
	{
		return new UDynArrFindToken;
	}
	else if (Type == UToken::DynArrayFindStruct)
	{
		return new UDynArrayFindStructToken;
	}
	else if (Type == UToken::OutVariable)
	{
		return new UOutVariableToken;
	}
	else if (Type == UToken::DefaultParmValue)
	{
		return new UDefaultParmValueToken;
	}
	else if (Type == UToken::NoParm)
	{
		return new UNoParmToken;
	}
	else if (Type == UToken::InstanceDelegate)
	{
		return new UInstanceDelegateToken;
	}
	else if (Type >= UToken::UnknownDynamicVariable1 && Type <= UToken::UnknownDynamicVariable5)
	{
		return new UUnknownDynamicVariableToken(Type);
	}
	else if (Type == UToken::InterfaceContext)
	{
		return new UInterfaceContextToken;
	}
	else if (Type == UToken::InterfaceCast)
	{
		return new UInterfaceCastToken;
	}
	else if (Type == UToken::EndOfScript)
	{
		return new UEndOfScriptToken;
	}
	else if (Type == UToken::DynArrAdd)
	{
		return new UDynArrAddToken;
	}
	else if (Type == UToken::DynArrAddItem)
	{
		return new UDynArrAddItemToken;
	}
	else if (Type == UToken::DynArrRemoveItem)
	{
		return new UDynArrRemoveItemToken;
	}
	else if (Type == UToken::DynArrInsertItem)
	{
		return new UDynArrInsertItemToken;
	}
	else if (Type == UToken::DynArrIterator)
	{
		return new UDynArrIteratorToken;
	}
	else if (Type == UToken::DynArrSort)
	{
		return new UDynArrSortToken;
	}
	else if (Type >= UToken::UnknownFilterEditorOnly1 && Type <= UToken::UnknownFilterEditorOnly6)
	{
		return new UUnknownFilterEditorOnlyToken(Type);
	}
	else if (Type >= UToken::ExtendedNative && Type <= UToken::NativeFunctionF)
	{
		return new UNativeFunctionToken(Type);
	}
    else
    {
        return nullptr;
    }
}
