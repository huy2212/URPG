// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Character/CharacterState.h"
#include "UObject/ObjectMacros.h"
#include "EnemyAIController.generated.h"
/**
 *
 */
UENUM(BlueprintType)
enum class ESenseType : uint8
{
    None,
    Sight,
    Hearing,
    Damage,
};

UCLASS()
class MYPROJECT_API AEnemyAIController : public AAIController
{
    GENERATED_BODY()

public:
    AEnemyAIController(const FObjectInitializer &ObjectInitializer);

    UFUNCTION(BlueprintCallable)
    virtual void SetStateToAttacking(AActor *ActorToAttack);

    UFUNCTION(BlueprintCallable)
    virtual void SetStateToPassive();

    UFUNCTION(BlueprintCallable)
    virtual void SetStateToInvestigating(FVector Location);

    UFUNCTION(BlueprintCallable)
    virtual void SetStateToStrafe(AActor *ActorToAttack);

    UFUNCTION(BlueprintCallable)
    void CanSenseActor(AActor *Actor, const ESenseType SenseType, bool &bOut_IsSensed, FAIStimulus &Stimulus);

    UFUNCTION(BlueprintCallable)
    void HandleAttackTargetDead();

    UFUNCTION(BlueprintCallable)
    virtual void SetStateToDead();

    UFUNCTION(BlueprintCallable)
    virtual void SetAttackRange(float Value);

    UFUNCTION(BlueprintCallable)
    AActor *GetAttackTarget() const
    {
        return AttackTarget;
    }

    UPROPERTY(EditAnywhere)
    UAIPerceptionComponent *AIPerceptionComponent;

protected:
    virtual void OnPossess(APawn *InPawn) override;

    UFUNCTION()
    virtual void ActorsPerceptionUpdated(const TArray<AActor *> &UpdatedActors) override;

    /**
     * Handle Sense
     */
    UFUNCTION(BlueprintCallable)
    virtual void HandleSenseSight(AActor *SeenActor);

    UFUNCTION(BlueprintCallable)
    virtual void HandleSenseHearing();

    UFUNCTION(BlueprintCallable)
    virtual void HandleSenseDamage(AActor *ActorToDamage);

    UFUNCTION(BlueprintCallable)
    virtual void RunBehaviourTree();

private:
    /**
     * AI
     */
    UPROPERTY(EditDefaultsOnly, category = "AI")
    class UBehaviorTree *BTAsset;

    UPROPERTY(EditDefaultsOnly, category = "AI")
    class UBlackboardComponent *BBComponent;

    class UBehaviorTreeComponent *BTComponent;

    /**
     * Combat
     */
    class IEnemyAIInterface *EnemyAIInterface;

    AActor *AttackTarget;

    FAIStimulus Stimulus;

    EActionState CurrentEnemyState;

    float AttackRange, DefenseRange;

    /**
     * Black board key name
     */
    FName AttackTargetKeyName = "AttackTarget";

    FName StateKeyName = "State";

    FName InvestigateLocationKeyName = "InvestigatingLocation";

    FName AttackRangeKeyName = "AttackRange";

    FName DefenseRangeKeyName = "DefenseRange";
};
