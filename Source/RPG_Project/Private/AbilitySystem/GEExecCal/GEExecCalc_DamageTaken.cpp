// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GEExecCal/GEExecCalc_DamageTaken.h"


#include "WarriorGamplayTag.h"
#include "AbilitySystem/WarriorAttributeSet.h"


//快捷的注册方式
//DECLARE_ATTRIBUTE_CAPTUREDEF 是 GAS 中用于在 ExecutionCalculation 或 ModifierMagnitudeCalculation 内部，便捷声明属性捕获定义的辅助宏
//它通常与 DEFINE_ATTRIBUTE_CAPTUREDEF 配对使
//标准的使用方式就如下
struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefencePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);



	FWarriorDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF (UWarriorAttributeSet,AttackPower,Source,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF (UWarriorAttributeSet,DefencePower,Target,false);
		DEFINE_ATTRIBUTE_CAPTUREDEF (UWarriorAttributeSet,DamageTaken,Target,false);


	}

};

static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	//Slow Way To Capture(注册操作的慢方法)
	//StaticClass返回一个指向该类的 UClass 对象的指针
	//FindFieldChecked用于根据名称在指定的 UStruct（包括 UClass）中安全地查找字段（属性、函数等），并在查找失败时触发断言（Assert）
	//GET_MEMBER_NAME_CHECKED宏用于在编译期安全地获取某个类或结构体中成员变量的名称，并返回一个 FName，如果成员变量被重命名或删除，编译会直接报错

	/*FProperty* AttackPowerProperty=FindFieldChecked<FProperty>(
		UWarriorAttributeSet::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet,AttackPower)
		);*/

	//FGameplayEffectAttributeCaptureDefinition 是 GAS 中用于精确定义“从谁身上、捕获什么属性、以及何时捕获” 的一个配置结构体
	//这里是其典型用处
	//核心成员变量包括：具体属性，捕获对象，指定捕获属性的时机。true 表示在效果应用前（快照）捕获，false 表示在效果执行时（实时）捕获。
	//EGameplayEffectAttributeCaptureSource 是一个枚举，存在两个取值，Source或Target

	/*FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
		AttackPowerProperty,
		EGameplayEffectAttributeCaptureSource::Source,
		false
	);
	RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);*/

	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefencePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);


}

//是 GAS 中 UGameplayEffectExecutionCalculation 类的核心虚函数，也是编写复杂效果计算逻辑的入口
//ExecutionParams输入参数。这是一个只读的结构体，包含了执行计算所需的所有上下文信息
//输出参数。这是一个引用，你需要在函数内部通过它来“输出”计算结果
//核心任务可以概括为“读取输入，计算，写入输出”三步
void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);
	//ExecutionParams.GetOwningSpec()获取当前正在执行的 GameplayEffectSpec
	const FGameplayEffectSpec& EffectSpec=ExecutionParams.GetOwningSpec();


	//GAS 的每个属性（Attribute）都拥有一个聚合器（Aggregator），它负责收集所有激活的 GameplayEffect 对该属性的修改。当需要读取属性的最终数值时，聚合器会进行“评估
	//FAggregatorEvaluateParameters 告诉聚合器：“只计算那些满足以下标签条件的修改器”
	FAggregatorEvaluateParameters EvaluateParameters;
	//SourceTags与TargetTags，属性的类型为FGameplayTagContainer
	//GetAggregatedTags作用为返回一个合并了 Spec 和 Actor 标签的 FGameplayTagContainer 指针
	EvaluateParameters.SourceTags=EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags=EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower=0.f;
	//AttemptCalculateCapturedAttributeMagnitude函数的作用是计算属性的基础值经过GE的修饰符之后的结果（只计算修饰符）
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef,EvaluateParameters,SourceAttackPower);
	float BaseDamage=0.f;
	int32 UsedLightComboCount=0;
	int32 UsedHeavyComboCount=0;

	for(const TPair<FGameplayTag,float>&TagMagnitude:EffectSpec.SetByCallerTagMagnitudes)
	{
		//MatchesTagExact 是 FGameplayTag 上的一个成员函数，用于判断一个标签是否与另一个标签完全相等。它执行的是严格的“全词匹配”，不会像 MatchesTag 那样将父标签也视为匹配。
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage=TagMagnitude.Value;
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightComboCount=TagMagnitude.Value;
		}
		if (TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyComboCount=TagMagnitude.Value;
		}

	}



	float TargetDefencePower=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefencePowerDef,EvaluateParameters,TargetDefencePower);

	if (UsedLightComboCount!=0)
	{
		const float DamageIncreasePercentLight=(UsedLightComboCount-1)*0.05+1.f;
		BaseDamage*=DamageIncreasePercentLight;
	}

	if (UsedHeavyComboCount!=0)
	{
		const float DamageIncreasePercentHeavy=UsedHeavyComboCount*0.15+1.f;
		BaseDamage*=DamageIncreasePercentHeavy;
	}

	const float FinalDamage=BaseDamage*SourceAttackPower/TargetDefencePower;

	if (FinalDamage>0.f)
	{
		//FGameplayModifierEvaluatedData是一个用于描述属性修改结果的核心结构体，通常用于OutExecutionOutput.AddOutputModifier()
		//核心参数有三个，被修改的属性，修改方式（如 Additive（加/减）、Multiplicitive（乘）、Division（除）、Override），修改的原始数值
		//AddOutputModifier作用是声明修改意图，并不会立即修改，等待GAS计算完成后统一修改，参数接收一个FGameplayModifierEvaluatedData结构体
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetWarriorDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamage
			)
		);
	}
}
