// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PIckUp/WarriorPickBase.h"

#include "Components/SphereComponent.h"

AWarriorPickBase::AWarriorPickBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PickUpCollisionSphere=CreateDefaultSubobject<USphereComponent>(TEXT("PickUpCollisionSphere"));
	SetRootComponent(PickUpCollisionSphere);
	PickUpCollisionSphere->InitSphereRadius(50.f);
	PickUpCollisionSphere->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnPickUpCollisionSphereBeginOverlap);
}

void AWarriorPickBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

