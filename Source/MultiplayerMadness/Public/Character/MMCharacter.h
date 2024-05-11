// Created by Brandon Davis

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MMCharacter.generated.h"

UCLASS()
class MULTIPLAYERMADNESS_API AMMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMMCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
