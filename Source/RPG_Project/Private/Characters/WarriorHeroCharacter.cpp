// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorHeroCharacter.h"

#include "EnhancedInputSubsystems.h"

#include "WarriorGamplayTag.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Input/WarriorInputComponent.h"
#include "DataAsset/Input/DataAsset_InputConfig.h"
#include "DataAsset/StratUpData/DataAsset_StartUpDataBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/Combat/HeroCombatComponent.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	//一般第三人称的统一设置，玩家不跟随摄像机（视角）旋转而旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll=false;
	bUseControllerRotationYaw=false;

	CameraBoom=CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength=200.f;
	//摄像机相对于弹簧臂末端的物理位置偏移。
	//摄像机向右上偏移，通过更好的视角，防止遮挡准星
	CameraBoom->SocketOffset=FVector(0.f,55.f,65.f);
	//设置弹簧臂（摄像机）是否跟随玩家的控制器（鼠标/摇杆）进行旋转。
	CameraBoom->bUsePawnControlRotation=true;;

	FollowCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom,USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation=false;

	GetCharacterMovement()->bOrientRotationToMovement=true;
	//设置玩家的最大速度，转身速度与减速度。
	GetCharacterMovement()->RotationRate=FRotator(0.f,500.f,0.f);
	GetCharacterMovement()->MaxWalkSpeed=400.f;
	GetCharacterMovement()->BrakingDecelerationWalking=2000.f;


	HeroCombatComponent=CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));


}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!CharacterStartUpData.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadData=CharacterStartUpData.LoadSynchronous())
		{
			LoadData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
}


void AWarriorHeroCharacter::SetupPlayerInputComponent( UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset,TEXT("Forget to assign a valid data asset as input config"));

	ULocalPlayer* LocalPlayer=GetController<APlayerController>()->GetLocalPlayer();

	UEnhancedInputLocalPlayerSubsystem* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);
	UWarriorInputComponent* WarriorInputComponent=CastChecked<UWarriorInputComponent>(PlayerInputComponent);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_Move,ETriggerEvent::Triggered,this,&ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset,WarriorGameplayTags::InputTag_Look,ETriggerEvent::Triggered,this,&ThisClass::Input_Look);

	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset,this,&ThisClass::Input_AbilityInputPressed,&ThisClass::Input_AbilityInputReleased);


}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector=InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.f,Controller->GetControlRotation().Yaw,0.f);
	if (MovementVector.Y!=0.f)
	{
		const FVector ForwardDirection=MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardDirection,MovementVector.Y);
	}

	if (MovementVector.X!=0.f)
	{
		const FVector RightDirection=MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(RightDirection,MovementVector.X);
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{

	const FVector2D LookAxisVector=InputActionValue.Get<FVector2D>();

	if (LookAxisVector.X!=0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y!=0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InputTag)
{
 	WarriorAbilitySystemComponent->OnAbilityInputReleased(InputTag);
}



