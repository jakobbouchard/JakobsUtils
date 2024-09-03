
#pragma once

class FKismetCompilerContext;
class UEdGraphPin;
class UK2Node;
class FBlueprintActionDatabaseRegistrar;
class UK2Node_CustomEvent;

// Utilities for writing custom K2 nodes
namespace CoreTechK2Utilities
{
	static const FName Self_ParamName( TEXT( "self" ) );
	static const FName WorldContext_ParamName( TEXT( "WorldContext" ) );
	static const FName PN_WorldContextObject( TEXT( "WorldContextObject" ) );

	// Handle the copy of data from one pin to another either moving the links or copying the default value data
	void MovePinLinksOrCopyDefaults( FKismetCompilerContext &CompilerContext, UEdGraphPin *Source, UEdGraphPin *Dest );

	// Forcibly detach and attempt to reattach all the links from the pin to other pins
	 void RefreshAllowedConnections( const UK2Node *K2Node, UEdGraphPin *Pin );

	// Get the pin that is acting as an input to the specified pin
	[[nodiscard]]  UEdGraphPin* GetInputPinLink( UEdGraphPin *Pin );

	// Set the tooltip for a pin and prepends the type information to specified tooltip
	 void SetPinToolTip( UEdGraphPin *MutablePin, const FText &PinDescription = FText( ) );

	// Utility function wrapping the bare minimum code needed for implementing overrides of UK2Node::GetMenuActions
	 void DefaultGetMenuActions( const UK2Node *Node, FBlueprintActionDatabaseRegistrar& ActionRegistrar );

	// Utility for creating event nodes that can be used to bind BlueprintInternal function delegate params to custom k2node exec output pins
	 UK2Node_CustomEvent* CreateCustomEvent( FKismetCompilerContext &CompilerContext, UEdGraphPin *SourcePin, UEdGraph *SourceGraph, UK2Node *Node, UEdGraphPin *ExternalPin );

	// Delegates used by the CreateFunctionPins and ExpandFunctionPins functions to delegate certain features
	DECLARE_DELEGATE_RetVal_OneParam( FName, FGetPinName, FProperty* );
	DECLARE_DELEGATE_RetVal_OneParam( FText, FGetPinText, FProperty* );
	// Create all pins required for a function signature
	 TArray< UEdGraphPin* > CreateFunctionPins( UK2Node *Node, UFunction *Signature, EEdGraphPinDirection, bool bMakeAdvanced, const FGetPinName &GetPinName, const FGetPinText &GetPinTooltip = { } );

	// Create the pins required for any multi-dispatch delegates
	 void CreateEventDispatcherPins( UClass *Class, UK2Node *Node, TArray< UEdGraphPin* > *OutDispatcherPins, bool bMakeAdvanced, const TArray< FName > &IgnoreDispatchers = { } );

	// Delegate used by ExpandFunctionPins to delegate certain features
	DECLARE_DELEGATE_TwoParams( FDoPinExpansion, FProperty*, UEdGraphPin* );
	// Execute logic for each of the function pins doing whatever the DoPinExpansion delegate wants
	 void ExpandFunctionPins( UK2Node *Node, UFunction *Signature, EEdGraphPinDirection Dir, const FGetPinName &GetPinName, const FDoPinExpansion &DoPinExpansion );

	// Attach pins created by CreateEventDispatcherPins to the object
	[[nodiscard]]  UEdGraphPin* ExpandDispatcherPins( FKismetCompilerContext &CompilerContext, UEdGraph *SourceGraph, UK2Node *Node, UEdGraphPin *ExecPin, UClass *Class, UEdGraphPin *InstancePin, TFunction< bool( UEdGraphPin* ) > IsGeneratedPin );

	// Change the order of a pin within the Node pins
	 void ReorderPin( UK2Node *Node, UEdGraphPin *Pin, int NewIndex );

	// Retrieve the standard UE4 icon and color for a regular function
	[[nodiscard]] FSlateIcon GetFunctionIconAndTint( FLinearColor& OutColor );
	// Retrieve the standard UE4 icon and color for a pure function
	[[nodiscard]] FSlateIcon GetPureFunctionIconAndTint( FLinearColor& OutColor );
}
