// Copyright Silver Desert Software


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interfaces/CombatInterface.h"

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectionTargetLocation)
{
	if (GetAvatarActorFromActorInfo()->HasAuthority() == false) return;
	
	ICombatInterface *CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface == nullptr)
	{
		return;
	}
	
	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	FRotator Rotation = (ProjectionTargetLocation - SocketLocation).Rotation();
	Rotation.Pitch = 0.f;
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	const UAbilitySystemComponent* SourceAsc = UAbilitySystemBlueprintLibrary
		::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
	const FGameplayEffectSpecHandle SpecHandle =  SourceAsc
		->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceAsc->MakeEffectContext());
	Projectile->DamageEffectSpecHandle = SpecHandle;
	
	Projectile->FinishSpawning(SpawnTransform);
}
