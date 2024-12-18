// Copyright Silver Desert Software


#include "Characters/EnemyCharacter.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AruaAbilitySystemFunctionLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
}

void AEnemyCharacter::CreateActorHighlight()
{
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOMER_STENCIL_DEPTH_RED);
	if (Weapon == nullptr)
	{
		return;
	}

	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOMER_STENCIL_DEPTH_RED);
}

void AEnemyCharacter::RemoveActorHighlight()
{
	GetMesh()->SetRenderCustomDepth(false);
	if (Weapon == nullptr)
	{
		return;
	}

	Weapon->SetRenderCustomDepth(false);
}

void AEnemyCharacter::Die()
{
	SetLifeSpan(LifeSpan);
	Super::Die();
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (HasAuthority() == false)
	{
		return;
	}

	AuraAIController = Cast<AAuraAIController>(NewController);

	AuraAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
	AuraAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	AuraAIController->RunBehaviorTree(BehaviorTree);
}

void AEnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
	if (AuraAIController == nullptr)
	{
		return;
	}

	UBlackboardComponent* Blackboard = AuraAIController->GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		return;
	}

	Blackboard->SetValueAsBool(FName("HitReacting"), bHitReacting);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();
	
	ConfigureHealthBar();
}

void AEnemyCharacter::InitAbilityActorInfo()
{
	BaseWalkSpeed = GetCharacterMovement()->MaxWalkSpeed; 

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	if (HasAuthority() == false)
	{
		return;
	}

	InitializeDefaultAttributes();
}

void AEnemyCharacter::InitializeDefaultAttributes() const
{
	UAruaAbilitySystemFunctionLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
	UAruaAbilitySystemFunctionLibrary::GiveStartupAbilities(this, AbilitySystemComponent, CharacterClass);
}

void AEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AEnemyCharacter::ConfigureHealthBar()
{
	if (UAuraUserWidget *AuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		AuraUserWidget->SetWidgetController(this);
	}

	const UAuraAttributeSet *AuraAttrSet = Cast<UAuraAttributeSet>(AttributeSet);
	if (!AuraAttrSet)
	{
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttrSet->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		});
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttrSet->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		});

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Effect_HitReact,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AEnemyCharacter::HitReactTagChanged);

	OnHealthChanged.Broadcast(AuraAttrSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttrSet->GetMaxHealth());
}
