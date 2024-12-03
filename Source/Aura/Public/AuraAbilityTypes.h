#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit;	}

	void SetBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }

	virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }
	virtual bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess) override;
	virtual FAuraGameplayEffectContext* Duplicate() const override;
	
protected:
	UPROPERTY()
	bool bIsBlockedHit = false;
	
	UPROPERTY()
	bool bIsCriticalHit = false;
	
private:
	static constexpr int32 BlockHitBitNumber = 7;
	static constexpr int32 CriticalHitBitNumber = 8;
};

template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext>
	: public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
