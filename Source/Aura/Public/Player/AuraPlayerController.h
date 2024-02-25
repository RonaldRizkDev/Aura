// Copyright Silver Desert Software

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
// #include "Interfaces/HighlightInterface.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class IHighlightInterface;

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
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	TObjectPtr<IHighlightInterface> CurrentActor;
	//IHighlightInterface* LastActor;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const struct FInputActionValue& InputActionValue);

	void CursorTrace();
};
