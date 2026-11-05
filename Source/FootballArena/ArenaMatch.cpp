#include "ArenaMatch.h"

#include "Camera/CameraActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace Arena
{
    constexpr float HalfLength = 6000.f;
    constexpr float HalfWidth = 3800.f;
    constexpr float GoalHalfWidth = 550.f;
    constexpr float RunnerSpeed = 640.f;
    constexpr float BallRadius = 22.f;
}

AArenaMatch::AArenaMatch()
{
    PrimaryActorTick.bCanEverTick = true;
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

UStaticMeshComponent* AArenaMatch::MakePart(UStaticMesh* Mesh,
    const FVector& Location, const FVector& Scale,
    const FLinearColor& Color, const FRotator& Rotation)
{
    UStaticMeshComponent* Part = NewObject<UStaticMeshComponent>(this);
    AddInstanceComponent(Part);
    Part->SetStaticMesh(Mesh);
    Part->SetupAttachment(RootComponent);
    Part->RegisterComponent();
    Part->SetWorldLocation(Location);
    Part->SetWorldRotation(Rotation);
    Part->SetWorldScale3D(Scale);
    Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(
        nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
    if (BaseMaterial)
    {
        UMaterialInstanceDynamic* Tint = UMaterialInstanceDynamic::Create(BaseMaterial, Part);
        Tint->SetVectorParameterValue(TEXT("Color"), Color);
        Part->SetMaterial(0, Tint);
    }
    return Part;
}

void AArenaMatch::MakePitch()
{
    const FLinearColor Grass(0.045f, 0.27f, 0.08f);
    const FLinearColor AlternateGrass(0.05f, 0.34f, 0.09f);
    const FLinearColor White(0.86f, 0.95f, 0.87f);
    MakePart(Cube, FVector(0.f, 0.f, -28.f), FVector(123.f, 79.f, 0.5f), Grass);

    for (int32 Stripe = 0; Stripe < 12; ++Stripe)
    {
        const float X = -5500.f + Stripe * 1000.f;
        if (Stripe % 2 == 0)
            MakePart(Cube, FVector(X, 0.f, 0.4f), FVector(10.f, 76.f, 0.025f), AlternateGrass);
    }

    // Touch lines, midfield line, penalty areas.
    for (float X : {-Arena::HalfLength, Arena::HalfLength})
        MakePart(Cube, FVector(X, 0.f, 6.f), FVector(0.09f, 76.f, 0.1f), White);
    for (float Y : {-Arena::HalfWidth, Arena::HalfWidth})
        MakePart(Cube, FVector(0.f, Y, 6.f), FVector(120.f, 0.09f, 0.1f), White);
    MakePart(Cube, FVector(0.f, 0.f, 6.f), FVector(0.09f, 76.f, 0.1f), White);

    for (int32 Side : {-1, 1})
    {
        const float GoalX = Side * Arena::HalfLength;
        const float BoxX = Side * (Arena::HalfLength - 1400.f);
        MakePart(Cube, FVector(BoxX, 0.f, 6.f), FVector(.09f, 29.f, .1f), White);
        for (int32 Sign : {-1, 1})
        {
            MakePart(Cube, FVector((GoalX + BoxX) * .5f, Sign * 1450.f, 6.f),
                FVector(14.f, .09f, .1f), White);
            MakePart(Cube, FVector(GoalX, Sign * Arena::GoalHalfWidth, 145.f),
                FVector(.16f, .13f, 2.9f), White);
        }
        MakePart(Cube, FVector(GoalX, 0.f, 291.f),
            FVector(.16f, 11.2f, .12f), White);
    }

    for (int32 I = 0; I < 48; ++I)
    {
        const float Angle = 2.f * PI * I / 48.f;
        MakePart(Cube,
            FVector(900.f * FMath::Cos(Angle), 900.f * FMath::Sin(Angle), 7.f),
            FVector(.9f, .07f, .1f), White, FRotator(0.f, FMath::RadiansToDegrees(Angle) + 90.f, 0.f));
    }
}

void AArenaMatch::MakeRunner(const FVector& Where, int32 Team, bool bCaptain)
{
    FArenaRunner R;
    R.Position = Where;
    R.StartingPosition = Where;
    R.Team = Team;
    R.Forward = Team == 0 ? FVector::ForwardVector : -FVector::ForwardVector;

    const FLinearColor Shirt = Team == 0
        ? FLinearColor(0.015f, 0.22f, 1.f)
        : FLinearColor(0.95f, 0.025f, 0.045f);
    const FLinearColor Skin(0.86f, 0.50f, 0.26f);
    const FLinearColor Shorts(0.06f, 0.075f, 0.15f);
    R.Body = MakePart(Cube, Where + FVector(0.f, 0.f, 120.f),
        FVector(.46f, .30f, .75f), Shirt);
    R.Head = MakePart(Sphere, Where + FVector(0.f, 0.f, 193.f),
        FVector(.31f), Skin);
    R.LeftLeg = MakePart(Cube, Where + FVector(0.f, -18.f, 43.f),
        FVector(.19f, .20f, .78f), Shorts);
    R.RightLeg = MakePart(Cube, Where + FVector(0.f, 18.f, 43.f),
        FVector(.19f, .20f, .78f), Shorts);
    R.LeftArm = MakePart(Cube, Where + FVector(0.f, -44.f, 114.f),
        FVector(.20f, .18f, .55f), Skin);
    R.RightArm = MakePart(Cube, Where + FVector(0.f, 44.f, 114.f),
        FVector(.20f, .18f, .55f), Skin);

    if (bCaptain)
    {
        MakePart(Sphere, Where + FVector(0.f, 0.f, 257.f),
            FVector(.13f), FLinearColor(1.f, .8f, .03f));
    }
    Runners.Add(R);
}

void AArenaMatch::BeginPlay()
{
    Super::BeginPlay();
    Cube = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
    Sphere = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (!Cube || !Sphere) return;

    UWorld* World = GetWorld();
    if (ADirectionalLight* Sun = World->SpawnActor<ADirectionalLight>())
    {
        Sun->SetActorRotation(FRotator(-65.f, -20.f, 0.f));
        Sun->GetLightComponent()->SetIntensity(8.f);
    }
    if (ASkyLight* Sky = World->SpawnActor<ASkyLight>())
        Sky->GetLightComponent()->SetIntensity(1.f);

    MakePitch();
    MakeRunner(FVector(-1000.f, 0.f, 0.f), 0, true);
    MakeRunner(FVector(-2500.f, -1800.f, 0.f), 0, false);
    MakeRunner(FVector(-2600.f, 1800.f, 0.f), 0, false);
    MakeRunner(FVector(-4200.f, 0.f, 0.f), 0, false);
    MakeRunner(FVector(1200.f, 0.f, 0.f), 1, false);
    MakeRunner(FVector(2900.f, -1550.f, 0.f), 1, false);
    MakeRunner(FVector(2900.f, 1550.f, 0.f), 1, false);
    MakeRunner(FVector(4100.f, 0.f, 0.f), 1, false);

    const FLinearColor KeeperGreen(.9f, .72f, .04f);
    for (int32 I = 0; I < 2; ++I)
    {
        const float X = I == 0 ? -5800.f : 5800.f;
        KeeperBody[I] = MakePart(Cube, FVector(X, 0.f, 110.f),
            FVector(.55f, .42f, 1.05f), KeeperGreen);
        KeeperHead[I] = MakePart(Sphere, FVector(X, 0.f, 208.f),
            FVector(.32f), FLinearColor(.86f, .50f, .26f));
    }

    BallMesh = MakePart(Sphere, BallPosition, FVector(.22f), FLinearColor::White);
    MatchCamera = World->SpawnActor<ACameraActor>();
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        PC->SetViewTarget(MatchCamera);
    UpdateCamera(1.f);
    ResetKickoff(0);
}

void AArenaMatch::ResetKickoff(int32 StartingTeam)
{
    for (FArenaRunner& R : Runners)
    {
        R.Position = R.StartingPosition;
        R.Forward = R.Team == 0 ? FVector::ForwardVector : -FVector::ForwardVector;
    }
    BallOwner = StartingTeam == 0 ? 0 : 4;
    BallPosition = Runners[BallOwner].Position + FVector(120.f, 0.f, Arena::BallRadius);
    BallVelocity = FVector::ZeroVector;
    ShotCooldown = 0.f;
    for (int32 I = 0; I < 2; ++I) { KeeperY[I] = 0.f; DiveTimer[I] = 0.f; }
}

void AArenaMatch::AnimateRunner(FArenaRunner& R, float Dt, bool bMoving)
{
    if (bMoving) R.RunPhase += Dt * 11.f;
    const float Swing = bMoving ? FMath::Sin(R.RunPhase) * 19.f : 0.f;
    const FRotator Facing = R.Forward.Rotation();
    const FVector Right = FVector::CrossProduct(FVector::UpVector, R.Forward);
    const FVector P = R.Position;
    R.Body->SetWorldLocation(P + FVector(0.f, 0.f, 122.f));
    R.Body->SetWorldRotation(Facing);
    R.Head->SetWorldLocation(P + FVector(0.f, 0.f, 194.f));

    R.LeftLeg->SetWorldLocation(P - Right * 17.f + R.Forward * Swing + FVector(0.f, 0.f, 45.f));
    R.RightLeg->SetWorldLocation(P + Right * 17.f - R.Forward * Swing + FVector(0.f, 0.f, 45.f));
    R.LeftArm->SetWorldLocation(P - Right * 41.f - R.Forward * Swing + FVector(0.f, 0.f, 117.f));
    R.RightArm->SetWorldLocation(P + Right * 41.f + R.Forward * Swing + FVector(0.f, 0.f, 117.f));
    R.LeftLeg->SetWorldRotation(Facing);
    R.RightLeg->SetWorldRotation(Facing);
    R.LeftArm->SetWorldRotation(Facing);
    R.RightArm->SetWorldRotation(Facing);
}

void AArenaMatch::ControlPlayer(float Dt)
{
    APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
    if (!PC || Runners.Num() < 1) return;

    float MX = 0.f, MY = 0.f;
    if (PC->IsInputKeyDown(EKeys::W) || PC->IsInputKeyDown(EKeys::Z)
        || PC->IsInputKeyDown(EKeys::Up)) MX += 1.f;
    if (PC->IsInputKeyDown(EKeys::S) || PC->IsInputKeyDown(EKeys::Down)) MX -= 1.f;
    if (PC->IsInputKeyDown(EKeys::D) || PC->IsInputKeyDown(EKeys::Right)) MY += 1.f;
    if (PC->IsInputKeyDown(EKeys::A) || PC->IsInputKeyDown(EKeys::Q)
        || PC->IsInputKeyDown(EKeys::Left)) MY -= 1.f;

    bool bShoot = PC->IsInputKeyDown(EKeys::SpaceBar)
        || PC->IsInputKeyDown(EKeys::Gamepad_FaceButton_Bottom);
    int32 ViewW = 0, ViewH = 0;
    PC->GetViewportSize(ViewW, ViewH);
    for (int32 TouchIndex = 0; TouchIndex < 3; ++TouchIndex)
    {
        float TX = 0.f, TY = 0.f;
        bool bDown = false;
        PC->GetInputTouchState(static_cast<ETouchIndex::Type>(TouchIndex), TX, TY, bDown);
        if (!bDown || ViewW <= 0 || ViewH <= 0) continue;
        if (TX < ViewW * .55f)
        {
            MX += FMath::Clamp((ViewH * .76f - TY) / (ViewH * .15f), -1.f, 1.f);
            MY += FMath::Clamp((TX - ViewW * .24f) / (ViewW * .16f), -1.f, 1.f);
        }
        else bShoot = true;
    }

    FArenaRunner& Player = Runners[0];
    FVector Direction(MX, MY, 0.f);
    Direction = Direction.GetClampedToMaxSize(1.f);
    if (!Direction.IsNearlyZero())
    {
        Player.Forward = Direction.GetSafeNormal();
        Player.Position += Direction * Arena::RunnerSpeed * Dt;
        Player.Position.X = FMath::Clamp(Player.Position.X, -5650.f, 5650.f);
        Player.Position.Y = FMath::Clamp(Player.Position.Y, -3550.f, 3550.f);
    }
    AnimateRunner(Player, Dt, !Direction.IsNearlyZero());

    if (bShoot && !bLastShootPressed && ShotCooldown <= 0.f)
        Kick(0);
    bLastShootPressed = bShoot;
}

void AArenaMatch::MoveAI(float Dt)
{
    for (int32 I = 1; I < Runners.Num(); ++I)
    {
        FArenaRunner& R = Runners[I];
        const bool bHasBall = BallOwner == I;
        const bool bChasing = BallOwner == -1 || (BallOwner >= 0 && Runners[BallOwner].Team != R.Team);
        FVector Target = R.StartingPosition;
        if (bHasBall)
        {
            Target = FVector(R.Team == 0 ? 5900.f : -5900.f, 0.f, 0.f);
            const float DistanceToGoal = FMath::Abs(Target.X - R.Position.X);
            if (DistanceToGoal < 2700.f && ShotCooldown <= 0.f) Kick(I);
        }
        else if (bChasing)
        {
            // Closest player of each team attacks the ball, the others keep their positions.
            float Best = TNumericLimits<float>::Max();
            int32 Closest = -1;
            for (int32 J = 0; J < Runners.Num(); ++J)
            {
                if (Runners[J].Team != R.Team) continue;
                const float Dist = FVector::DistSquared2D(Runners[J].Position, BallPosition);
                if (Dist < Best) { Best = Dist; Closest = J; }
            }
            if (Closest == I) Target = BallPosition;
        }

        FVector Move = Target - R.Position;
        Move.Z = 0.f;
        const bool bRunning = Move.SizeSquared() > 3000.f;
        if (bRunning)
        {
            R.Forward = Move.GetSafeNormal();
            const float Speed = R.Team == 1 ? 530.f : 490.f;
            R.Position += R.Forward * FMath::Min(Speed * Dt, Move.Size());
            R.Position.X = FMath::Clamp(R.Position.X, -5650.f, 5650.f);
            R.Position.Y = FMath::Clamp(R.Position.Y, -3550.f, 3550.f);
        }
        AnimateRunner(R, Dt, bRunning);
    }
}

void AArenaMatch::Kick(int32 ByRunner)
{
    if (BallOwner != ByRunner || !Runners.IsValidIndex(ByRunner)) return;
    const FArenaRunner& Shooter = Runners[ByRunner];
    const float GoalX = Shooter.Team == 0 ? Arena::HalfLength : -Arena::HalfLength;
    const float Distance = FMath::Abs(GoalX - Shooter.Position.X);
    const float Flight = FMath::Max(.35f, Distance / 4200.f);
    const float TargetY = FMath::RandBool() ? 380.f : -380.f;
    const FVector Target(GoalX + (Shooter.Team == 0 ? 110.f : -110.f), TargetY, 270.f);
    BallPosition = Shooter.Position + Shooter.Forward * 105.f + FVector(0.f, 0.f, Arena::BallRadius);
    BallVelocity = (Target - BallPosition) / Flight;
    BallVelocity.Z += 0.5f * 980.f * Flight;
    BallOwner = -1;
    ShotCooldown = .8f;
}

void AArenaMatch::UpdateKeepers(float Dt)
{
    for (int32 I = 0; I < 2; ++I)
    {
        const float X = I == 0 ? -5750.f : 5750.f;
        const float TargetY = FMath::Clamp(BallPosition.Y, -460.f, 460.f);
        KeeperY[I] = FMath::FInterpConstantTo(KeeperY[I], TargetY, Dt, 950.f);
        const bool bApproaching = (I == 0 ? BallVelocity.X < -100.f : BallVelocity.X > 100.f)
            && FMath::Abs(BallPosition.X - X) < 1350.f && BallOwner == -1;
        if (bApproaching && DiveTimer[I] <= 0.f)
        {
            DiveTimer[I] = .7f;
            DiveDirection[I] = BallPosition.Y > KeeperY[I] ? 1.f : -1.f;
        }
        if (DiveTimer[I] > 0.f)
            DiveTimer[I] = FMath::Max(0.f, DiveTimer[I] - Dt);

        const float Dive = DiveTimer[I] > 0.f ? FMath::Sin(DiveTimer[I] / .7f * PI) : 0.f;
        const FVector KeeperLocation(X, KeeperY[I] + Dive * DiveDirection[I] * 90.f,
            100.f - Dive * 55.f);
        KeeperBody[I]->SetWorldLocation(KeeperLocation);
        KeeperBody[I]->SetWorldRotation(FRotator(0.f, 0.f, Dive * DiveDirection[I] * 60.f));
        KeeperHead[I]->SetWorldLocation(KeeperLocation + FVector(0.f, 0.f, 100.f - Dive * 30.f));

        if (BallOwner == -1 && FMath::Abs(BallPosition.X - X) < 125.f
            && FMath::Abs(BallPosition.Y - KeeperLocation.Y) < 130.f
            && BallPosition.Z < 300.f)
        {
            BallVelocity.X *= -.55f;
            BallVelocity.Y += (BallPosition.Y - KeeperLocation.Y) * 3.f;
            BallVelocity.Z = FMath::Max(BallVelocity.Z, 220.f);
        }
    }
}

void AArenaMatch::UpdateBall(float Dt)
{
    if (BallOwner >= 0 && Runners.IsValidIndex(BallOwner))
    {
        const FArenaRunner& Owner = Runners[BallOwner];
        BallPosition = Owner.Position + Owner.Forward * 113.f + FVector(0.f, 0.f, Arena::BallRadius);
        BallVelocity = FVector::ZeroVector;
    }
    else
    {
        BallPosition += BallVelocity * Dt;
        BallVelocity.Z -= 980.f * Dt;
        if (BallPosition.Z < Arena::BallRadius)
        {
            BallPosition.Z = Arena::BallRadius;
            BallVelocity.Z = FMath::Abs(BallVelocity.Z) > 70.f ? -BallVelocity.Z * .28f : 0.f;
            BallVelocity.X *= .987f;
            BallVelocity.Y *= .987f;
        }
        if (FMath::Abs(BallPosition.Y) > Arena::HalfWidth)
        {
            BallPosition.Y = FMath::Clamp(BallPosition.Y, -Arena::HalfWidth, Arena::HalfWidth);
            BallVelocity.Y *= -.55f;
        }

        // Valid goals: ball must pass between the posts and below the crossbar.
        if (FMath::Abs(BallPosition.X) > Arena::HalfLength + 40.f)
        {
            if (FMath::Abs(BallPosition.Y) < Arena::GoalHalfWidth && BallPosition.Z < 295.f)
            {
                if (BallPosition.X > 0.f) ++BlueScore;
                else ++RedScore;
                ResetKickoff(BallPosition.X > 0.f ? 1 : 0);
            }
            else
            {
                BallPosition.X = FMath::Clamp(BallPosition.X, -Arena::HalfLength, Arena::HalfLength);
                BallVelocity.X *= -.45f;
            }
        }

        if (BallOwner == -1 && BallVelocity.Size2D() < 1250.f && BallPosition.Z < 145.f)
        {
            for (int32 I = 0; I < Runners.Num(); ++I)
            {
                if (FVector::DistSquared2D(BallPosition, Runners[I].Position) < 125.f * 125.f)
                {
                    BallOwner = I;
                    break;
                }
            }
        }
    }
    if (BallMesh)
    {
        BallMesh->SetWorldLocation(BallPosition);
        BallMesh->AddWorldRotation(FRotator(BallVelocity.Size() * Dt * .013f, 0.f, 0.f));
    }
}

void AArenaMatch::UpdateCamera(float Dt)
{
    if (!MatchCamera || Runners.Num() == 0) return;
    const FVector P = Runners[0].Position;
    const FVector CameraTarget = P + FVector(-1750.f, -1100.f, 1400.f);
    MatchCamera->SetActorLocation(FMath::VInterpTo(
        MatchCamera->GetActorLocation(), CameraTarget, Dt, 3.5f));
    const FVector Look = P + FVector(1100.f, 0.f, 90.f);
    MatchCamera->SetActorRotation((Look - MatchCamera->GetActorLocation()).Rotation());
}

void AArenaMatch::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (Runners.Num() < 8) return;
    const float Dt = FMath::Min(DeltaTime, .05f);
    Elapsed += Dt;
    ShotCooldown = FMath::Max(0.f, ShotCooldown - Dt);
    ControlPlayer(Dt);
    MoveAI(Dt);
    UpdateBall(Dt);
    UpdateKeepers(Dt);
    UpdateCamera(Dt);
}
