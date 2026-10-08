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
}
