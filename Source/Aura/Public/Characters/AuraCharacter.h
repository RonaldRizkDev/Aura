// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "HLSLTypeAliases.h"
#include "Characters/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual int32 GetPLayerLevel() const override;
	
private:
	virtual void InitAbilityActorInfo() override;
};
