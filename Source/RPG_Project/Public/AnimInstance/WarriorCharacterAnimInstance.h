// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstance/WarriorBaseAnimInstance.h"
#include "WarriorCharacterAnimInstance.generated.h"

class UCharacterMovementComponent;
class AWarriorBaseCharacter;
/**
 *
 */
UCLASS()
class RPG_PROJECT_API UWarriorCharacterAnimInstance : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()
public :
	virtual void NativeInitializeAnimation() override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY()
	AWarriorBaseCharacter* OwningCharacter;

	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation|LocomotionData")
	float GroundSpeed;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Animation|LocomotionData")
	bool bHasAcceleration;
};
