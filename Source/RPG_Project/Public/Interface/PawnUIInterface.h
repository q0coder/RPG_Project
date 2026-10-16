// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnUIInterface.generated.h"

class UEnemyUIComponent;
class UHeroUIComponent;
class UPawnUIComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class RPG_PROJECT_API IPawnUIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	//无论是原生的C++，还是UE5，接口的函数的访问限定都是public，因为实现接口的类与调用接口函数的类通常并不是同一个类
public:
	virtual  UPawnUIComponent* GetPawnUIComponent() const=0;
	virtual  UHeroUIComponent* GetHeroUIComponent() const;
	virtual  UEnemyUIComponent* GetEnemyUIComponent() const;


};
