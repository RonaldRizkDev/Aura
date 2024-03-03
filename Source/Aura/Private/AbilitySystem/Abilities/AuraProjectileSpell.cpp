// Copyright Silver Desert Software


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

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
	
	// Give the projectile a gameplay effect spec for causing damage
	
	Projectile->FinishSpawning(SpawnTransform);
}
