// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MmcMaxHealth.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMmcMaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMmcMaxHealth();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
