#include "pch.h"
#include "Axe.h"
#include "../../Game/Game.h"
#include "../../Managers/ResourceManager.h"
#include "../../Resources/Sprite.h"

void Axe::Init(Vector2 pos, Vector2 ownerPos, Vector2 dir, bool throwOrSwing)
{
	Super::Init(pos);

	CreateAABBCollider(80, 80, ECollisionLayer::ENEMY_HITBOX);
	_sprite = ResourceManager::GetInstance()->GetSprite("spr_kissyface_axe");
	vDirection = dir;
	fRadian = 0.f;
	vOwnerPos = ownerPos;
	bThrowOrSwing = throwOrSwing;
}

void Axe::Update(float deltaTime)
{
	if (!bActive) return;
	fRadian -= 10.f * deltaTime;
	Vector2 dist = vOwnerPos - GetPos();
	if (dist.LengthSquared() <= 100.f)
	{
		bActive = false;
	}

	Vector2 pos = GetPos();
	vNewPos = pos + vDirection * 500.f * deltaTime;
}

void Axe::PostUpdate(float deltaTime)
{
	if (!bActive) return;
	SetPos(vNewPos);
}

void Axe::Render(HDC hdc)
{
	if (!bActive) return;

	Vector2 pos = GetPos();
	Vector2 screenPos = Game::GetInstance()->ConvertCurSceneScreenPos(pos);
	_sprite->RenderRotatedSprite(hdc, screenPos, fRadian, 1.f, 0, false);

	Super::Render(hdc);
}

void Axe::TakeDamage(Actor* damageCauser, const Vector2& attackDirection)
{
	if (!bActive) return;
	bWasHit = true;
	fLifeTime = 0.f;
	vDirection = { -vDirection.x, -vDirection.y };
}