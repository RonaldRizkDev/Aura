// Copyright Silver Desert Software


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());
}

void AAuraEffectActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent,
									AActor* OtherActor,
									UPrimitiveComponent* OtherComp,
									int32 OtherBodyIndex,
									bool bFromSweep,
									const FHitResult& SweepResult)
{
	// Todo:  Change this to apply a gameplay effect, this is just a hack for now
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor);
	if (ASCInterface == nullptr)
	{
		return;
	}

	const UAuraAttributeSet* AuraAttributes = 
	Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));

	UAuraAttributeSet* LocalAttributes = const_cast<UAuraAttributeSet*>(AuraAttributes);
	
	LocalAttributes->SetHealth(AuraAttributes->GetHealth() + 25.f);
	Destroy();
}

void AAuraEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnBeginOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::OnEndOverlap);
}

