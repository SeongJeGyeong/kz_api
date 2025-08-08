#include "pch.h"
#include "Tile_FG.h"
#include "../../Resources/Texture.h"
#include "../../Components/TileRenderer.h"
#include "../../Managers/ResourceManager.h"
#include "../../Components/OBBCollider.h"

void Tile_FG::Init(Vector2 pos, Vector2 tilePos, int32 tilemapIndex)
{
	Super::Init(pos);

	_renderer = new TileRenderer();
	_renderer->SetOwner(this);
	_renderer->SetTileMap(ResourceManager::GetInstance()->GetTileMapForIndex(tilemapIndex));
	_renderer->InitComponent(tilePos.x, tilePos.y);
}

void Tile_FG::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

void Tile_FG::Render(HDC hdc)
{
	_renderer->RenderComponent(hdc);
	Super::Render(hdc);
}

void Tile_FG::OnCollisionHit(Collider* other)
{
	_collider->SetOverlapped(true);
}