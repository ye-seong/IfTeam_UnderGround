#include "BaseCharacter.h"
#include "EngineUtils.h"
#include "Gun.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BarrierActor.h"
#include "BubbleVisualShieldActor.h"
#include "DrawDebugHelpers.h"
#include "MissileProjectile.h"
#include "TimerManager.h"
#include "IfProjectGameState.h"
#include "IfProjectPlayerController.h"
#include "AIController.h"
#include "AICharacter.h"



ABaseCharacter::ABaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentHP = MaxHP;
    bReplicates = true;
    bAlwaysRelevant = true;
    GetCharacterMovement()->SetIsReplicated(true);
}

void ABaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    GetCharacterMovement()->MaxWalkSpeed = 1200.f;

    if (TeamID == -1)
    {
        if (IsPlayerControlled())
            TeamID = 0;
        else
            TeamID = 1; // ���ð�
    }

    // ���� ���� ��ġ ����
    ReviveLocation = GetActorLocation();

    //CurrentHP = MaxHP;
    FireRate = 0.5f;
    LastFireTime = -FireRate;

    if (GunClass)
    {
        Gun = GetWorld()->SpawnActor<AGun>(GunClass);
        if (Gun)
        {
            Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GripPoint"));
            Gun->SetOwner(this);
        }
    }
}

void ABaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseCharacter::ReceiveDamage(float DamageAmount)
{
    if (bIsDead) return;
    
    //if (bUnderTankerUltimate)
    //{
    //    DamageAmount *= 0.5f;  // 50% ????
    //}
    if (CurrentShieldVisual && IsValid(CurrentShieldVisual) || bUnderTankerUltimate)
    {
        DamageAmount *= 0.5f;  // 50% ����
        bUnderTankerUltimate = false;
    }
    CurrentHP -= DamageAmount;

    //GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Black, FString::Printf(TEXT("%s CurrentHP :%d"), *GetName(), CurrentHP));

    if (CurrentHP <= 0.f)
    {
        LastHitByChar = this;
        if (LastHitByChar)
        {
            AIfProjectPlayerController* PC = Cast<AIfProjectPlayerController>(LastHitByChar->GetController());
            if (PC)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("!!!!!!!!!!!!!!!!!!!!!!!Get pc!!!!!!!!!!!!!!!!!!!!!!"));
            }
        }
        CurrentHP = 0.f;
        UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());

        
        HandleDeath();
    }
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (bIsDead) return 0.f; // ? ��� �� ����
    FString CauserName = DamageCauser ? DamageCauser->GetName() : TEXT("Unknown");

    ReceiveDamage(DamageAmount);  // ���� ���� Ŀ���� �Լ� ȣ��
    return DamageAmount;
}

void ABaseCharacter::OnFire()
{
    if (bIsDead || !Gun) return;
    FString Msg = FString::Printf(TEXT("bIsReloading: %s"), Gun->bIsReloading ? TEXT("true") : TEXT("false"));
    GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Msg);
    // ���⼭ ���Ǹ� üũ: ź�� �ְ�, ���ε� �ƴϸ�
    if (Gun->CurrBulletNum > 0 && !Gun->bIsReloading)
    {
        Super::OnFire();   // ���� Projectile ����
        Gun->Fire();       // ź�� ����, ���ε� üũ�� ���ο���
    }
    else if (Gun->CurrBulletNum <= 0 && !Gun->bIsReloading)
    {
        // ź�� ���� + ���� ���ε� �� �ɷ� ������ ���ε� ����
        Gun->Fire();  // ���ο��� bIsReloading = true + Ÿ�̸� ����
    }
    // �� �� (�̹� ���ε� ��)�� �ƹ��͵� �� ��
}

void ABaseCharacter::Heal(float HealAmount)
{
    if (bIsDead) return;

    CurrentHP += HealAmount;
    if (CurrentHP > MaxHP)
        CurrentHP = MaxHP;

    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, FString::Printf(TEXT("Healed: +%.0f / HP: %.0f"), HealAmount, CurrentHP));
}

void ABaseCharacter::Revive()
{
    if (!bIsDead) return;


    bIsDead = false;

    CurrentHP = MaxHP;

    // ��ġ ����
    SetActorLocation(SpawnPosition); // �ʿ� �� ���� ����

    // �ٽ� ���̰�
    SetActorHiddenInGame(false);

    // �浹/�Է�/�̵� ����
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // �޽�
    if (GetMesh()) GetMesh()->SetVisibility(true, true);
    if (Gun) {
        Gun->SetActorHiddenInGame(false);
        Gun->SetActorEnableCollision(true);
        Gun->SetActorTickEnabled(true);
    }

    if (AAICharacter* AIChar = Cast<AAICharacter>(this))
    {
        AIChar->CurrentTagIndex = 0;  // ��Ȱ �� �ε��� �ʱ�ȭ
    }

    if (!IsPlayerControlled())
    {
        AController* MyController = GetController();
        if (!MyController)
        {
            SpawnDefaultController();
        }
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s has been revived."), *GetName()));
}


void ABaseCharacter::HandleDeath()
{
    if (bIsDead) return;

    bIsDead = true;  // ? ���⼭�� true�� ����

    // ��� Ÿ�̸� ����
    GetWorldTimerManager().ClearAllTimersForObject(this);

    GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Red,
        FString::Printf(TEXT("[DEATH] %s has died."), *GetName()));

    ReviveLocation = GetActorLocation();

    // �Է� ���� (�÷��̾�)
    DisableInput(nullptr);

    // �̵� ����
    GetCharacterMovement()->DisableMovement();

    // �浹 ����
    SetActorEnableCollision(false);

    // ���� ��ü �����
    SetActorHiddenInGame(true);

    // �޽� �����
    if (GetMesh())
    {
        GetMesh()->SetVisibility(false, true);
    }

    // �ѱ� ��Ȱ��ȭ
    if (Gun)
    {
        Gun->SetActorHiddenInGame(true);
        Gun->SetActorEnableCollision(false);
        Gun->SetActorTickEnabled(false);
    }

    // Tick ����
    SetActorTickEnabled(false);

    // AI ��������
    if (!IsPlayerControlled())
    {
        AController* AIController = GetController();
        if (AIController)
        {
            AIController->StopMovement();
            AIController->UnPossess();
        }
    }
    else {
        // �÷��̾� �Է� �ٽ� ����
        APlayerController* PC = Cast<APlayerController>(GetController());
        if (PC)
        {
            EnableInput(PC);
        }
    }
    if (CurrentShieldVisual && IsValid(CurrentShieldVisual))
    {
        CurrentShieldVisual->Destroy();
        CurrentShieldVisual = nullptr;
    }
    

    // ?? 10�� �� �ڵ� ��Ȱ Ÿ�̸� ����
    GetWorldTimerManager().SetTimer(ReviveTimerHandle, this, &ABaseCharacter::Revive, 10.f, false);
}

void ABaseCharacter::ReceiveHealing(float HealAmount)
{
    CurrentHP += HealAmount;
    if (CurrentHP > MaxHP)
    {
        CurrentHP = MaxHP;
    }

    //GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,
    //    FString::Printf(TEXT("%s healed: %.1f / %.1f"), *GetName(), CurrentHP, MaxHP));
}

void ABaseCharacter::AddUltiNum(int32 Amount)
{
    // �ñر� �������� �����ϵ�, 0 ~ MaxUltimateCount ������ ���� �ʵ��� Clamp
    this->UltiNum = FMath::Clamp(this->UltiNum + Amount, 0, this->MaxUltiNum);
}

/* ���� �Լ� */

// ��Ŀ
void ABaseCharacter::PerformBarrierSkill(float& CurCooldown, float CoolTime, float Distance, TSubclassOf<ABarrierActor> BarrierClassInput)
{
    if (bIsDead) return;
    // �̹� ��Ÿ�� ���̰ų� Ŭ���� ������ �� �Ǿ� ������ ����
    if (CurCooldown > 0.f || !BarrierClassInput)
    {
        /* GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Barrier Skill is on cooldown!"));*/
        return;
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
        FString::Printf(TEXT("[Tanker Skill1] %s"), *GetName()));

    /*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TankerCharacter's BarrierSkill FUNCTION start~"));*/
    // ĳ���� �տ� �溮 ������ ��ġ ���
    // ĳ���� �������� BarrierDistance��ŭ ������ ��ġ�� �溮 ����
    FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * Distance + FVector(0, 0, 50.f); // Z������ ���� 50cm;
    FRotator SpawnRotation = GetActorRotation();

    // �溮 ����
    FActorSpawnParameters SpawnParams;
    ABarrierActor* Barrier = GetWorld()->SpawnActor<ABarrierActor>(BarrierClassInput, SpawnLocation, SpawnRotation, SpawnParams);
    if (Barrier)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Barrier is true~"));
        // �溮���� �� ID ���� (ĳ������ PlayerState���� ������)
        Barrier->SetTeamID(TeamID);
    }

    // ��� �Ұ��� �����ϰ� ��Ÿ�� Ÿ�̸� ����
    CurCooldown = CoolTime;
}

void ABaseCharacter::PerformShockStrikeSkill(float &CurrCooldown, float CoolTime, float ImpactRadius, float ImpactDamage)
{
    if (bIsDead) return;
    // ��Ÿ��
    if (CurrCooldown > 0.f) return;

    CurrCooldown = CoolTime;

    UE_LOG(LogTemp, Warning, TEXT("Is Shock Strike!!"));

    // ĳ���� ���� ��ġ�� �������� ����� �߽� ����
    FVector Origin = GetActorLocation();

    //// ����� Ž�� ��� ����� �迭
    TArray<FHitResult> HitResults;

    // ����� ����(��ü ����)
    FCollisionShape Sphere = FCollisionShape::MakeSphere(ImpactRadius);

    // �ݰ� ������ Pawn ä�η� �浹 �˻� (��, ĳ���͵鸸 Ž��)
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        Origin,
        Origin + FVector(0.1f),       // ���� ���¿��� Sweep
        FQuat::Identity,              // ȸ�� ����
        ECC_Pawn,                     // Pawn ä�θ� Ž�� (ĳ���͵�)
        Sphere
    );

    TSet<AActor*> ProcessedActors;

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            ABaseCharacter* Target = Cast<ABaseCharacter>(Hit.GetActor());

            if (Target
                && Target != this
                && !Target->bIsDead
                && Target->TeamID != this->TeamID
                && !ProcessedActors.Contains(Target))
            {
                Target->ReceiveDamage(ImpactDamage);
                ProcessedActors.Add(Target);

                /* GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow,
                     FString::Printf(TEXT("[%s] Shock skill use! Target: %s / Damage: %.1f"),
                         *GetName(), *Target->GetName(), ImpactDamage));*/
            }
        }
    }

    DrawDebugSphere(GetWorld(), Origin, ImpactRadius, 16, FColor::Red, false, 2.0f);
}


void ABaseCharacter::PerformTankerUltimateSkill(float Duration)
{
    if (bIsDead) return;
    if (UltiNum < 3)
    {
        /*GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red,
            FString::Printf(TEXT("current ultiNum : %d"), UltiNum));*/
        return;
    }

    if (UltiNum == 3) {

        // 1. ���� ���� ���� (��ID Ȯ���ؼ� �ڽ� ���� �͸� ����)
        for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
        {
            if (It->TeamID == this->TeamID && !It->bIsDead)
            {
                // ��????? ?????? ??? ???? ????
                if (It->CurrentShieldVisual && IsValid(It->CurrentShieldVisual))
                {
                    It->CurrentShieldVisual->Destroy();
                    It->CurrentShieldVisual = nullptr;
                }
            }
        
        }

        // 2. ���� ���� �� ����
        for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
        {
            if (It->TeamID == this->TeamID)
            {
                It->SetBUnderTankerUltimate(true);

                if (BubbleShieldVisualClass)
                {
                    FVector SpawnLocation = It->GetActorLocation() + FVector(0, 0, 100.f);
                    FRotator SpawnRotation = FRotator::ZeroRotator;

                    ABubbleVisualShieldActor* ShieldVisual = GetWorld()->SpawnActor<ABubbleVisualShieldActor>(
                        BubbleShieldVisualClass, SpawnLocation, SpawnRotation);

                    if (ShieldVisual)
                    {
                        ShieldVisual->AttachToActor(*It, FAttachmentTransformRules::KeepWorldTransform);
                        It->CurrentShieldVisual = ShieldVisual;  // ���� ����
                        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
                            FString::Printf(TEXT("[TankerUltimateSkill] : %s"), *GetName()));
                    }
                }
            }
        }

        // 3. ���� �ð� �� ���� ���� �� ���� ����
        FTimerHandle Handle;
        GetWorld()->GetTimerManager().SetTimer(Handle, [this]()
            {
                for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
                {
                    if (It->TeamID == this->TeamID)
                    {
                        It->SetBUnderTankerUltimate(false);

                        // ����� ���� ����
                        if (It->CurrentShieldVisual && IsValid(It->CurrentShieldVisual))
                        {
                            It->CurrentShieldVisual->Destroy();
                            It->CurrentShieldVisual = nullptr;
                        }
                    }
                }

            }, Duration, false);

        UltiNum = 0;
    }

}


// ����
void ABaseCharacter::PerformDashSkill(float& CurrCooldown, float CoolTime)
{
    // ��Ÿ��
    if (CurrCooldown > 0.f || !GetCharacterMovement()->IsMovingOnGround()) return;

    CurrCooldown = CoolTime;

    UE_LOG(LogTemp, Warning, TEXT("Dealer Dash Skill"));

    // ���� ĳ���Ͱ� �ٶ󺸴� ����
    FVector ForwardDirection = GetActorForwardVector();

    // ��� �� (���� ���� ����)
    // 
    //float DashStrength = 8000.0f;

    // LaunchCharacter: ĳ���͸� Ư�� �������� ƨ�ܳ����� �� (XY ���⸸ ���� Z�� 0)
    LaunchCharacter(ForwardDirection * 27000.f, true, false);

}

void ABaseCharacter::PerformStunSkill(float& CurrCooldown, float CoolTime)
{
    if (bIsDead || CurrCooldown > 0.f) return;
    CurrCooldown = CoolTime;

    float Radius = 1000.f;             // Ž�� �ݰ�
    float ConeAngle = 180.f;           // ��ä�� ���� (�� ����)
    FVector MyLocation = GetActorLocation();
    FVector MyForward = GetActorForwardVector();

    DrawDebugCone(GetWorld(), MyLocation, MyForward, Radius,
        FMath::DegreesToRadians(ConeAngle * 0.5f),
        FMath::DegreesToRadians(ConeAngle * 0.5f),
        12, FColor::Yellow, false, 1.0f);

    TArray<FOverlapResult> Overlaps;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    // ���� ���� Ž��
    bool bFound = GetWorld()->OverlapMultiByChannel(
        Overlaps,
        MyLocation,
        FQuat::Identity,
        ECC_Pawn,
        FCollisionShape::MakeSphere(Radius),
        Params
    );

    bool bHitSomething = false;

    if (bFound)
    {
        for (auto& Result : Overlaps)
        {
            ABaseCharacter* Target = Cast<ABaseCharacter>(Result.GetActor());
            if (!Target || Target == this || Target->bIsDead) continue;

            FVector ToTarget = (Target->GetActorLocation() - MyLocation).GetSafeNormal();
            float Dot = FVector::DotProduct(MyForward, ToTarget);
            float CosThreshold = FMath::Cos(FMath::DegreesToRadians(ConeAngle * 0.5f));

            if (Dot >= CosThreshold)  // ��ä�� �ȿ� ����
            {
                Target->DisableCharacter(2.0f);
                bHitSomething = true;

                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
                    FString::Printf(TEXT("[ConeStun Skill] Target: %s"), *Target->GetName()));
                //break;  // �� �� ������ �Ÿ� break, ���� ���̸� �����ص� ��
            }
        }
    }

    if (!bHitSomething)
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
            TEXT("[ConeStun Skill] No Target in Cone"));
    }

}

void ABaseCharacter::DisableCharacter(float Duration)
{
    DisableInput(nullptr);  // ��� �Է� ����

    GetWorldTimerManager().SetTimer(
        BuffTimerHandle,
        [this]()
        {
            EnableInput(nullptr);  // 2�� �� �ٽ� �Է� ���
        },
        Duration,
        false
    );

    UE_LOG(LogTemp, Warning, TEXT("Disable character control: %s"), *GetName());
}

void ABaseCharacter::PerformDealerUltimateSkill(FTimerHandle& UltimateDelayHandle)
{
    if (bIsDead) return;
    if (UltiNum < 3) return;

    if (UltiNum == 3) {

        // 1. �ϴ÷� ����
        LaunchCharacter(FVector(0, 0, 2000), true, true);

        // 2. 1�� �� �ڵ� ���� �� �̻��� �߻�
        GetWorld()->GetTimerManager().SetTimerForNextTick([this, &UltimateDelayHandle]()
            {
                GetWorld()->GetTimerManager().SetTimer(
                    UltimateDelayHandle,
                    [this]()
                    {
                        // �ֺ� �� Ž��
                        FVector MyLocation = GetActorLocation();
                        float MinDist = FLT_MAX;
                        ABaseCharacter* ClosestTarget = nullptr;

                        for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
                        {
                            ABaseCharacter* Target = *It;
                            if (Target != this && !Target->IsPendingKill())
                            {
                                float Dist = FVector::Dist2D(Target->GetActorLocation(), MyLocation);  // Z ����
                                if (Dist < 1500.f && Dist < MinDist)
                                {
                                    ClosestTarget = Target;
                                    MinDist = Dist;
                                }
                            }
                        }

                        if (ClosestTarget && Gun && Gun->GetProjectileClass())
                        {
                            FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);
                            FRotator SpawnRotation = (ClosestTarget->GetActorLocation() - SpawnLocation).Rotation();

                            FActorSpawnParameters Params;
                            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

                            LaunchMissileAtTarget(ClosestTarget);

                        }
                        else
                        {

                            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
                                TEXT("[Dealer UltimateSkill] Target : No Target �� Faild Missile Launch"));

                        }

                    },
                    1.0f, false  // 1�� �� ����
                );
            });

    }
    UltiNum = 0;
    }


void ABaseCharacter::LaunchMissileAtTarget(AActor* Target)
{
    if (bIsDead) return;
    if (!Gun || !Target) return;

    // �Ѿ� Ŭ���� ��������
    TSubclassOf<AMissileProjectile> TempProjectileClass = Gun->GetProjectileClass();
    if (!TempProjectileClass) return;

    // �߻� ��ġ �� ���� ����
    FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 50);  // �Ӹ� �� �ణ
    FRotator SpawnRotation = (Target->GetActorLocation() - SpawnLocation).Rotation();

    // ���� �Ķ����
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // �Ѿ�(�̻���) ����
    AMissileProjectile* Projectile = GetWorld()->SpawnActor<AMissileProjectile>(
        DealerUltimateMissileClass, SpawnLocation, SpawnRotation, Params);

    // ���� Ÿ�� ���� + ������ ����
    if (Projectile)
    {
        Projectile->SetOwner(this);
        Projectile->SetDamage(70.0f);              // �ñر� ������
        Projectile->SetHomingTarget(Target);       // ���� ��� ����

        UE_LOG(LogTemp, Warning, TEXT("Missile Launch Completed! : %s"), *Target->GetName());
    }
}

// ����
ABaseCharacter* ABaseCharacter::FindClosestAlly()
{
    ABaseCharacter* ClosestAlly = nullptr;
    float MinDistance = 1000.f;

    for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
    {
        ABaseCharacter* Other = *It;

        if (!IsValid(Other) || Other == this || Other->bIsDead) continue;  // ??? ???, ????? ???

        if (Other->TeamID != this->TeamID) continue;     // ?? ????? ????

        float Distance = FVector::Dist(this->GetActorLocation(), Other->GetActorLocation());

        if (Distance < MinDistance)
        {
            MinDistance = Distance;
            ClosestAlly = Other;
        }
    }

    return ClosestAlly;
}

void ABaseCharacter::PerformTeleportHealSkill(float& CurrCooldown, float HealerCoolDown)
{
    if (bIsDead) return;
    if (CurrCooldown > 0.f) return;

    ABaseCharacter* FoundAlly = FindClosestAlly();

    if (FoundAlly)
    {
        TargetAlly = FoundAlly;
        TargetLocation = FoundAlly->GetActorLocation();
        bIsMovingToTarget = true;
        CurrCooldown = HealerCoolDown;
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("[HealSkill1] NO TEAM MEMBER"));
        CurrCooldown = 0;
    }
}

void ABaseCharacter::PerformTeleportHealSkillTick(ABaseCharacter* Character, float DeltaTime)
{

    if (!bIsMovingToTarget) return;

    Character->SetActorHiddenInGame(true);
    Character->SetActorEnableCollision(false);
    Character->SetActorTickEnabled(false); 

    Character->SetActorLocation(TargetLocation);

    Character->SetActorHiddenInGame(false);
    Character->SetActorEnableCollision(true);
    Character->SetActorTickEnabled(true);

    if (TargetAlly)
    {
        TargetAlly->ReceiveHealing(30.f);
        GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("INSTANT HEAL COMPLETE!"));
    }

    bIsMovingToTarget = false;
}

ABaseCharacter* ABaseCharacter::FindFurthestAlly()
{
	ABaseCharacter* FurthestAlly = nullptr;

	float MaxDistance = 0.0f;

	// ���忡 �����ϴ� ��� ABaseCharacter ã��
	TArray<AActor*> AllCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseCharacter::StaticClass(), AllCharacters);

	for (AActor* Actor : AllCharacters)
	{
		ABaseCharacter* OtherChar = Cast<ABaseCharacter>(Actor);
		if (!OtherChar || OtherChar == this || OtherChar->bIsDead)
			continue;

		// ���� ������ Ȯ��
		if (OtherChar->TeamID != this->TeamID)
			continue;

		// �Ÿ� ���
		float Distance = FVector::Dist(this->GetActorLocation(), OtherChar->GetActorLocation());
		if (Distance > MaxDistance)
		{
			MaxDistance = Distance;
			FurthestAlly = OtherChar;
		}
	}

	return FurthestAlly; // ���� ��� nullptr ��ȯ
}
void ABaseCharacter::PerformHealShieldSkill(float& CurrCooldown, float Duration, FTimerHandle HealSpeedTimerHandle, float DeltaTime)
{
    if (bIsDead) return;
    if (CurrCooldown > 0.f) return;

    ABaseCharacter* FoundAlly = FindFurthestAlly();
    if (!FoundAlly) return;

    if (FoundAlly->CurrentShieldVisual && IsValid(FoundAlly->CurrentShieldVisual))
    {
        FoundAlly->CurrentShieldVisual->Destroy();
        FoundAlly->CurrentShieldVisual = nullptr;
    }

    ABubbleVisualShieldActor* ShieldVisual = nullptr;
    if (BubbleShieldVisualClass)
    {
        FVector SpawnLocation = FoundAlly->GetActorLocation() + FVector(0, 0, 100.f);
        FRotator SpawnRotation = FRotator::ZeroRotator;

        ShieldVisual = GetWorld()->SpawnActor<ABubbleVisualShieldActor>(BubbleShieldVisualClass, SpawnLocation, SpawnRotation);
        if (ShieldVisual)
        {
            ShieldVisual->AttachToActor(FoundAlly, FAttachmentTransformRules::KeepWorldTransform);
            GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("[HealSkill2]ShieldVisual COMPLETE!"));
            FoundAlly->CurrentShieldVisual = ShieldVisual;


        }
    }
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow,
        FString::Printf(TEXT("Before Speed: %.1f"), FoundAlly->GetCharacterMovement()->MaxWalkSpeed));
    float OriginalSpeed = FoundAlly->GetCharacterMovement()->MaxWalkSpeed;
    FoundAlly->GetCharacterMovement()->MaxWalkSpeed *= 1.8f;
    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
        FString::Printf(TEXT("New Speed: %.1f (Old: %.1f)"), FoundAlly->GetCharacterMovement()->MaxWalkSpeed, OriginalSpeed));
    GetWorldTimerManager().SetTimer(HealSpeedTimerHandle, [FoundAlly, OriginalSpeed]()
        {
            if (FoundAlly)
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("[HealSkill2]SPEED UP AND DOWN!"));
                FoundAlly->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
                //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Healer Speed Reset"));
            }

            if (FoundAlly->CurrentShieldVisual && IsValid(FoundAlly->CurrentShieldVisual))
            {
                FoundAlly->CurrentShieldVisual->Destroy();
                FoundAlly->CurrentShieldVisual = nullptr;
            }

        }, Duration, false);

    CurrCooldown = 10;
}


void ABaseCharacter::ResetHealSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed /= 2.5;
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Blue, TEXT("Healer Speed Reset"));
}

void ABaseCharacter::PerformHealerUltimateSkill()
{
	if (UltiNum < 3) return;
	 
	int32 RevivedCount = 0;

	for (TActorIterator<ABaseCharacter> It(GetWorld()); It; ++It)
	{
		if (*It == this || It->TeamID != TeamID || !It->bIsDead) continue;

		It->UltimateRevive();
		break;
	}

	UltiNum = 0;
}

void ABaseCharacter::UltimateRevive()
{
    if (!bIsDead) return;

    // ?? �ڵ� ��Ȱ Ÿ�̸� ���� (������ ���� ��Ȱ��Ű�� ��� �ߺ� ����)
    GetWorldTimerManager().ClearTimer(ReviveTimerHandle);


    bIsDead = false;

    CurrentHP = MaxHP;

    // ��ġ ����
    SetActorLocation(ReviveLocation); // �ʿ� �� ���� ����

    // �ٽ� ���̰�
    SetActorHiddenInGame(false);

    // �浹/�Է�/�̵� ����
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    GetCharacterMovement()->SetMovementMode(MOVE_Walking);

    // �޽�
    if (GetMesh()) GetMesh()->SetVisibility(true, true);
    if (Gun) {
        Gun->SetActorHiddenInGame(false);
        Gun->SetActorEnableCollision(true);
        Gun->SetActorTickEnabled(true);
    }


    if (!IsPlayerControlled())
    {
        AController* MyController = GetController();
        if (!MyController)
        {
            SpawnDefaultController();
        }
    }

    GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Cyan, FString::Printf(TEXT("%s has been revived. (UltimateRevive)"), *GetName()));
}