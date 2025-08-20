#include "pch.h"
#include "Actor.h"
#include "../../Managers/CollisionManager.h"
#include "../../Components/OBBCollider.h"
#include "../../Components/AABBCollider.h"
#include "../../Components/LineCollider.h"
#include "../../Components/MovingLineCollider.h"

void Actor::Init(Vector2 pos)
{
	Super::Init();
	Super::SetPos(pos);
}

void Actor::Update(float deltaTime)
{
}

void Actor::Render(HDC hdc)
{
	if (_collider) _collider->RenderComponent(hdc);
}

void Actor::CreateOBBCollider(float width, float height, float rotation, ECollisionLayer layer)
{
	_collider = new OBBCollider();
	_collider->InitOBB(width, height, rotation);
	_collider->SetOwner(this);
	_collider->SetCollisionLayer(layer);
	_collider->SetColliderType(EColliderType::OBB);
	CollisionManager::GetInstance()->AddCollider(_collider);
}

void Actor::CreateAABBCollider(float width, float height, ECollisionLayer layer)
{
	_collider = new AABBCollider();
	_collider->InitAABB(width, height);
	_collider->SetOwner(this);
	_collider->SetCollisionLayer(layer);
	_collider->SetColliderType(EColliderType::AABB);
	CollisionManager::GetInstance()->AddCollider(_collider);
}

void Actor::CreateLineCollider(Vector2 start, Vector2 end, ECollisionLayer layer)
{
	_collider = new LineCollider();
	_collider->InitLine(start, end);
	_collider->SetOwner(this);
	_collider->SetCollisionLayer(layer);
	_collider->SetColliderType(EColliderType::LINE);
	CollisionManager::GetInstance()->AddCollider(_collider);
}

void Actor::CreateMovingLineCollider(float length, float radian, ECollisionLayer layer)
{
	_collider = new MovingLineCollider();
	_collider->InitLine(length, radian);
	_collider->SetOwner(this);
	_collider->SetCollisionLayer(layer);
	_collider->SetColliderType(EColliderType::LINE);
	CollisionManager::GetInstance()->AddCollider(_collider);
}