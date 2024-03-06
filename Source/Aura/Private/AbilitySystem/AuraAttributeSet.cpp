// Copyright Silver Desert Software


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

#include "Net/UnrealNetwork.h"
#include "AuraGameplayTags.h"
#include "Interfaces/CombatInterface.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	AddDelegatesToList();
}

void UAuraAttributeSet::AddDelegatesToList()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	AddVitalDelegatesToList(GameplayTags);
	AddPrimaryDelegatesToList(GameplayTags);
	AddSecondaryDelegatesToList(GameplayTags);
}

void UAuraAttributeSet::AddVitalDelegatesToList(const FAuraGameplayTags& GameplayTags)
{
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Vital_Mana, GetManaAttribute);
}

void UAuraAttributeSet::AddPrimaryDelegatesToList(const FAuraGameplayTags& GameplayTags)
{
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);
}

void UAuraAttributeSet::AddSecondaryDelegatesToList(const FAuraGameplayTags& GameplayTags)
{
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Primary
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience, COND_None, REPNOTIFY_Always);

	// Secondary
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);

	// Vital
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
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

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
		return;
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}

	ProcessDamage(Data, EffectProperties);
}

void UAuraAttributeSet::ProcessDamage(const FGameplayEffectModCallbackData& Data, FEffectProperties EffectProperties)
{
	if (Data.EvaluatedData.Attribute != GetIncomingDamageAttribute())
	{
		return;
	}

	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage <= 0)
	{
		return;
	}

	const float NewHealth = GetHealth() - LocalIncomingDamage;
	SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

	if (NewHealth <= 0)
	{
		if (ICombatInterface *CombatInterface = Cast<ICombatInterface>(EffectProperties.Target.AvatarActor))
		{
			CombatInterface->Die();
		}
		
		return;
	}

	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FAuraGameplayTags::Get().Effect_HitReact);
	EffectProperties.Target.AbilitySystemComponent->TryActivateAbilitiesByTag(TagContainer);
}


void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}
