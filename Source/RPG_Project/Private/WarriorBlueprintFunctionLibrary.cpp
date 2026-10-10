// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorBlueprintFunctionLibrary.h"

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

UWarriorAbilitySystemComponent* UWarriorBlueprintFunctionLibrary::
NativeGetWarriorAbilitySystemComponentFromActor(AActor* InActor)
{
	check(InActor);
	return	Cast<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));


}

void UWarriorBlueprintFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC=NativeGetWarriorAbilitySystemComponentFromActor(InActor);
	if (!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorBlueprintFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC=NativeGetWarriorAbilitySystemComponentFromActor(InActor);
	if (ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorBlueprintFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC=NativeGetWarriorAbilitySystemComponentFromActor(InActor);
	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorBlueprintFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType=NativeDoesActorHaveTag(InActor,TagToCheck)?EWarriorConfirmType::Yes:EWarriorConfirmType::No;
}
