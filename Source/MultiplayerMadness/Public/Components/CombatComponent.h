// Created by Brandon Davis

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYERMADNESS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	// Doing this is fine because the combat component was made for the character
	friend class AMMCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(class AWeapon* WeaponToEquip);
protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<class AMMCharacter> Character;
	UPROPERTY(Replicated)
	TObjectPtr<class AWeapon> EquippedWeapon;


public:	


		
};
