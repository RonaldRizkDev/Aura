// Copyright Silver Desert Software


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfo(FGameplayTag AttributeTag, bool bLogNotFound) const
{
	for (const FAuraAttributeInfo Info : AttributesInformation)
	{
		if (Info.AttributeTag != AttributeTag)
		{
			continue;
		}

		return Info;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find infor for AttributeTag ]%s] on AttributeInfo [%s]"), *AttributeTag.ToString(), *GetNameSafe(this));
	}

	return FAuraAttributeInfo();
}
