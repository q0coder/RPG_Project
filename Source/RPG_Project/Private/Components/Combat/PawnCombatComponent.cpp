// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"

#include "Items/Weapon/WarriorWeaponBase.h"

void UPawnCombatComponent::RegisterSpawnWeapon(FGameplayTag InWeaponTagToRegister,
                                               AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	//检测TMap中是否已经存在了需要注册的对象
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister),TEXT("A named %s has already been added to the CharacterCarriedMap"),*InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister,InWeaponToRegister);
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		if (AWarriorWeaponBase* const* FoundWeapon=CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return  *FoundWeapon;
		}
	}
	return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}
	return  GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

