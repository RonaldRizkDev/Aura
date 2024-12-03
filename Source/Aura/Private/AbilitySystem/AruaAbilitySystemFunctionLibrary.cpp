// Copyright Silver Desert Software


#include "AbilitySystem/AruaAbilitySystemFunctionLibrary.h"

#include "AuraAbilityTypes.h"
#include "Game/AuraGameModeBase.h"
#include "kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAruaAbilitySystemFunctionLibrary::GetOverlayWidgetController(
	const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD;
	const FWidgetControllerParams* WidgetControllerParams = GetHUDAndParams(WorldContextObject, AuraHUD); 
	if (WidgetControllerParams == nullptr || AuraHUD == nullptr) return nullptr;
	return AuraHUD->GetOverlayWidgetController(*WidgetControllerParams);
}

UAttributeMenuWidgetController* UAruaAbilitySystemFunctionLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD;
	const FWidgetControllerParams* WidgetControllerParams = GetHUDAndParams(WorldContextObject, AuraHUD);
	if (WidgetControllerParams == nullptr || AuraHUD == nullptr) return nullptr;
	return AuraHUD->GetAttributeMenuWidgetController(*WidgetControllerParams);
}

void UAruaAbilitySystemFunctionLibrary::InitializeDefaultAttributes(
	const UObject* WorldContextObject,
	ECharacterClass CharacterClass,
	float Level,
	UAbilitySystemComponent *Asc)
{
	const AActor *AvatarActor = Asc->GetAvatarActor();
	
	UCharacterClassInfo *ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo == nullptr) return;
	
	const FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetDefaultClassInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = Asc->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = Asc->MakeOutgoingSpec(
		ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = Asc->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = Asc->MakeOutgoingSpec(
		ClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = Asc->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = Asc->MakeOutgoingSpec(
		ClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	Asc->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAruaAbilitySystemFunctionLibrary::GiveStartupAbilities(const UObject* WorldContextObject,
	UAbilitySystemComponent* Asc)
{
	const UCharacterClassInfo *ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (ClassInfo == nullptr) return;
	
	for (const TSubclassOf<UGameplayAbility> AbilityClass : ClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		Asc->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAruaAbilitySystemFunctionLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase *AuraGameMode = Cast<AAuraGameModeBase>(
		UGameplayStatics::GetGameMode(WorldContextObject));
	return AuraGameMode == nullptr ? nullptr : AuraGameMode->CharacterClassInfo;
}

bool UAruaAbilitySystemFunctionLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext *AuraEffectContext = static_cast<const FAuraGameplayEffectContext *>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->IsBlockedHit() : false;
}

bool UAruaAbilitySystemFunctionLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	const FAuraGameplayEffectContext *AuraEffectContext = static_cast<const FAuraGameplayEffectContext *>(EffectContextHandle.Get());
	return AuraEffectContext ? AuraEffectContext->IsCriticalHit() : false;
}

FWidgetControllerParams* UAruaAbilitySystemFunctionLibrary::GetHUDAndParams(
	const UObject* WorldContextObject,
	AAuraHUD*& AuraHUD)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
	if (PC == nullptr)
	{
		return nullptr;
	}

	AuraHUD = Cast<AAuraHUD>(PC->GetHUD());
	if (AuraHUD == nullptr)
	{
		return nullptr;
	}

	AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
	if (PS == nullptr)
	{
		return nullptr;
	}

	UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
	UAttributeSet* AttrSet = PS->GetAttributeSet();
	return new FWidgetControllerParams(PC, PS, Asc, AttrSet);
}
