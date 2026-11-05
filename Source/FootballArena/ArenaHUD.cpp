#include "ArenaHUD.h"
#include "ArenaMatch.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

void AArenaHUD::DrawHUD()
{
    Super::DrawHUD();
    if (!Canvas || !GEngine) return;

    const AArenaMatch* Match = Cast<AArenaMatch>(
        UGameplayStatics::GetActorOfClass(GetWorld(), AArenaMatch::StaticClass()));

    const float W = Canvas->SizeX;
    const float H = Canvas->SizeY;
    const float Scale = FMath::Max(0.9f, FMath::Min(W / 1280.0f, 1.8f));
    DrawRect(FLinearColor(0.01f, 0.04f, 0.07f, 0.82f), W * 0.33f, 16.f, W * 0.34f, 64.f);
    const FString Score = Match
        ? FString::Printf(TEXT("BLEUS  %d : %d  ROUGES"), Match->GetBlueScore(), Match->GetRedScore())
        : TEXT("FOOTBALL ARENA");
    DrawText(Score, FLinearColor::White, W * 0.36f, 26.f, GEngine->GetMediumFont(), Scale);

    DrawText(TEXT("ZQSD / WASD / fleches : bouger     ESPACE : tirer"),
        FLinearColor::White, 18.f, H - 45.f, GEngine->GetSmallFont(), Scale);

    if (W < 1150.f)
    {
        DrawRect(FLinearColor(0.03f, 0.15f, 0.33f, 0.32f), 25.f, H - 225.f, 155.f, 155.f);
        DrawText(TEXT("BOUGER"), FLinearColor::White, 52.f, H - 150.f, GEngine->GetSmallFont(), Scale);
        DrawRect(FLinearColor(0.4f, 0.03f, 0.04f, 0.50f), W - 175.f, H - 208.f, 155.f, 155.f);
        DrawText(TEXT("TIRER"), FLinearColor::White, W - 130.f, H - 135.f, GEngine->GetSmallFont(), Scale);
    }
}
