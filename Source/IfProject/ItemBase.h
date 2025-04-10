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

    // ������ ȿ�� �� (ü�� ȸ�� �� �ñر� ������ ����)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    float HealthValue = 50.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    float UltimateGaugeValue = 1.0f;

    // ������ Ÿ�� (��)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Properties")
    float RespawnTime = 10.0f;

    // ������ Ÿ�̸�
    FTimerHandle RespawnHandle;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    // ������ ȿ�� ���� �Լ�

    UFUNCTION()
    virtual bool ApplyItemEffect(class ABaseCharacter* TargetCharacter);

    // ������ ����� �Լ� (������ ���� ����)
    UFUNCTION()
    void RespawnItem();

    // ������ ���� ����� �Լ�
    UPROPERTY(EditAnywhere, Category = "Item Properties")
    UMaterialInterface* HP_Material;

    UPROPERTY(EditAnywhere, Category = "Item Properties")
    UMaterialInterface* Ultimate_Material;

};
