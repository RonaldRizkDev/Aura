// Copyright Silver Desert Software


#include "Characters/EnemyCharacter.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AEnemyCharacter::AEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

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

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilityActorInfo();

	ConfigureHealthBar();
}

void AEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();

	InitializeDefaultAttributes();
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

	OnHealthChanged.Broadcast(AuraAttrSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttrSet->GetMaxHealth());
}
