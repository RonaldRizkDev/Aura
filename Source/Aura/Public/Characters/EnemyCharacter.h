// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Characters/AuraCharacterBase.h"
#include "Interfaces/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "EnemyCharacter.generated.h"

class UWidgetComponent;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

UCLASS()
class AURA_API AEnemyCharacter : public AAuraCharacterBase, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	AEnemyCharacter();

	// Highlight interface
	virtual void CreateActorHighlight() override;
	virtual void RemoveActorHighlight() override;
	virtual void Die() override;
	
	//Combat Interface
	FORCEINLINE virtual int32 GetPLayerLevel() const override { return Level; }

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 500.f;

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

private:
	void ConfigureHealthBar();
};
