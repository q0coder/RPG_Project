// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PIckUp/WarriorStoneBase.h"

#include "WarriorGamplayTag.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"

void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* AbilitySystemComponent, int32 ApplyLevel)
{
	ensure(StoneConsumeGameplayEffect);

	//Get获取的是对应类的UClass*，GetDefaultObject的模版函数能够获取对应类的实际类型
	UGameplayEffect* EffectCDO=StoneConsumeGameplayEffect->GetDefaultObject<UGameplayEffect>();

	AbilitySystemComponent->ApplyGameplayEffectToSelf(
	EffectCDO,
	ApplyLevel,
	AbilitySystemComponent->MakeEffectContext()
	);

	BP_OnStoneConsumed();
}

void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                            AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                            const FHitResult& SweepResult)
{
	if (AWarriorHeroCharacter* OverLapHeroCharacter=Cast<AWarriorHeroCharacter>(OtherActor) )
	{
		OverLapHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGameplayTags::Player_Ability_PickUp_Stones);
	}


}
