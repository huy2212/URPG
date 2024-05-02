#pragma once
#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/EnemyAIInterface.h"
#include "Interfaces/HitInterface.h"
#include "Character/CharacterState.h"
#include "AI/PatrolPath.h"
#include "Engine/EngineTypes.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IBlockable.h"
#include "Enemy.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipped);

UCLASS()
class MYPROJECT_API AEnemy : public ACharacter, public IHitInterface, public IEnemyAIInterface, public IIBlockable
{
    GENERATED_BODY()

public:
    AEnemy();

    /**
     * IHitInterface
     */
    UFUNCTION(BlueprintCallable)
    virtual void GetHit(const FVector &ImpactPoint) override;

    UFUNCTION(BlueprintCallable)
    virtual void TakeDamageInterface(float DamageAmount, struct FDamageEvent const &DamageEvent,
                                     class AController *EventInstigator, AActor *DamageCauser) override;

    UFUNCTION(BlueprintCallable)
    virtual bool IsDead() override;

    UFUNCTION(BlueprintCallable)
    virtual void Die() override;

    /**
     * IEnemyAIInterface
     */
    UFUNCTION(BlueprintCallable)
    virtual APatrolPath *GetPatrolPath() override;

    UFUNCTION(BlueprintCallable)
    virtual void SetMovementType(EMovementType NewMovementType) override;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    virtual void GetAttackRange(float &Out_AttackRange, float &Out_DefenseRange) override
    {
        // Return next attack range for the enemy to move
        Out_AttackRange = EnemyAIAttackRange;
        Out_DefenseRange = DefenseRange;
    }

    UFUNCTION(BlueprintCallable)
    virtual void SetActionState(EActionState NewActionState) override;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    virtual EActionState GetEnemyActionState() override
    {
        return ActionState;
    };

    /**
     * Blockable interface
     */
    UFUNCTION(BlueprintCallable, category = "Block")
    virtual void StartBlock() override;

    UFUNCTION(BlueprintCallable, category = "Block")
    virtual void EndBlock() override;

    UFUNCTION(BlueprintCallable, category = "Block")
    virtual void OnBlockSuccess() override;

    UFUNCTION(BlueprintCallable, category = "Block")
    virtual bool IsBlocking() override
    {
        return bIsBlocking;
    };

    UFUNCTION(BlueprintCallable, category = "Block")
    virtual bool CanBlock() override
    {
        return bCanBlock;
    };

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, category = "Block")
    void PlayBlockMontage();

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    bool bIsBlocking;

    UPROPERTY(EditDefaultsOnly)
    bool bCanBlock;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *BlockMontage;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *BlockSuccessMontage;

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void Attack();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void EquipWeapon();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void UnequipWeapon();

    UPROPERTY(BlueprintCallable, BlueprintAssignable, category = "Event Dispatcher")
    FOnWeaponEquipped OnWeaponEquipped;

    UPROPERTY(BlueprintCallable, BlueprintAssignable, category = "Event Dispatcher")
    FOnWeaponUnequipped OnWeaponUnequipeed;

    UPROPERTY(BlueprintCallable, BlueprintAssignable, category = "Event Dispatcher")
    FOnAttackEnd OnAttackEnd;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent,
                             class AController *EventInstigator, AActor *DamageCauser) override;

    UPROPERTY(BlueprintReadOnly)
    EDeathPose DeathPose = EDeathPose::Alive;

    UFUNCTION(BlueprintCallable)
    void SpawnWeapon(FName SocketName);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    EActionState GetActionState()
    {
        return ActionState;
    }

    UPROPERTY(BlueprintReadOnly)
    EActionState ActionState = EActionState::Passive;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UBlueprint *BP_WeaponAsset;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UWeaponDataAsset *WeaponAsset;

    UPROPERTY(BlueprintReadOnly)
    class AWeapon *EquippedWeapon;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    class UBehaviorTree *GetBehaviorTree()
    {
        return this->BehaviorTree;
    }

    UFUNCTION(BlueprintCallable)
    void ToggleWeaponCollision(ECollisionEnabled::Type CollisionType, bool IsLeftHandWeapon);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool GetIsEquippingWeapon()
    {
        return IsEquippingWeapon;
    }

    UPROPERTY(BlueprintReadWrite)
    bool IsEquippingWeapon;

    UFUNCTION(BlueprintCallable)
    void SetNextWeaponRange(float &Out_AttackRange);

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable)
    virtual void SetWeaponBehavior(UAnimMontage *AttackMontage, FName SectionName);

    UFUNCTION(BlueprintCallable)
    virtual void SetWeaponParam(float Damage, bool bCanBeBlocked);

    UFUNCTION(BlueprintCallable)
    virtual void DealDamage();

    UFUNCTION(BlueprintCallable)
    virtual FName FindNextSection(UAnimMontage *AnimMontage);

    UPROPERTY(EditAnywhere)
    class UHealthComponent *HealthComponent;

    virtual void UpdateHealthBar();

private:
    /**
     * Movement
     */
    UPROPERTY(VisibleAnywhere, category = "Movement")
    EMovementType MovementType;

    /**
     * Animation montages
     */
    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *HitReactMontage;

    UPROPERTY(EditDefaultsOnly)
    UAnimMontage *DeathMontage;

    UPROPERTY(VisibleAnywhere)
    UParticleSystem *HitVFX;

    /**
     * AI
     */

    UPROPERTY(EditInstanceOnly, category = "AI")
    APatrolPath *PatrolPath;

    UPROPERTY(EditDefaultsOnly, category = "AI")
    class UBehaviorTree *BehaviorTree;

    UPROPERTY(EditDefaultsOnly)
    float DefenseRange = 500.f;

    /**
     * Combat
     */

    UPROPERTY(VisibleAnywhere)
    class UHealthBarComponent *HealthBarWidget;

    UPROPERTY()
    AActor *CombatTarget;

    UFUNCTION()
    void PlayHitReactMontage(const FName &SectionName);

    UFUNCTION()
    void PlayDeathMontage();

    UFUNCTION()
    void OnMontageBlendingOut(UAnimMontage *AnimMontage, bool bIsInterrupt);

    class UAnimInstance *AnimInstance;

    int8 AttackCounter;

    float EnemyAIAttackRange = 200;
};
