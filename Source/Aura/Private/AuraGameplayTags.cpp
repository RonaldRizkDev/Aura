// Copyright Silver Desert Software


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Vital.Health"),
					FString(""));
	GameplayTags.Attributes_Vital_Mana = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Vital.Mana"),
					FString(""));

	GameplayTags.Attributes_Primary_Strength = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Primary.Strength"),
					FString(""));
	GameplayTags.Attributes_Primary_Intelligence = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Primary.Intelligence"),
					FString(""));
	GameplayTags.Attributes_Primary_Resilience = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Primary.Resilience"),
					FString(""));
	GameplayTags.Attributes_Primary_Vigor = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Primary.Vigor"),
					FString(""));

	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.Armor"),
					FString("Reduces damage taken, improves block chances"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.ArmorPenetration"),
					FString(""));
	GameplayTags.Attributes_Secondary_BlockChance = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.BlockChance"),
					FString(""));
	GameplayTags.Attributes_Secondary_CriticalHitChance = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitChance"),
					FString(""));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitDamage"),
					FString(""));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.CriticalHitResistance"),
					FString(""));
	GameplayTags.Attributes_Secondary_HealthRegeneration = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.HealthRegeneration"),
					FString(""));
	GameplayTags.Attributes_Secondary_ManaRegeneration = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.ManaRegeneration"),
					FString(""));
	GameplayTags.Attributes_Secondary_MaxHealth = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.MaxHealth"),
					FString(""));
	GameplayTags.Attributes_Secondary_MaxMana = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Secondary.MaxMana"),
					FString(""));

	GameplayTags.InputTag_1 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("InputTag.1"),
					FString(""));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("InputTag.2"),
					FString(""));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("InputTag.3"),
					FString(""));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("InputTag.4"),
					FString(""));
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("InputTag.LMB"),
					FString(""));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("InputTag.RMB"),
					FString(""));
	/*
	 *	Damages
	 */
	GameplayTags.Damage = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Damage"),
			FString(""));
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Damage.Fire"),
			FString("Damage From Fire Attack"));
	GameplayTags.Damage_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Damage.Lightning"),
			FString("Damage From Lightning Attack"));
	GameplayTags.Damage_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Damage.Arcane"),
			FString("Damage From Arcane Attack"));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Damage.Physical"),
			FString("Damage From Physical Attack"));

	/*
	 *	Resistances
	 */
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Resistance.Fire"),
			FString("Resistance From Fire Damage"));
	GameplayTags.Attributes_Resistance_Lightning = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Resistance.Lightning"),
			FString("Resistance From Lightning Damage"));
	GameplayTags.Attributes_Resistance_Arcane = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Resistance.Arcane"),
			FString("Resistance From Arcane Damage"));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Attributes.Resistance.Physical"),
			FString("Resistance From Physical Damage"));

	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire, GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Lightning, GameplayTags.Attributes_Resistance_Lightning);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Arcane, GameplayTags.Attributes_Resistance_Arcane);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical, GameplayTags.Attributes_Resistance_Physical);

	/*
	 * Effects
	 */
	GameplayTags.Effect_HitReact = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Effect.HitReact"),
			FString(""));

	/*
	 * Attacks
	 */
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get()
		.AddNativeGameplayTag(FName("Abilities.Attack"),
			FString("Attack Ability Tag"));
}
