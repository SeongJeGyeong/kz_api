#pragma once
#include "Component.h"

struct tileRenderInfo
{
	Vector2 worldPos;
	Vector2 tilePos;
};

class Texture;

class TileRenderer : public Component
{
public:
	TileRenderer() { iHalfTileSize = TILE_SIZE / 2; }

	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	int32 iHalfTileSize;
	Texture* _tileMap;

	vector<tileRenderInfo> _tileRenderList;

public:
	virtual void InitComponent() override;
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void SetTileMap(Texture* texture);

	void AddTileInfo(Vector2 worldPos, Vector2 tilePos);
};