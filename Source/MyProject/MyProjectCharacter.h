// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "Animation/AnimMontage.h"
#include "Components/HealthComponent.h"
#include "CoreMinimal.h"
#include "DataAsset/WeaponDataAsset.h"
#include "GameFramework/Character.h"
#include "HUD/HealthBarComponent.h"
#include "InputActionValue.h"
#include "Character/CharacterState.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Interfaces/HitInterface.h"
#include "UObject/ObjectMacros.h"
#include "Interfaces/IBlockable.h"
#include "MyProjectCharacter.generated.h"

UCLASS()
class MYPROJECT_API AMyProjectCharacter : public ACharacter, public IIBlockable, public IHitInterface
{
    GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent *CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent *FollowCamera;

    /** MappingContext */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputMappingContext *DefaultMappingContext;

    /** Jump Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *JumpAction;

    /** Move Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *MoveAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *LookAction;

    /** Equip Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *EquipAction;

    /** Attack Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *AttackAction;

    /** Block or Aim Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *BlockAction;

public:
    AMyProjectCharacter();

protected:
    /** Called for movement input */
    void Move(const FInputActionValue &Value);
    void Look(const FInputActionValue &Value);
    void Equip();
    void Attack();

    UFUNCTION(BlueprintCallable)
    void Block();

    UFUNCTION(BlueprintCallable)
    void BlockEnd();

    /** Animation events */
    UFUNCTION(BlueprintCallable)
    void AttackEnd();

    UFUNCTION(BlueprintCallable)
    void ToggleWeaponCollision(ECollisionEnabled::Type CollisionType, bool IsLeftHandWeapon);

protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

    // To add mapping context
    virtual void BeginPlay();

    virtual void Jump() override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent,
                             class AController *EventInstigator, AActor *DamageCauser) override;

private:
    TArray<class AWeapon *> EquippedWeapons;

    TArray<class AItem *> OverlappingItems;

    ECharacterState CharacterState;

    EActionState ActionState;

    EWeaponType CurrentWeaponType = EWeaponType::Unequipped;

    int32 AttackCounter;

    UAnimInstance *AnimInstance;

    class AWeapon *EquippedLeftHandWeapon;

    class AWeapon *EquippedRightHandWeapon;

    UAnimMontage *AttackMontage;

    UAnimMontage *BlockMontage;

    UAnimMontage *BlockSuccessMontage;

    bool IsEquippingWeapon;

    UPROPERTY(EditDefaultsOnly)
    float CombatRemainTime;

    FTimerHandle CombatTimerHandle;

    void SetStateToUnequipped();

    void AttachWeaponToHand(UWeaponDataAsset *WeaponAsset, class AWeapon *OverlappingWeapon);

    void UnequippedOldWeapon();

    void PlayHitReactMontage(const FName &SectionName);

    void PlayDeathMontage();

    void SetWeaponAttackBehavior(float DamageAmount, bool bCanBlock);

    void OnMontageBlendingOut(UAnimMontage *AnimMontage, bool bIsInterrupted);

    void SetIsCombattingToFalse();

public:
    /**
     * Blockable Interface
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

    UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
    bool bIsBlocking;

    UPROPERTY(EditDefaultsOnly)
    bool bCanBlock;

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

    FORCEINLINE class USpringArmComponent *GetCameraBoom() const
    {
        return CameraBoom;
    }

    FORCEINLINE class UCameraComponent *GetFollowCamera() const
    {
        return FollowCamera;
    }

    void AddOverlappingItem(AItem *Item)
    {
        OverlappingItems.AddUnique(Item);
    }

    void RemoveOverlappingItem(AItem *Item)
    {
        OverlappingItems.Remove(Item);
    }

    UFUNCTION(BlueprintCallable)
    ECharacterState GetCharacterState()
    {
        return CharacterState;
    }

    UPROPERTY(EditAnywhere)
    class UHealthComponent *HealthComponent;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UUserWidget> WidgetClass;

    UPROPERTY(VisibleAnywhere)
    class UHealthBar *HealthBarWidget;

    UPROPERTY(BlueprintReadOnly)
    EDeathPose DeathPose = EDeathPose::Alive;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *DeathMontage;

    UPROPERTY(EditDefaultsOnly)
    class UAnimMontage *HitReactMontage;

    UFUNCTION(BlueprintCallable)
    void PlayAttackMontage();

    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
    bool IsCombatting = false;

    UPROPERTY(BlueprintReadOnly)
    bool CanJumpToBlockEndSectionFromStartSection;

    UPROPERTY(BlueprintReadOnly)
    bool IsAttacking = false;
};
