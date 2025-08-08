#include "pch.h"
#include "TileRenderer.h"
#include "../Resources/Texture.h"

void TileRenderer::InitComponent(int32 x, int32 y)
{
	iPosX = x;
	iPosY = y;
}

void TileRenderer::UpdateComponent(float deltaTime)
{
}

void TileRenderer::RenderComponent(HDC hdc)
{
	for (const tileRenderInfo& info : _tileRenderList)
	{
		_tileMap->RenderSprite(hdc, info.worldPos, TILE_SIZE, TILE_SIZE, info.tilePos.x * TILE_SIZE, info.tilePos.y * TILE_SIZE);
	}
}

void TileRenderer::SetTileMap(Texture* texture)
{
	_tileMap = texture;
}

void TileRenderer::AddTileInfo(Vector2 worldPos, Vector2 tilePos)
{
	_tileRenderList.push_back({ worldPos, tilePos});
}
