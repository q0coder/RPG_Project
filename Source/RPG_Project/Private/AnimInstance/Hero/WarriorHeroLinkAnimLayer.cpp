// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/Hero/WarriorHeroLinkAnimLayer.h"

#include "AnimInstance/Hero/WarriorHeroAnimInstance.h"


UWarriorHeroAnimInstance* UWarriorHeroLinkAnimLayer::GetHeroAnimInstance() const
{
	return  Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());

}
