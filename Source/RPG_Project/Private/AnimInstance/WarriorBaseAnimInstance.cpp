// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WarriorBaseAnimInstance.h"

#include "WarriorBlueprintFunctionLibrary.h"

bool UWarriorBaseAnimInstance::DoesOwnerHaveTag(FGameplayTag TagToCheck)
{
	if (APawn* OwningPawn=TryGetPawnOwner())
	{
		return UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(OwningPawn,TagToCheck);
	}
	return false;
}
