// Copyright Silver Desert Software


#include "AbilitySystem/AruaAbilitySystemFunctionLibrary.h"

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
