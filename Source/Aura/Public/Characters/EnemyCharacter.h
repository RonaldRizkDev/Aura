// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "Characters/AuraCharacterBase.h"
#include "Interfaces/HighlightInterface.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class AURA_API AEnemyCharacter : public AAuraCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

	// Highlight interface
	virtual void CreateActorHighlight() override;
	virtual void RemoveActorHighlight() override;

	//Combat Interface
	FORCEINLINE virtual int32 GetPLayerLevel() const override { return Level; }
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;
};
