// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class IHighlightInterface;
class ACharacter;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(const float Damage, ACharacter* TargetCharacter, const bool bBlockedHit, const bool bCriticalHit);
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	FHitResult CursorHit;
	
	TObjectPtr<IHighlightInterface> CurrentActor;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void Move(const struct FInputActionValue& InputActionValue);
	void ShiftPressed();
	void ShiftReleased();
	bool bShiftKeyDown = false;
	
	void CursorTrace();

	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);

	void AutoRun();
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
	
	UAuraAbilitySystemComponent* GetAsc();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressedThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;

	TObjectPtr<USplineComponent> Spline;
};
