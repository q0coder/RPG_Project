// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorBlueprintFunctionLibrary.h"
#include "Interface/PawnCombatInterface.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamplayTag.h"
#include "Interface/PawnCombatInterface.h"
#include "Kismet/KismetMathLibrary.h"

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

FGameplayTag UWarriorBlueprintFunctionLibrary::ComputeReactDirectionTag(AActor* InAttacker, AActor* InVictim,
	float& OutAngleDifference)
{
	check(InAttacker&&InVictim);

	const FVector VictimForward=InVictim->GetActorForwardVector();
	const FVector VictimToAttackerNormalize=(InAttacker->GetActorLocation()-InVictim->GetActorLocation()).GetSafeNormal();
	const float DotResult=FVector::DotProduct(VictimForward,VictimToAttackerNormalize);
	const FVector CrossResult=FVector::CrossProduct(VictimForward,VictimToAttackerNormalize);


	OutAngleDifference=UKismetMathLibrary::DegAcos(DotResult);
	if (CrossResult.Z>0)
	{
		OutAngleDifference*=-1.f;
	}

	if (OutAngleDifference>-45.f&&OutAngleDifference<45.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Front;

	}
	else if (OutAngleDifference>45.f&&OutAngleDifference<135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Right;
	}
	else if (OutAngleDifference<-45.f&&OutAngleDifference>-135.f)
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Left;
	}
	else
	{
		return WarriorGameplayTags::Shared_Status_HitReact_Back;
	}
}

bool UWarriorBlueprintFunctionLibrary::IsBlockValid(const AActor* InAttacker,const AActor* InDefender)
{
	check(InAttacker&&InDefender);
	float DotResult=FVector::DotProduct(InAttacker->GetActorForwardVector(),InDefender->GetActorForwardVector());
	//DegAcos返回的值的范围是0~180
	float OutAngleDifference= UKismetMathLibrary::DegAcos(DotResult);

	if (OutAngleDifference<=180.f&&OutAngleDifference>=150.f)
	{
		return true;
	}
	return false;
}

bool UWarriorBlueprintFunctionLibrary::ApplyGameplayEffectSpecHandleToTarget( AActor* Instigator,
	 AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UWarriorAbilitySystemComponent* SourceASC=NativeGetWarriorAbilitySystemComponentFromActor(Instigator);
	UWarriorAbilitySystemComponent* TargetASC=NativeGetWarriorAbilitySystemComponentFromActor(TargetActor);

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle=SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data,TargetASC);

	return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}

