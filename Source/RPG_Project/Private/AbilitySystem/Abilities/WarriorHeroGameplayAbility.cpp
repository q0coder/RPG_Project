// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

#include "Characters/WarriorHeroCharacter.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Controler/WarriorHeroControler.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		CachedWarriorHeroCharacter=Cast<AWarriorHeroCharacter>(GetAvatarActorFromActorInfo());
	}
	return CachedWarriorHeroCharacter.IsValid()? CachedWarriorHeroCharacter.Get():nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActionInfo()
{
	if (!CachedWarriorHeroController.IsValid())
	{
		CachedWarriorHeroController=Cast<AWarriorHeroController>(GetAvatarActorFromActorInfo());
	}
	return CachedWarriorHeroController.IsValid()? CachedWarriorHeroController.Get():nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}
