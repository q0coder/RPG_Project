// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_PickUpStone.h"


#include "Characters/WarriorHeroCharacter.h"
#include "Components/UI/HeroUIComponent.h"
#include "Items/PIckUp/WarriorStoneBase.h"
#include "Kismet/KismetSystemLibrary.h"

void UHeroGameplayAbility_PickUpStone::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                       const FGameplayAbilityActorInfo* ActorInfo,
                                                       const FGameplayAbilityActivationInfo ActivationInfo,
                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetHeroUIComponentFromActorInfo()->OnStoneInteract.Broadcast(true);
}

void UHeroGameplayAbility_PickUpStone::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                  const FGameplayAbilityActorInfo* ActorInfo,
                                                  const FGameplayAbilityActivationInfo ActivationInfo,
                                                  bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	GetHeroUIComponentFromActorInfo()->OnStoneInteract.Broadcast(false);

}

void UHeroGameplayAbility_PickUpStone::ConsumeStones()
{

	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
		return;;
	}
	for (AWarriorStoneBase* StoneBase:CollectedStones)
	{
		if (StoneBase)
		{
			StoneBase->Consume(GetWarriorAbilitySystemComponentFromActorInfo(),GetAbilityLevel());
		}
	}

}

void UHeroGameplayAbility_PickUpStone::CollectStone()
{
	CollectedStones.Empty();
	TArray<FHitResult> OutHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + -GetHeroCharacterFromActorInfo()->GetActorUpVector() *
		BoxTraceDistance,
		BoxTraceSize / 2.f,
		(-GetHeroCharacterFromActorInfo()->GetActorUpVector()).ToOrientationRotator(),
		StoneTraceChannel,
		false,
		TArray<AActor*>(),
		bDrawDebugTrace ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		OutHits,
		true
	);

	for (const FHitResult& TraceHit:OutHits)
	{
		if (AWarriorStoneBase* StoneBase=Cast<AWarriorStoneBase>(TraceHit.GetActor()))
		{
			CollectedStones.AddUnique(StoneBase);
		}
	}

	if (CollectedStones.IsEmpty())
	{
		CancelAbility(GetCurrentAbilitySpecHandle(),GetCurrentActorInfo(),GetCurrentActivationInfo(),true);
	}
}
