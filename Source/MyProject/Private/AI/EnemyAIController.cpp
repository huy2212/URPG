// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/EnemyAIController.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Character/CharacterState.h"
#include "Enemy/Enemy.h"
#include "Engine/Engine.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "HAL/Platform.h"
#include "Interfaces/EnemyAIInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/TokenizedMessage.h"
#include "Math/Color.h"
#include "Math/MathFwd.h"
#include "MyProject/MyProjectCharacter.h"
#include "Perception/AIPerceptionListenerInterface.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISense.h"
#include "Templates/SubclassOf.h"
#include "TimerManager.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Damage.h"
#include "BrainComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer &ObjectInitializer)
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AIPerceptionComponent");
    BrainComponent = BTComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BTComponent");
}

void AEnemyAIController::OnPossess(APawn *InPawn)
{
    Super::OnPossess(InPawn);
    EnemyAIInterface = Cast<IEnemyAIInterface>(InPawn);

    if (BTAsset)
    {
        UBlackboardComponent *BlackboardComponent = Cast<UBlackboardComponent>(Blackboard.Get());
        UseBlackboard(BTAsset->GetBlackboardAsset(), BlackboardComponent);
        BBComponent = BlackboardComponent;
    }

    SetStateToPassive();
    RunBehaviourTree();

    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AEnemyAIController::ActorsPerceptionUpdated);
    }

    if (BBComponent)
    {
        if (EnemyAIInterface)
        {
            EnemyAIInterface->GetAttackRange(AttackRange, DefenseRange);
        }
        BBComponent->SetValueAsFloat(AttackRangeKeyName, AttackRange);
        BBComponent->SetValueAsFloat(DefenseRangeKeyName, DefenseRange);
        BBComponent->SetValueAsFloat(DefenseRangeKeyName, DefenseRange);
    }
}

void AEnemyAIController::RunBehaviourTree()
{
    if (BTAsset)
    {
        BTComponent->StartTree(*BTAsset);
        BTComponent->StartLogic();
    }
}

void AEnemyAIController::SetStateToAttacking(AActor *ActorToAttack)
{
    if (EnemyAIInterface)
    {
        EnemyAIInterface->SetActionState(EActionState::Attacking);
    }
    if (BBComponent)
    {
        BBComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EActionState::Attacking));
        if (ActorToAttack)
        {
            BBComponent->SetValueAsObject(AttackTargetKeyName, Cast<UObject>(ActorToAttack));
            AttackTarget = ActorToAttack;
        }
    }
}

void AEnemyAIController::SetStateToPassive()
{
    if (EnemyAIInterface)
    {
        EnemyAIInterface->SetActionState(EActionState::Passive);
    }
    if (BBComponent)
    {
        BBComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EActionState::Passive));
    }
}

void AEnemyAIController::SetStateToInvestigating(FVector InvestigateLocation)
{
    if (EnemyAIInterface)
    {
        EnemyAIInterface->SetActionState(EActionState::Investigating);
    }
    if (BBComponent)
    {
        BBComponent->SetValueAsVector(InvestigateLocationKeyName, InvestigateLocation);
        BBComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EActionState::Investigating));
    }
}

void AEnemyAIController::SetStateToStrafe(AActor *ActorToAttack)
{
    GEngine->AddOnScreenDebugMessage(1, 1, FColor::Red, "Handle Sight");
    if (EnemyAIInterface)
    {
        EnemyAIInterface->SetActionState(EActionState::Strafe);
    }
    if (BBComponent)
    {
        if (ActorToAttack)
        {
            BBComponent->SetValueAsObject(AttackTargetKeyName, Cast<UObject>(ActorToAttack));
            AttackTarget = ActorToAttack;
        }
        BBComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EActionState::Strafe));
    }
}

void AEnemyAIController::ActorsPerceptionUpdated(const TArray<AActor *> &UpdatedActors)
{
    Super::ActorsPerceptionUpdated(UpdatedActors);
    if (EnemyAIInterface->GetEnemyActionState() == EActionState::Dead) return;
    for (AActor *Actor : UpdatedActors)
    {
        bool WasSuccessfullySensedSight = false;
        CanSenseActor(Actor, ESenseType::Sight, WasSuccessfullySensedSight, Stimulus);
        if (WasSuccessfullySensedSight)
        {
            HandleSenseSight(Actor);
        }
        bool WasSuccessfullySensedHearing = false;
        CanSenseActor(Actor, ESenseType::Hearing, WasSuccessfullySensedHearing, Stimulus);
        if (WasSuccessfullySensedHearing)
        {
            HandleSenseHearing();
        }
        bool WasSuccessfullySensedDamage = false;
        CanSenseActor(Actor, ESenseType::Damage, WasSuccessfullySensedDamage, Stimulus);
        if (WasSuccessfullySensedDamage)
        {
            HandleSenseDamage(Actor);
        }
    }
}

void AEnemyAIController::CanSenseActor(AActor *Actor, const ESenseType SenseType, bool &Out_IsSensed,
                                       FAIStimulus &Out_Stimulus)
{
    FActorPerceptionBlueprintInfo PerceptionInfo;
    AIPerceptionComponent->GetActorsPerception(Actor, PerceptionInfo);

    // Find out AISenseID of SenseType
    FAISenseID AISenseID;
    switch (SenseType)
    {
    case (ESenseType::Sight):
        AISenseID = UAISense::GetSenseID<UAISense_Sight>();
        break;
    case (ESenseType::Hearing):
        AISenseID = UAISense::GetSenseID<UAISense_Hearing>();
        break;
    case (ESenseType::Damage):
        AISenseID = UAISense::GetSenseID<UAISense_Damage>();
        break;
    default:
        AISenseID = FAISenseID::InvalidID();
    }

    for (FAIStimulus EachStimulus : PerceptionInfo.LastSensedStimuli)
    {
        if (EachStimulus.Type == AISenseID)
        {
            Out_IsSensed = EachStimulus.WasSuccessfullySensed();
            Out_Stimulus = EachStimulus;
            break;
        }
    }
}

void AEnemyAIController::HandleSenseSight(AActor *Actor)
{
    CurrentEnemyState = EnemyAIInterface->GetEnemyActionState();
    if (CurrentEnemyState == EActionState::Passive || CurrentEnemyState == EActionState::Investigating)
    {
        if (Cast<ACharacter>(Actor))
        {
            // SetStateToAttacking(Actor);
            SetStateToStrafe(Actor);
        }
    }
}

void AEnemyAIController::HandleSenseHearing()
{
    CurrentEnemyState = EnemyAIInterface->GetEnemyActionState();
    if (CurrentEnemyState == EActionState::Passive || CurrentEnemyState == EActionState::Investigating)
    {
        SetStateToInvestigating(Stimulus.StimulusLocation);
    }
}

void AEnemyAIController::HandleSenseDamage(AActor *ActorToDamage)
{
    CurrentEnemyState = EnemyAIInterface->GetEnemyActionState();
    if (CurrentEnemyState == EActionState::Passive || CurrentEnemyState == EActionState::Investigating ||
        CurrentEnemyState == EActionState::Attacking)
    {
        if (Cast<AMyProjectCharacter>(ActorToDamage))
        {
            SetStateToAttacking(ActorToDamage);
        }
    }
}

void AEnemyAIController::HandleAttackTargetDead()
{
    SetStateToPassive();
}

void AEnemyAIController::SetStateToDead()
{
    if (BBComponent)
    {
        BBComponent->SetValueAsEnum(StateKeyName, static_cast<uint8>(EActionState::Dead));
    }
    UnPossess();
}

void AEnemyAIController::SetAttackRange(float Value)
{
    if (BBComponent)
    {
        BBComponent->SetValueAsFloat(AttackRangeKeyName, Value);
    }
}
