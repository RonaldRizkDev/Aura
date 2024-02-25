// Copyright Silver Desert Software


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

#include "Aura/Aura.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(ATTRIBUTE_HEALTH_START);
	InitMaxHealth(ATTRIBUTE_MAX_HEALTH);

	InitMana(ATTRIBUTE_MANA_START);
	InitMaxMana(ATTRIBUTE_MAX_MANA);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxHealth());
		return;
	}

	if (Attribute != GetManaAttribute())
	{
		return;
	}

	NewValue = FMath::Clamp(NewValue, 0, GetMaxMana());
}

void UAuraAttributeSet::SetEffectSourceProperties(
		const FGameplayEffectContextHandle& EffectContextHandle,
		FEffectCharacterProperties& CharacterProperties)
{
	CharacterProperties.AbilitySystemComponent = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	if (IsValid(CharacterProperties.AbilitySystemComponent) == false
			|| CharacterProperties.AbilitySystemComponent->AbilityActorInfo.IsValid() == false
			|| CharacterProperties.AbilitySystemComponent->AbilityActorInfo->AvatarActor.IsValid() == false)
	{
		return;
	}

	CharacterProperties.AvatarActor = CharacterProperties.AbilitySystemComponent->AbilityActorInfo->AvatarActor.Get();
	CharacterProperties.Controller = CharacterProperties.AbilitySystemComponent->AbilityActorInfo->PlayerController.Get();
	if (CharacterProperties.Controller)
	{
		CharacterProperties.Character = Cast<ACharacter>(CharacterProperties.Controller->GetPawn());
		return;
	}
	
	if (CharacterProperties.AvatarActor == nullptr)
	{
		return;
	}

	const APawn* Pawn = Cast<APawn>(CharacterProperties.AvatarActor);
	if (Pawn == nullptr)
	{
		return;
	}

	CharacterProperties.Controller = Pawn->GetController();
	if (CharacterProperties.Controller)
	{
		CharacterProperties.Character = Cast<ACharacter>(CharacterProperties.Controller->GetPawn());
	}
}

void UAuraAttributeSet::SetEffectTargetProperties(
		const FGameplayEffectModCallbackData& Data,
		FEffectCharacterProperties& CharacterProperties)
{
	if (Data.Target.AbilityActorInfo.IsValid() == false
		|| Data.Target.AbilityActorInfo->AvatarActor.IsValid() == false)
	{
		return;
	}

	CharacterProperties.AvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
	CharacterProperties.Controller = Data.Target.AbilityActorInfo->PlayerController.Get();
	CharacterProperties.Character = Cast<ACharacter>(CharacterProperties.AvatarActor);
	CharacterProperties.AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(CharacterProperties.AvatarActor);
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& EffectProperties)
{
	EffectProperties.EffectContextHandle = Data.EffectSpec.GetContext();
	SetEffectSourceProperties(EffectProperties.EffectContextHandle, EffectProperties.Source);	
	SetEffectTargetProperties(Data, EffectProperties.Target);
}


void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProperties;
	SetEffectProperties(Data, EffectProperties);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
