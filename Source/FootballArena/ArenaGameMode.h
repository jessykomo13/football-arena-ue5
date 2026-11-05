#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ArenaGameMode.generated.h"

UCLASS()
class FOOTBALLARENA_API AArenaGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AArenaGameMode();
    virtual void StartPlay() override;
};
