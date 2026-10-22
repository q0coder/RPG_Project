// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorBlueprintFunctionLibrary.h"
#include "Interface/PawnCombatInterface.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/PawnCombatInterface.h"

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

UPawnCombatComponent* UWarriorBlueprintFunctionLibrary::NativeGetPawnCombatComponent(AActor* InActor)
{
	check(InActor);

	if (IPawnCombatInterface* PawnCombatInterface=Cast<IPawnCombatInterface>(InActor) )
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return  nullptr;
}

UPawnCombatComponent* UWarriorBlueprintFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent=NativeGetPawnCombatComponent(InActor);

	OutValidType=CombatComponent?EWarriorValidType::Valid:EWarriorValidType::Invalid;

	return CombatComponent;
}

bool UWarriorBlueprintFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
 {
	check(QueryPawn&& TargetPawn);

	const IGenericTeamAgentInterface* QueryTeamAgent=Cast<const IGenericTeamAgentInterface>(QueryPawn->GetController());

	const IGenericTeamAgentInterface* TargetTeamAgent=Cast<const IGenericTeamAgentInterface>(TargetPawn->GetController());

	if (QueryTeamAgent&&TargetTeamAgent)
	{
		return QueryTeamAgent->GetGenericTeamId()!=TargetTeamAgent->GetGenericTeamId();
	}

	return false;

 }

float UWarriorBlueprintFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
	return InScalableFloat.GetValueAtLevel(InLevel);
}
