// Created by Brandon Davis


#include "Character/MMCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"

// Sets default values
AMMCharacter::AMMCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;


	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetRootComponent());

	// Components do not need to be registered for replicated props
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	Combat->SetIsReplicated(true);

	// Allowing our player to use the built-in crouch
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

// Called when the game starts or when spawned
void AMMCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (MMMapingContext)
			{
				InputSystem->AddMappingContext(MMMapingContext, 0);
			}
		}
	}

}


// Called every frame
void AMMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}


// Called to bind functionality to input
void AMMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::MovePlayer);
	EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ThisClass::Jump);
	EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	EnhancedInput->BindAction(EquipAction, ETriggerEvent::Triggered, this, &ThisClass::EquipWeapon);
	EnhancedInput->BindAction(CrouchAction, ETriggerEvent::Started, this, &ThisClass::Crouch);
	EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &ThisClass::Aim);
	EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, this, &ThisClass::Aim);

}

void AMMCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (Combat)
	{
		Combat->Character = this;
	}
}

void AMMCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Only replicated on the owning client.
	DOREPLIFETIME_CONDITION(AMMCharacter, OverlappingWeapon, COND_OwnerOnly);

}

void AMMCharacter::AimOffset(float DeltaTime)
{
	if (Combat && !Combat->EquippedWeapon) return;
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if(Speed == 0.f && !bIsInAir) // Not moving and not jumping
	{
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);

		AimOffsetYaw = DeltaAimRotation.Yaw;
		bUseControllerRotationYaw = false;
	}

	if(Speed > 0.f || bIsInAir) // Running or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AimOffsetYaw = 0.f;
		bUseControllerRotationYaw = true;
	}

	AimOffsetPitch = GetBaseAimRotation().Pitch;

	if(AimOffsetPitch > 90.f && !IsLocallyControlled())
	{
		// Pitch is compressed to a short when sending a packet over the network then decompressed back
		// Map pitch from [270, 360) to [-90, 0) after it's been decompressed
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90, 0.f);
		AimOffsetPitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimOffsetPitch);
	}
}

void AMMCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	// Locally controlled on the server.
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(false);
		}
	}

	OverlappingWeapon = Weapon;
	// Locally controlled on the server.
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

void AMMCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}

	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AMMCharacter::MovePlayer(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

	if (Axis.Y != 0.f)
	{
		const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(Direction, Axis.Y);
	}

	if (Axis.X != 0.f)
	{
		const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(Direction, Axis.X);
	}
}

void AMMCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();

	AddControllerPitchInput(Axis.Y);
	AddControllerYawInput(Axis.X);
}

void AMMCharacter::EquipWeapon(const FInputActionValue& Value)
{
	if (Combat)
	{
		// Equip on the server
		if (HasAuthority())
		{
			Combat->EquipWeapon(OverlappingWeapon);

		}
		else
		{
			// Equip on the clients
			ServerEquipWeapon();
		}
	}
}

void AMMCharacter::Crouch(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
		return;
	}
	ACharacter::Crouch();
}

void AMMCharacter::Aim(const FInputActionValue& Value)
{
	bool Aiming = Value.Get<bool>();

	if (Combat)
	{
		if (Aiming)
		{
			Combat->SetAiming(true);
		}
		else
		{
			Combat->SetAiming(false);
		}
	}
}

bool AMMCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

bool AMMCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

void AMMCharacter::ServerEquipWeapon_Implementation()
{
	// RPC from client to sever
	if (Combat)
	{
		Combat->EquipWeapon(OverlappingWeapon);
	}

}




