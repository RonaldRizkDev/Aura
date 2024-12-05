// Copyright Silver Desert Software


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interfaces/HighlightInterface.h"
#include "UI/Widget/DamageTextComponent.h"

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

	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float Damage, ACharacter* TargetCharacter, const bool bBlockedHit, const bool bCriticalHit)
{
	if (IsValid(TargetCharacter) == false || DamageTextComponentClass == nullptr || IsLocalController() == false) return;

	UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
	DamageText->RegisterComponent();
	DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DamageText->SetDamageText(Damage, bBlockedHit, bCriticalHit);
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
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	
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

void AAuraPlayerController::ShiftPressed()
{
	bShiftKeyDown = true;
}

void AAuraPlayerController::ShiftReleased()
{
	bShiftKeyDown = false;
}

void AAuraPlayerController::CursorTrace()
{	
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (!CursorHit.bBlockingHit) return;

	const TObjectPtr<IHighlightInterface> LastActor = CurrentActor;
	CurrentActor = Cast<IHighlightInterface>(CursorHit.GetActor());

	if (LastActor == CurrentActor)
	{
		return;
	}

	if (LastActor) LastActor->RemoveActorHighlight();
	if (CurrentActor) CurrentActor->CreateActorHighlight();
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

	if (bTargeting || bShiftKeyDown)
	{
		if (GetAsc() == nullptr) return;
		GetAsc()->AbilityInputTagHeld(InputTag);
		return;
	}

	FollowTime += GetWorld()->GetDeltaSeconds();

	if (CursorHit.bBlockingHit)
	{
		CachedDestination = CursorHit.ImpactPoint;
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector WorldDestination = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDestination);
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) == false)
	{
		if (GetAsc() == nullptr) return;
		GetAsc()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (bTargeting)
	{
		if (GetAsc() == nullptr) return;
		GetAsc()->AbilityInputTagReleased(InputTag);
		return;
	}

	const APawn* ControlledPawn = GetPawn();
	if (FollowTime > ShortPressedThreshold || ControlledPawn == nullptr)
	{
		FollowTime = 0;
		bTargeting = false;
		return;
	}

	FollowTime = 0;
	bTargeting = false;
	const UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination);
	
	if (NavPath == nullptr)
	{
		return;
	}

	Spline->ClearSplinePoints();
	for (const FVector& PointLoc : NavPath->PathPoints)
	{
		Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
		DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
	}

	const int NumPathCnt = NavPath->PathPoints.Num();;
	if (NumPathCnt > 0)
	{
		CachedDestination = NavPath->PathPoints[NumPathCnt - 1];
	}
	
	bAutoRunning = true;	
}

void AAuraPlayerController::AutoRun()
{
	if (bAutoRunning == false) return;
	
	APawn* ControllerPawn = GetPawn();
	if (!ControllerPawn) return;

	const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControllerPawn->GetActorLocation(), ESplineCoordinateSpace::World);
	const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
	ControllerPawn->AddMovementInput(Direction);

	const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
	bAutoRunning = DistanceToDestination > AutoRunAcceptanceRadius;
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
