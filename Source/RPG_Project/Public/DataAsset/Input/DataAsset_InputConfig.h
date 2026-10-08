// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "DataAsset_InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;

USTRUCT(Blueprintable)
struct FWarriorInputConfig
{
	GENERATED_BODY()

public :
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories = "InputTag"))
	FGameplayTag InputTag;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputAction* InputAction;

	bool IsValid() const
	{
		return InputTag.IsValid() && InputAction;
	}

};
UCLASS()
class RPG_PROJECT_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()
public :
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputConfig> NativeInputActions;

	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InputTag)  const ;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(TitleProperty="InputTag"))
	TArray<FWarriorInputConfig> AbilityInputActions;

};
