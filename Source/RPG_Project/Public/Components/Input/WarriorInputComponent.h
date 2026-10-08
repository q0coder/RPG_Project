// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"

#include "DataAsset/Input/DataAsset_InputConfig.h"
#include "WarriorInputComponent.generated.h"

/** */
UCLASS()
class RPG_PROJECT_API UWarriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject,typename  CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InputConfig,const FGameplayTag& InPutTag,ETriggerEvent TriggerEvent,UserObject* ContextObject,CallbackFunc Func);

	template<class UserObject,typename  CallbackFunc>
	void BindAbilityInputAction(const UDataAsset_InputConfig* InputConfig,UserObject* ContextObject,CallbackFunc InputPressedFunc,CallbackFunc InputReleasedFunc);
};

template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InputConfig,
	const FGameplayTag& InPutTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InputConfig, TEXT("InputConfig is nullptr!"));

	if (UInputAction* FundAction=InputConfig->FindNativeInputActionByTag(InPutTag))
	{

		BindAction(FundAction, TriggerEvent, ContextObject, Func);
	}
}

template <class UserObject, typename CallbackFunc>
inline  void UWarriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InputConfig,
	UserObject* ContextObject, CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
	checkf(InputConfig, TEXT("InputConfig is nullptr!"));

	for (const FWarriorInputConfig& AbilityInputActionConfig: InputConfig->AbilityInputActions)
	{
		if (!AbilityInputActionConfig.IsValid()) continue;

			BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressedFunc,AbilityInputActionConfig.InputTag);
			BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputReleasedFunc,AbilityInputActionConfig.InputTag);

	}
}


