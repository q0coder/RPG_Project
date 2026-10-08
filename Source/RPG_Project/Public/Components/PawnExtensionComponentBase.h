// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPG_PROJECT_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()
protected:
	template<class T>
	T* GetOwningPawn() const
	{
		//C++11引入的编译期断言，判断应该条件是否成立，这里是判断T类型是否为APawn的子类
		//不成立编译器报错，成立没有造成任何开销
		//TPointerIsConvertibleFromTo是UE4提供的一个模板类，用于判断两个类型之间的继承关系
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value, "'T' template Parameter to GetPawn must be derived from APawn");
		//CastChecked是一个带有check断言的模版函数，发布版会忽略check
		return CastChecked<T>(GetOwner());
	}

	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	template<class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T' template Parameter to GetController must be derived from AController");
		// APawn::GetController<T>()内部也是cast，获取到Pawn的Controller并转换为T类型
		return GetOwningPawn<APawn>()->GetController<T>();
	}


};
