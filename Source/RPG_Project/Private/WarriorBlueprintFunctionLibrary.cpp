// Fill out your copyright notice in the Description page of Project Settings.

#include "WarriorBlueprintFunctionLibrary.h"
#include "Interface/PawnCombatInterface.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "FWarriorCountDownAction.h"
#include "GenericTeamAgentInterface.h"
#include "WarriorGameInstance.h"
#include "WarriorDebugHelper.h"
#include "WarriorGamplayTag.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "SaveGame/WarriorSaveGame.h"

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

void UWarriorBlueprintFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime,
	float UpdateInterval, float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,
	UPARAM(DisplayName="Output") EWarriorCountDownActionOutput& CountDownOutput, FLatentActionInfo LatentInfo)
{

	UWorld* World=nullptr;
	//官方推荐必须始终检查GEngine的有效性
	if (GEngine)
	{
		World=GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return;
	}

	FLatentActionManager& LatentActionManager=World->GetLatentActionManager();
	FWarriorCountDownAction* FoundAction=LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget,LatentInfo.UUID);

	if (CountDownInput==EWarriorCountDownActionInput::Start)
	{
		if (!FoundAction)
		{
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget,LatentInfo.UUID,new FWarriorCountDownAction(TotalTime,UpdateInterval,OutRemainingTime,CountDownOutput,LatentInfo));
		}
	}

	if (CountDownInput==EWarriorCountDownActionInput::Cancel)
	{
		if (FoundAction)
		{
			FoundAction->CancelAction();
		}
	}

}

UWarriorGameInstance* UWarriorBlueprintFunctionLibrary::GetWarriorGameInStance(const UObject* WorldContext)
{
	if (GEngine)
	{
		if (UWorld* World=GEngine->GetWorldFromContextObject(WorldContext,EGetWorldErrorMode::LogAndReturnNull))
		{
			return World->GetGameInstance<UWarriorGameInstance>();
		}
	}
	return nullptr;
}

void UWarriorBlueprintFunctionLibrary::ToggleInputMode(const UObject* WorldContext, EWarriorInputMode InputMode)
{
	APlayerController* PlayerController=nullptr;
	if (GEngine)
	{
		if (UWorld* World=GEngine->GetWorldFromContextObject(WorldContext,EGetWorldErrorMode::LogAndReturnNull))
		{
			 PlayerController=World->GetFirstPlayerController();
		}
	}

	if (PlayerController==nullptr)
	{
		return;
	}
	FInputModeGameOnly InputModeGameOnly;
	FInputModeUIOnly InputModeUIOnly;
	switch (InputMode)
	{
	case EWarriorInputMode::GameOnly:
		PlayerController->SetInputMode(InputModeGameOnly);
		PlayerController->bShowMouseCursor=false;
		break;
	case EWarriorInputMode::UIOnly:
		PlayerController->SetInputMode(InputModeUIOnly);
		PlayerController->bShowMouseCursor=true;
		break;
	}

}

void UWarriorBlueprintFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave)
{
	USaveGame* SaveGame=UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());

	if (UWarriorSaveGame* WarriorSaveGame=Cast<UWarriorSaveGame>(SaveGame))
	{
		WarriorSaveGame->SaveCurrentGameDifficulty=InDifficultyToSave;
		//这里的保存方式的同步的保存，只适用于少量的游戏数据的保存，对于大量游戏数据的保存，为了防止游戏线程卡顿，我们需要使用异步保存的方式，加载游戏数据相同
		UGameplayStatics::SaveGameToSlot(WarriorSaveGame,WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(),0);
	}
}

bool UWarriorBlueprintFunctionLibrary::LoadCurrentGameDifficulty(EWarriorGameDifficulty& OutDifficultyToLoad)
{
	if (UGameplayStatics::DoesSaveGameExist(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(),0))
	{
		USaveGame* SaveGame=UGameplayStatics::LoadGameFromSlot(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(),0);
		if (UWarriorSaveGame* WarriorSaveGame=Cast<UWarriorSaveGame>(SaveGame))
		{
			OutDifficultyToLoad=WarriorSaveGame->SaveCurrentGameDifficulty;
			return true;
		}
	}
	return false;

}

