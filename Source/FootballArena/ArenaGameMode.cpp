#include "ArenaGameMode.h"
#include "ArenaHUD.h"
#include "ArenaMatch.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"

AArenaGameMode::AArenaGameMode()
{
    PlayerControllerClass = APlayerController::StaticClass();
    DefaultPawnClass = nullptr;
    HUDClass = AArenaHUD::StaticClass();
}

void AArenaGameMode::StartPlay()
{
    Super::StartPlay();
    if (UWorld* World = GetWorld())
    {
        World->SpawnActor<AArenaMatch>(AArenaMatch::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
    }
}
