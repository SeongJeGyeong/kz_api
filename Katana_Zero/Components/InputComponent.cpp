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
    if (_player->GetIsGround() && !_player->GetIsJumped() && InputManager::GetInstance()->GetButtonDown(KeyType::W))
    {
        _player->Jump();
    }
    if (!_player->GetIsGround() && !_player->GetIsJumped() && InputManager::GetInstance()->GetButtonPressed(KeyType::W))
    {
        _player->IncreaseJump(deltaTime);
    }
    if (InputManager::GetInstance()->GetButtonUp(KeyType::W))
    {
        _player->ReleaseJump();
    }

    if (InputManager::GetInstance()->GetButtonPressed(KeyType::S))
    {

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