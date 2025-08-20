#include "pch.h"
#include "Bullet.h"
#include "../../Managers/ResourceManager.h"
#include "../../Resources/Sprite.h"
#include "../../Game/Game.h"
#include "../../Components/Collider.h"

void Bullet::Init(Vector2 pos, Vector2 dir, float length, float radian)
{
	Super::Init(pos);
	vDirection = dir;
	fLength = length;
	fRadian = radian;

	CreateMovingLineCollider(length, radian, ECollisionLayer::ENEMY_HITBOX);

	_sprite = ResourceManager::GetInstance()->GetSprite("spr_bullet");
}

void Bullet::Update(float deltaTime)
{
	if (bIsDead) return;
	fLifeTime += deltaTime;
	if (fLifeTime >= 0.5f) bIsDead = true;

	Vector2 pos = GetPos();
	vNewPos = pos + vDirection * 1000.f * deltaTime;
}

void Bullet::PostUpdate(float deltaTime)
{
	if (bIsDead) return;
	SetPos(vNewPos);
}

void Bullet::Render(HDC hdc)
{
	if (bIsDead) return;
	Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(GetPos());
	_sprite->RenderRotatedSpriteBitBlt(hdc, screenPos, fRadian, 1.f, 0, false);

	Super::Render(hdc);
}

void Bullet::TakeDamage(const Vector2& hitDir)
{
	if (bWasHit) return;
	bWasHit = true;
	fLifeTime = 0.f;
	vDirection = { -vDirection.x, -vDirection.y };
}