// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapon/WarriorWeaponBase.h"
#include "Components/BoxComponent.h"

// Sets default values
AWarriorWeaponBase::AWarriorWeaponBase()
{

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox=CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponCollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());

	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxBeginOverlap);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this,&ThisClass::OnCollisionBoxEndOverlap);


}

void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	APawn* WeaponOwningPawn=GetInstigator<APawn>();

	checkf(WeaponOwningPawn,TEXT("Forget to assign an instigator as the owning pawn for the weapon: %s "),*GetName());

	if (APawn* HitPawn=Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn!=HitPawn)
		{


			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{




	//GetInstigator获取的是对象发起者的APawn，模版参数的作用是Cast
	//在一个类的成员函数中调用另一个成员函数，这里省略了this
	//GetInstigator获取的就是this（WarriorWeaponBase）的APawn
	//Instigator一般必须主动设置，这里在蓝图中的公共武器生成节点通过SpawnActor节点就已经设定Instigator了
	APawn* WeaponOwningPawn=GetInstigator<APawn>();

	checkf(WeaponOwningPawn,TEXT("Forget to assign an instigator as the owning pawn for the weapon: %s "),*GetName());

	if (APawn* HitPawn=Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn!=HitPawn)
		{


			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);

		}
	}

}




