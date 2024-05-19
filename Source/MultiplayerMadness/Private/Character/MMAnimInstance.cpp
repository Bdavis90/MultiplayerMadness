// Created by Brandon Davis


#include "Character/MMAnimInstance.h"

#include "Character/MMCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "KismetAnimationLibrary.h"


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
	bWeaponEquipped = MMCharacter->IsWeaponEquipped();
	bIsCrouched = MMCharacter->bIsCrouched;
	bIsAiming = MMCharacter->IsAiming();

	// Offset Yaw for Strafing
	FRotator AimRotation = MMCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(MMCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;
	//YawOffset = UKismetAnimationLibrary::CalculateDirection(MMCharacter->GetVelocity(), MMCharacter->GetActorRotation());


	// Leaning
	LastFrameRotation = CurrentRotation;
	CurrentRotation = MMCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CurrentRotation, LastFrameRotation);
	const float Target = Delta.Yaw / DeltaSeconds;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

}
