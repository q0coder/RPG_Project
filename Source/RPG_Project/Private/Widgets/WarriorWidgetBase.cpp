// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WarriorWidgetBase.h"

#include "Components/UI/HeroUIComponent.h"
#include "Interface/PawnUIInterface.h"

//NativeOnInitialized是UserWidget的成员函数，由引擎在创建Widget时调用。
void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent=PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}

}

void UWarriorWidgetBase::InitEnemyCreateWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface=Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent=PawnUIInterface->GetEnemyUIComponent();

		checkf(EnemyUIComponent,TEXT("Failed to Extract an EnemyUIComponent from %s"),*OwningEnemyActor->GetActorNameOrLabel());
			BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);

	}
}
