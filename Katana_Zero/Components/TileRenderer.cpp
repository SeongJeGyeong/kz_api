#include "pch.h"
#include "TileRenderer.h"
#include "../Resources/Texture.h"
#include "../Game/Game.h"
#include "../Objects/Camera.h"

void TileRenderer::InitComponent()
{
	iHalfTileSize = TILE_SIZE / 2;
}

void TileRenderer::UpdateComponent(float deltaTime)
{
}

void TileRenderer::RenderComponent(HDC hdc)
{
	Camera* camera = Game::GetInstance()->GetCurrentSceneCamera();
	if (camera == nullptr) return;

	for (const tileRenderInfo& info : _tileRenderList)
	{
		Vector2 screenPos = camera->ConvertScreenPos(info.worldPos);
		_tileMap->RenderTexture(hdc, screenPos.x - (TILE_SIZE * 0.5f), screenPos.y - (TILE_SIZE * 0.5f), TILE_SIZE, TILE_SIZE, info.tilePos.x * TILE_SIZE, info.tilePos.y * TILE_SIZE, TILE_SIZE, TILE_SIZE);
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
