// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAsset/WeaponDataAsset.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Character/CharacterState.h"
#include "MyProjectCharacter.generated.h"
UCLASS(config = Game) class AMyProjectCharacter : public ACharacter
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

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *EquipAction;

    /** Look Input Action */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
    class UInputAction *AttackAction;

    UPROPERTY(VisibleInstanceOnly)
    class AItem *OverlappingItem;

public:
    AMyProjectCharacter();

    void SetOverlappingItem(AItem *Item)
    {
        OverlappingItem = Item;
    }

    ECharacterState GetCharacterState()
    {
        return CharacterState;
    }

protected:
    /** Called for movement input */
    void Move(const FInputActionValue &Value);

    /** Called for looking input */
    void Look(const FInputActionValue &Value);
    void Equip();
    void Attack();

    void PlayAttackMontage();

    UFUNCTION(BlueprintCallable)
    void AttackEnd();

    UFUNCTION(BlueprintCallable)
    void ToggleCollision(ECollisionEnabled::Type CollisionType);

    void UnequippedOldWeapon();

protected:
    // APawn interface
    virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

    // To add mapping context
    virtual void BeginPlay();

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent *GetCameraBoom() const
    {
        return CameraBoom;
    }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent *GetFollowCamera() const
    {
        return FollowCamera;
    }

private:
    ECharacterState CharacterState = ECharacterState::Unarmed;

    EActionState ActionState = EActionState::Unoccupied;
    int32 AttackCounter;

    class AWeapon *EquippedWeapon = nullptr;
    class AWeapon *AnotherHandWeapon;
    bool IsOneWeaponEquipped = false;
};
