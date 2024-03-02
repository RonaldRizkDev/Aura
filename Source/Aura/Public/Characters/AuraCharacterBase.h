// Copyright Silver Desert Software

#pragma once

#include<string>
#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class AURA_API AAuraCharacterBase
	: public ACharacter,
		public IAbilitySystemInterface,
		public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual int32 GetPLayerLevel() const override;
	
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo();;
	
	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category="Combat")
	FName WeaponTipSocketName;

	virtual FVector GetCombatSocketLocation() override;
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	void InitializeDefaultAttributes() const;
	void AddCharacterAbilities();
	
private:
	const FName WeaponName = "Weapon";
	const FName WeaponHandSocketName = "WeaponHandSocket";

	UPROPERTY(EditAnywhere, Category="Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffect, const float Level) const;
};
