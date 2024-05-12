// Created by Brandon Davis

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERMADNESS_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerNetRole(APawn* InPawn);

protected:
	virtual void NativeDestruct() override;
};
