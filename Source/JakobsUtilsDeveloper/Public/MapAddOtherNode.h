#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "K2Node.h"
#include "EdGraph/EdGraphPin.h"
#include "MapAddOtherNode.generated.h"

/**
 * 
 */
UCLASS()
class UMapAddOtherNode: public UK2Node
{
	GENERATED_BODY()

public:
	UMapAddOtherNode();
	
	// Pin Accessors
	[[nodiscard]] UEdGraphPin* GetTargetMapPin() const;
	[[nodiscard]] UEdGraphPin* GetOtherMapPin() const;

	virtual bool ShouldDrawCompact() const override {return true;}
	
	// K2Node API
	[[nodiscard]] bool IsNodeSafeToIgnore() const override { return true; }
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual FText GetMenuCategory() const override;

	// EdGraphNode API
	void AllocateDefaultPins() override;
	void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	[[nodiscard]] FText GetNodeTitle (ENodeTitleType::Type TitleType) const override;
	[[nodiscard]] FText GetTooltipText() const override;
	[[nodiscard]] FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	void PinConnectionListChanged(UEdGraphPin* Pin) override;
	bool ShouldShowNodeProperties() const override {return true;}
	void PostPasteNode() override;

	// Object API
#if WITH_EDITOR
	void PostEditChangeProperty( FPropertyChangedEvent &PropertyChangedEvent ) override;
#endif

private:
	// Pin Names
	static const FName TargetMapPinName;
	static const FName OtherMapPinName;

	// Determine if there is any configuration options that shouldn't be allowed
	[[nodiscard]] bool CheckForErrors( const FKismetCompilerContext& CompilerContext );

	// Memory of what the input pin type is when it's a wildcard
	UPROPERTY()
	FEdGraphPinType InputWildcardType;

	// Memory of what the input pin type currently is
	UPROPERTY()
	FEdGraphPinType InputCurrentType;

	// Whether or not AllocateDefaultPins should fill out the wildcard types, or assign from the current types
	UPROPERTY()
	bool bOneTimeInit = true;
	
	FString TargetName;
	FString OtherName;
};

