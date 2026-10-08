// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorTypes/WArriorStructTypes.h"

#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

bool FWarriorHeroAbilitySet::IsValid() const
{
	return InputTag.IsValid() && AbilityToGrant;
}
