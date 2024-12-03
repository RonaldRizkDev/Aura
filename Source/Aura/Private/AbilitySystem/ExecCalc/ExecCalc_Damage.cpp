// Copyright Silver Desert Software


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AruaAbilitySystemFunctionLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interfaces/CombatInterface.h"

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;

	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent *SourceAsc = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent *TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();
	
	const AActor *SourceAvatar = SourceAsc ? SourceAsc->GetAvatarActor() : nullptr;
	const AActor *TargetAvatar = TargetAsc ? TargetAsc->GetAvatarActor() : nullptr;

	const UCharacterClassInfo* CharacterClassInfo = UAruaAbilitySystemFunctionLibrary::GetCharacterClassInfo(SourceAvatar);
	check(CharacterClassInfo);

	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	const int32 SourceLevel = SourceCombatInterface->GetPLayerLevel();
	const int32 TargetLevel = TargetCombatInterface->GetPLayerLevel();
	
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float TargetArmor = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);

	const FRealCurve *ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationsCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceLevel);

	const FRealCurve *BlockedDamageCurve = CharacterClassInfo->DamageCalculationsCoefficients->FindCurve(FName("BlockedDamage"), FString());
	const float BlockedDamageCoefficient = BlockedDamageCurve->Eval(SourceLevel);

	const FRealCurve *EffectiveArmorCurve = CharacterClassInfo->DamageCalculationsCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetLevel);

	const FRealCurve *CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationsCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetLevel);

	const FRealCurve *CriticalHitMultiplierCurve = CharacterClassInfo->DamageCalculationsCoefficients->FindCurve(FName("CriticalHitMultiplier"), FString());
	const float CriticalHitMultiplierCoefficient = CriticalHitMultiplierCurve->Eval(TargetLevel);

	// Get Aura Gameplay Effect Context Handle
	FAuraGameplayEffectContext *AuraContext = static_cast<FAuraGameplayEffectContext *>(Spec.GetContext().Get());
	
	// get Damage amount
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);
	
	float TargetBlockChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);
	const bool IsBlockedHit = TargetBlockChance > (rand() % 100);
	if (IsBlockedHit) Damage *= BlockedDamageCoefficient;
	AuraContext->SetBlockedHit(IsBlockedHit);
	
	float SourceArmorPenetration = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);
	const float EffectiveArmor = TargetArmor *= (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	// Reduce damage based on the targets armor, and the sources armor penetration calculations
	Damage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// Calculate critical hits
	float SourceCriticalHitChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(0.f, SourceCriticalHitChance);
	float SourceCriticalHitDamage = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
	SourceCriticalHitDamage = FMath::Max<float>(0.f, SourceCriticalHitDamage);
	float TargetCriticalHitResistance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(0.f, TargetCriticalHitResistance);

	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool IsCriticalHit = EffectiveCriticalHitChance > FMath::RandRange(1, 100);
	if (IsCriticalHit) Damage = Damage * CriticalHitMultiplierCoefficient + SourceCriticalHitDamage; 
	AuraContext->SetCriticalHit(IsCriticalHit);
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
