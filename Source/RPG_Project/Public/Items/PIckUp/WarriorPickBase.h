// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorPickBase.generated.h"

class USphereComponent;

UCLASS()
class RPG_PROJECT_API AWarriorPickBase : public AActor
{
	GENERATED_BODY()

public:
	AWarriorPickBase();
protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="PickUp|Collision")
	 USphereComponent* PickUpCollisionSphere;

	UFUNCTION()
	virtual void OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
