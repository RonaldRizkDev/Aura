// Copyright Silver Desert Software


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionByTag(const FGameplayTag& InputTag,
	bool bLogNotFound) const
{
	for (const FAuraInputAction& Input : AbilityInputActions)
	{
		if (Input.InputAction && Input.InputTag.MatchesTag(InputTag) == false)
		{
			continue;
		}

		return Input.InputAction;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot find AbilityInputAction for InputTag [%s], on InputConfig [%s}"), *InputTag.ToString(), *GetNameSafe(this))
	}

	return nullptr;
}
