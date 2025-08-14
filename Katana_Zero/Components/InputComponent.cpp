#include "pch.h"
#include "InputComponent.h"
#include "../Managers/InputManager.h"
#include "../Objects/Actors/Player.h"

void InputComponent::InitComponent(Actor* owner)
{
	SetOwner(owner);
    _player = static_cast<Player*>(owner);
}

void InputComponent::UpdateComponent(float deltaTime)
{
    if (!_player->GetIsJumped() && InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }
    if (_player->GetIsJumped() && !_player->GetIsMaxJump() && InputManager::GetInstance()->GetButtonPressed(KeyType::W))
    {
        _player->IncreaseJump(deltaTime);
    }
    if (InputManager::GetInstance()->GetButtonUp(KeyType::W))
    {
        _player->ReleaseJump();
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::S))
    {
        if (_player->GetIsGround() || _player->GetOnStair())
        {
            if (_player->GetCurrentState() == EPlayerState::PLAYER_IDLE)
            {
                _player->Crouch(true);
            }
        }
        else
        {
            _player->Landing();
        }
    }
    else if (InputManager::GetInstance()->GetButtonUp(KeyType::S))
    {
        if (_player->GetCurrentState() == EPlayerState::PLAYER_CROUCH || 
            _player->GetCurrentState() == EPlayerState::PLAYER_PRECROUCH)
        {
            _player->Crouch(false);
        }
    }
    if (InputManager::GetInstance()->GetButtonPressed(KeyType::A))
    {
        _player->Move(false);
    }
    if (InputManager::GetInstance()->GetButtonPressed(KeyType::D))
    {
        _player->Move(true);
    }

    if (InputManager::GetInstance()->GetButtonDown(KeyType::LeftMouse))
    {
        _player->Attack();
    }
}

void InputComponent::RenderComponent(HDC hdc)
{
}