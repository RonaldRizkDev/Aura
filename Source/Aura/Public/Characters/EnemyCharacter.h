// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interfaces/HighlightInterface.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AEnemyCharacter : public AAuraCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	virtual void CreateActorHighlight() override;
	virtual void RemoveActorHighlight() override;
};
