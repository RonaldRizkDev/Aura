// Copyright Silver Desert Software


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetUnderDataMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse *TargetData = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return TargetData;
}

void UTargetDataUnderMouse::Activate()
{
	if (Ability->GetCurrentActorInfo()->IsLocallyControlled())
	{
		SendMouseCursorData();
		return;
	}

	const FGameplayAbilitySpecHandle SpecHande = GetAbilitySpecHandle();
	const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent.Get()
		->AbilityTargetDataSetDelegate(SpecHande, ActivationPredictionKey)
		.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

	if (AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHande, ActivationPredictionKey))
	{
		return;
	}

	SetWaitingOnRemotePlayerData();
}

void UTargetDataUnderMouse::SendMouseCursorData() const
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	const APlayerController *PlayerController = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit *Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates() == false)
	{
		return;
	}

	ValidData.Broadcast(DataHandle);
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates() == false)
	{
		return;
	}

	ValidData.Broadcast(DataHandle);
}
