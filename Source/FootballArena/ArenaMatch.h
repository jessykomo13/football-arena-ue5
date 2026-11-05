#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArenaMatch.generated.h"

class ACameraActor;
class UStaticMesh;
class UStaticMeshComponent;

struct FArenaRunner
{
    FVector Position = FVector::ZeroVector;
    FVector Forward = FVector::ForwardVector;
    FVector StartingPosition = FVector::ZeroVector;
    int32 Team = 0;
    float RunPhase = 0.0f;
    UStaticMeshComponent* Body = nullptr;
    UStaticMeshComponent* Head = nullptr;
    UStaticMeshComponent* LeftLeg = nullptr;
    UStaticMeshComponent* RightLeg = nullptr;
    UStaticMeshComponent* LeftArm = nullptr;
    UStaticMeshComponent* RightArm = nullptr;
};

UCLASS()
class FOOTBALLARENA_API AArenaMatch : public AActor
{
    GENERATED_BODY()

public:
    AArenaMatch();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    int32 GetBlueScore() const { return BlueScore; }
    int32 GetRedScore() const { return RedScore; }

private:
    UStaticMesh* Cube = nullptr;
    UStaticMesh* Sphere = nullptr;
    UStaticMeshComponent* BallMesh = nullptr;
    UStaticMeshComponent* KeeperBody[2] = {nullptr, nullptr};
    UStaticMeshComponent* KeeperHead[2] = {nullptr, nullptr};
    TArray<FArenaRunner> Runners;
    ACameraActor* MatchCamera = nullptr;
    FVector BallPosition = FVector(100.f, 0.f, 32.f);
    FVector BallVelocity = FVector::ZeroVector;
    int32 BallOwner = 0;
    int32 BlueScore = 0;
    int32 RedScore = 0;
    float Elapsed = 0.f;
    float ShotCooldown = 0.f;
    float KeeperY[2] = {0.f, 0.f};
    float DiveTimer[2] = {0.f, 0.f};
    float DiveDirection[2] = {0.f, 0.f};
    bool bLastShootPressed = false;

    UStaticMeshComponent* MakePart(UStaticMesh* Mesh, const FVector& Location,
        const FVector& Scale, const FLinearColor& Color, const FRotator& Rotation = FRotator::ZeroRotator);
    void MakePitch();
    void MakeRunner(const FVector& Where, int32 Team, bool bCaptain);
    void AnimateRunner(FArenaRunner& Runner, float DeltaTime, bool bMoving);
    void ControlPlayer(float DeltaTime);
    void MoveAI(float DeltaTime);
    void UpdateKeepers(float DeltaTime);
    void UpdateBall(float DeltaTime);
    void UpdateCamera(float DeltaTime);
    void Kick(int32 ByRunner);
    void ResetKickoff(int32 StartingTeam);
};
