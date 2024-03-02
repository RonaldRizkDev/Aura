// Copyright Silver Desert Software


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interfaces/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const bool bIsServer = HasAuthority(&ActivationInfo);
	if (bIsServer == false) return;

	ICombatInterface *CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo());
	if (CombatInterface == nullptr)
	{
		return;
	}

	const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
	
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	
	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	// Give the projectile a gameplay effect spec for causing damage

	Projectile->FinishSpawning(SpawnTransform);
}
