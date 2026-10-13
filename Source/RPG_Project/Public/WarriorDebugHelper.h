#pragma once


namespace Debug
{
	inline  void Print(const FString& msg,const FColor& color=FColor::MakeRandomColor(),int32 Inkey=-1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Inkey,7.0,color,msg);

		}

		UE_LOG(LogTemp,Warning,TEXT("%s"), *msg);


	}

	inline void Print(const FString& FloatTile,float FloatValue,int32 Inkey=-1,const FColor& color=FColor::MakeRandomColor())
	{
		if (GEngine)
		{
			//SanitizeFloat是去除数字末尾多余的零，从而获得更简洁、易读的字符串表示
			const FString msg=FloatTile+TEXT(":")+FString::SanitizeFloat(FloatValue);
			GEngine->AddOnScreenDebugMessage(Inkey,7.0,color,msg);
			UE_LOG(LogTemp,Warning,TEXT("%s"), *msg);
		}


	}
}
