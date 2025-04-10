// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemTypes.h"
#include "BaseCharacter.h"
#include "ItemBase.generated.h"

UCLASS()
class IFPROJECT_API AItemBase : public AActor
{
    GENERATED_BODY()

public:
    AItemBase();

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    class USphereComponent* CollisionComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    class UStaticMeshComponent* MeshComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    EItemType ItemType;

    // 아이템 효과 값 (체력 회복 및 궁극기 게이지 증가)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    float HealthValue = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    float UltimateGaugeValue = 1.0f;

    // 리스폰 타임 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    float RespawnTime = 10.0f;

    // 리스폰 타이머
    FTimerHandle RespawnHandle;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // 아이템 효과 적용 함수

    UFUNCTION()
    virtual bool ApplyItemEffect(class ABaseCharacter* TargetCharacter);

    // 아이템 재생성 함수 (아이템 숨김 해제)
    UFUNCTION()
    void RespawnItem();

    // 아이템 외형 변경용 함수
    UPROPERTY(EditAnywhere, Category = "Item Properties")
    UMaterialInterface* HP_Material;

    UPROPERTY(EditAnywhere, Category = "Item Properties")
    UMaterialInterface* Ultimate_Material;

};
