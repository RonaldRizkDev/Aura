// Copyright Silver Desert Software


#include "Characters/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);

	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(WeaponName);
	Weapon->SetupAttachment(GetMesh(), WeaponHandSocketName);
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

int32 AAuraCharacterBase::GetPLayerLevel() const
{
	return 0;
}

void AAuraCharacterBase::Die()
{
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	HandleDeath();
}

void AAuraCharacterBase::HandleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);;
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	if (!HasAuthority()) return;

	UAuraAbilitySystemComponent* AuraAsc = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	AuraAsc->AddCharacterAbilities(StartupAbilities);
}

UAnimMontage* AAuraCharacterBase::GetHitReactionMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect> GameplayEffect, const float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffect);
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, Level, ContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());	
}
