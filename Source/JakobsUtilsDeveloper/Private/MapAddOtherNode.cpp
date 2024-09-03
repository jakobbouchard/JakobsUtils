#include "MapAddOtherNode.h"

#include "CoreTechK2Utilities.h"
#include "GraphEditorSettings.h"
#include "JakobsRuntimeLibrary.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "MapAddOtherNamespace"

const FName UMapAddOtherNode::TargetMapPinName = TEXT("Target");
const FName UMapAddOtherNode::OtherMapPinName = TEXT("Other");


UMapAddOtherNode::UMapAddOtherNode()
{
	TargetName = LOCTEXT("KeyPin_FriendlyName", "Map Key").ToString();
	OtherName = LOCTEXT("ValuePin_FriendlyName", "Map Value").ToString();
}

void UMapAddOtherNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Create an execution input pin
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT(""), nullptr, UEdGraphSchema_K2::PN_Execute);

	// Create an execution output pin
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT(""), nullptr, UEdGraphSchema_K2::PN_Then);

	FCreatePinParams MapPinParams;
	MapPinParams.ContainerType = EPinContainerType::Map;
	MapPinParams.ValueTerminalType.TerminalCategory = UEdGraphSchema_K2::PC_Wildcard;
	
	const auto TargetMapPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, TargetMapPinName, MapPinParams);
	TargetMapPin->PinType.bIsReference = true;
	TargetMapPin->PinFriendlyName = LOCTEXT("TargetMapPin_FriendlyName", "TargetMap");
	const auto OtherMapPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, OtherMapPinName, MapPinParams);
	OtherMapPin->PinType.bIsReference = true;
	OtherMapPin->PinFriendlyName = LOCTEXT("OtherMapPin_FriendlyName", "OtherMap");

	if (bOneTimeInit)
	{
		InputWildcardType = TargetMapPin->PinType;
		InputCurrentType = TargetMapPin->PinType;

		bOneTimeInit = false;
	}
	else
	{
		TargetMapPin->PinType = InputCurrentType;
		OtherMapPin->PinType = InputCurrentType;
	}

	CoreTechK2Utilities::SetPinToolTip(TargetMapPin, LOCTEXT("TargetMapPin_Tooltip", "Map to add to"));
	CoreTechK2Utilities::SetPinToolTip(OtherMapPin, LOCTEXT("OtherMapPin_Tooltip", "Map to add from"));
}

void UMapAddOtherNode::PostPasteNode()
{
	Super::PostPasteNode();
	const auto MapPin = GetTargetMapPin();
	const auto OtherMapPin = GetOtherMapPin();
	
	if (MapPin && OtherMapPin)
	{
		if (MapPin->LinkedTo.Num() == 0 && OtherMapPin->LinkedTo.Num() == 0)
			bOneTimeInit = true;
	}
	else
	{
		bOneTimeInit = true;
	}
}

#if WITH_EDITOR
void UMapAddOtherNode::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	bool bRefresh = false;

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UMapAddOtherNode, TargetMapPinName))
	{
		GetTargetMapPin()->PinFriendlyName = FText::FromString(TargetName);
		bRefresh = true;
	}
	else if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UMapAddOtherNode, OtherMapPinName))
	{
		GetOtherMapPin()->PinFriendlyName = FText::FromString(OtherName);
		bRefresh = true;
	}
	if (bRefresh)
	{
		// Poke the graph to update the visuals based on the above changes
		GetGraph()->NotifyGraphChanged();
		FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
	}
}
#endif


void UMapAddOtherNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	if (CheckForErrors(CompilerContext))
	{
		// remove all the links to this node as they are no longer needed
		BreakAllNodeLinks();
		return;
	}

	const auto K2Schema = GetDefault<UEdGraphSchema_K2>();

	///////////////////////////////////////////////////////////////////////////////////
	// Cache off versions of all our important pins
	const auto MapAdd_Exec = GetExecPin();
	const auto MapAdd_TargetMap = GetTargetMapPin();
	const auto MapAdd_OtherMap = GetOtherMapPin();
	const auto MapAdd_Then = GetThenPin();

	///////////////////////////////////////////////////////////////////////////////////
	//
	const auto CallMapAddOther = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallMapAddOther->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UJakobsRuntimeLibrary, Map_AddOther), UJakobsRuntimeLibrary::StaticClass());
	CallMapAddOther->AllocateDefaultPins();

	const auto Get_Exec = CallMapAddOther->GetExecPin();
	const auto Get_TargetMap = CallMapAddOther->FindPinChecked(TEXT("TargetMap"));
	const auto Get_OtherMap = CallMapAddOther->FindPinChecked(TEXT("OtherMap"));
	const auto Get_Then = CallMapAddOther->GetThenPin();

	Get_TargetMap->PinType = MapAdd_TargetMap->PinType;
	Get_OtherMap->PinType = MapAdd_OtherMap->PinType;
	CompilerContext.MovePinLinksToIntermediate(*MapAdd_TargetMap, *Get_TargetMap);
	CallMapAddOther->PinConnectionListChanged(Get_TargetMap);
	CompilerContext.MovePinLinksToIntermediate(*MapAdd_OtherMap, *Get_OtherMap);
	CallMapAddOther->PinConnectionListChanged(Get_OtherMap);

	CompilerContext.MovePinLinksToIntermediate(*MapAdd_Exec, *Get_Exec);
	CompilerContext.MovePinLinksToIntermediate(*MapAdd_Then, *Get_Then);

	BreakAllNodeLinks();
}


bool UMapAddOtherNode::CheckForErrors(const FKismetCompilerContext& CompilerContext)
{
	bool bError = false;

	if (GetTargetMapPin()->LinkedTo.Num() == 0 || GetOtherMapPin()->LinkedTo.Num() == 0)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("MissingMap_Error", "Each Map pin @@ must have a Map.").ToString( ), this);
		bError = true;
	}

	return bError;
}

void UMapAddOtherNode::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);

	if (Pin == nullptr)
	{
		return;
	}

	const auto TargetPin = GetTargetMapPin();
	const auto OtherPin = GetOtherMapPin();
	
	if (Pin->PinName == TargetMapPinName || Pin->PinName == OtherMapPinName)
	{
		const UEdGraphPin* LinkedPin = nullptr;
		if (Pin->LinkedTo.Num() > 0)
		{
			LinkedPin = Pin->LinkedTo[0];
		}
		if (LinkedPin)
		{
			if (TargetPin->PinType != LinkedPin->PinType)
			{
				TargetPin->PinType = LinkedPin->PinType;
				OtherPin->PinType = LinkedPin->PinType;
			}
			else
			{
				return;
			}
		}
		else
		{
			TargetPin->PinType = InputWildcardType;
			OtherPin->PinType = InputWildcardType;
		}
		InputCurrentType = Pin->PinType;
		
		CoreTechK2Utilities::SetPinToolTip(TargetPin, LOCTEXT("MapPin_Tooltip", "Map to add all elements to"));
		CoreTechK2Utilities::SetPinToolTip(OtherPin, LOCTEXT("OtherPin_Tooltip", "Map to add all elements from"));

		GetGraph()->NotifyGraphChanged();

		FBlueprintEditorUtils::MarkBlueprintAsModified(GetBlueprint());
	}
}

UEdGraphPin* UMapAddOtherNode::GetTargetMapPin() const
{
	return FindPinChecked(TargetMapPinName);
}

UEdGraphPin* UMapAddOtherNode::GetOtherMapPin() const
{
	return FindPinChecked(OtherMapPinName);
}

FText UMapAddOtherNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("NodeTitle_NONE", "Add Other Map");
}

FText UMapAddOtherNode::GetTooltipText( ) const
{
	return LOCTEXT("NodeToolTip", "Add a Map to another Map");
}

FText UMapAddOtherNode::GetMenuCategory( ) const
{
	return LOCTEXT("NodeMenu", "Utilities|Map");
}

FSlateIcon UMapAddOtherNode::GetIconAndTint(FLinearColor& OutColor) const
{
	return CoreTechK2Utilities::GetFunctionIconAndTint(OutColor);
}

void UMapAddOtherNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	CoreTechK2Utilities::DefaultGetMenuActions(this, ActionRegistrar);
}

#undef LOCTEXT_NAMESPACE
