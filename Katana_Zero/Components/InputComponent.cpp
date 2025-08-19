#include "pch.h"
#include "InputComponent.h"
#include "../Managers/InputManager.h"
#include "../Objects/Actors/Player.h"
#include "../Components/Animator.h"
#include "../Components/CameraComponent.h"

void InputComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
    _player = static_cast<Player*>(owner);
}

void InputComponent::UpdateComponent(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonPressed(KeyType::S))
    {
        bPressedDown = true;
    }
    else
    {
        bPressedDown = false;
    }
}

void InputComponent::RenderComponent(HDC hdc)
{

}

void InputComponent::Input_In_Idle(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        if (InputManager::GetInstance()->GetButtonDown(KeyType::S))
        {
            _player->Roll(false);
        }
        else
        {
            _player->Move(false);
        }
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        if (InputManager::GetInstance()->GetButtonDown(KeyType::S))
        {
            _player->Roll(true);
        }
        else
        {
            _player->Move(true);
        }
    }
    else if (!_player->GetIsCrouch() && InputManager::GetInstance()->GetButtonPressed(KeyType::S))
    {
        _player->Crouch(true);
    }
}

void InputComponent::Input_In_IdleToRun(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::S))
    {
        _player->Roll(InputManager::GetInstance()->GetButtonPressed(KeyType::D));
    }
}

void InputComponent::Input_In_Run(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::S))
    {

        _player->Roll(InputManager::GetInstance()->GetButtonPressed(KeyType::D));
    }
}

void InputComponent::Input_In_RunToIdle(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }

    if (!_player->GetIsCrouch() && InputManager::GetInstance()->GetButtonPressed(KeyType::S))
    {
        _player->Crouch(true);
    }
}

void InputComponent::Input_In_Jump(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (!_player->GetIsMaxJump() && InputManager::GetInstance()->GetButtonPressed(KeyType::W))
    {
        _player->IncreaseJump(deltaTime);
    }
    if (InputManager::GetInstance()->GetButtonUp(KeyType::W))
    {
        _player->ReleaseJump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }
}

void InputComponent::Input_In_Fall(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }
}

void InputComponent::Input_In_PreCrouch(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::A))
    {
        _player->Roll(false);
    }
    else if (InputManager::GetInstance()->GetButtonDown(KeyType::D))
    {
        _player->Roll(true);
    }

    if (InputManager::GetInstance()->GetButtonUp(KeyType::S))
    {
        _player->Crouch(false);
    }
}

void InputComponent::Input_In_Crouch(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::A))
    {
        _player->Roll(false);
    }
    else if (InputManager::GetInstance()->GetButtonDown(KeyType::D))
    {
        _player->Roll(true);
    }

    if (InputManager::GetInstance()->GetButtonUp(KeyType::S))
    {
        _player->Crouch(false);
    }
}

void InputComponent::Input_In_PostCrouch(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    else if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }

    if (InputManager::GetInstance()->GetButtonUp(KeyType::S))
    {
        _player->Crouch(true);
    }
}

void InputComponent::Input_In_Attack(float deltaTime)
{
}

void InputComponent::Input_In_Roll(float deltaTime)
{
    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        if (_player->GetAttackDelayTime() > 0.3f)
        {
            _player->Attack();
        }
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }
}
