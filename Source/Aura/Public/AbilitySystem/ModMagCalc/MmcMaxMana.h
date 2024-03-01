// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MmcMaxMana.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMmcMaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMmcMaxMana();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
