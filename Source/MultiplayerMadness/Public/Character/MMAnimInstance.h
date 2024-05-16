// Created by Brandon Davis

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MMAnimInstance.generated.h"

class AMMCharacter;
/**
 * 
 */
UCLASS()
class MULTIPLAYERMADNESS_API UMMAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AMMCharacter> MMCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	bool bIsCrouched;
};
