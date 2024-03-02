// Copyright Silver Desert Software


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interfaces/HighlightInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	CurrentActor = nullptr;
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(AuraContext,  0);
	}
	
	bShowMouseCursor = true;

	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityActions(InputConfig, this,
		&ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2d InputAxisVector = InputActionValue.Get<FVector2d>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn =GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (!CursorHit.bBlockingHit) return;

	const TObjectPtr<IHighlightInterface> LastActor = CurrentActor;
	CurrentActor = Cast<IHighlightInterface>(CursorHit.GetActor());

	if (LastActor == nullptr)
	{
		if (CurrentActor != nullptr)
		{
			CurrentActor->CreateActorHighlight();
		}

		return;
	}

	if (CurrentActor == nullptr)
	{
		LastActor->RemoveActorHighlight();
		return;
	}

	if (LastActor != CurrentActor)
	{
		CurrentActor->CreateActorHighlight();
		LastActor->RemoveActorHighlight();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) == false)
	{
		return;
	}
	
	bTargeting = CurrentActor != nullptr;
	bAutoRunning = false;
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) == false)
	{
		if (GetAsc() == nullptr) return;
		GetAsc()->AbilityInputTagHeld(InputTag);
		return;
	}

	if (bTargeting)
	{
		if (GetAsc() == nullptr) return;
		GetAsc()->AbilityInputTagHeld(InputTag);
		return;
	}

	FollowTime += GetWorld()->GetDeltaSeconds();

	if (FHitResult Hit; GetHitResultUnderCursor(ECC_Vehicle, false, Hit))
	{
		CachedDestination = Hit.ImpactPoint;
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector WorldDestination = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDestination);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (GetAsc() == nullptr) return;
	GetAsc()->AbilityInputTagReleased(InputTag);
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAsc()
{
	if (AuraAbilitySystemComponent != nullptr)
	{
		return AuraAbilitySystemComponent;
	}

	AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));

	return AuraAbilitySystemComponent;
}
