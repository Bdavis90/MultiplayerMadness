// Created by Brandon Davis


#include "Character/MMAnimInstance.h"

#include "Character/MMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMMAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MMCharacter = Cast<AMMCharacter>(TryGetPawnOwner());
}

void UMMAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!MMCharacter)
	{
		MMCharacter = Cast<AMMCharacter>(TryGetPawnOwner());
	}

	if (!MMCharacter) return;

	FVector Velocity = MMCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = MMCharacter->GetCharacterMovement()->IsFalling();

	bIsAccelerating = MMCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
}
