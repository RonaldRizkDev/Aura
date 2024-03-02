// Copyright Silver Desert Software


#include "Interfaces/CombatInterface.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.
int32 ICombatInterface::GetPLayerLevel() const
{
	return 0;
}

FVector ICombatInterface::GetCombatSocketLocation()
{
	return FVector();
}
