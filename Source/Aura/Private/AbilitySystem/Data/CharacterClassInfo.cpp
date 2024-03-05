// Copyright Silver Desert Software


#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "GameplayEffect.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetDefaultClassInfo(ECharacterClass CharacterClass)
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
