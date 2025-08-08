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
	static size_t StaticClass()
	{
		static char id = 0;
		return (size_t)&id;
	}

private:
	int32 iPosX;
	int32 iPosY;

	Texture* _tileMap;

	vector<tileRenderInfo> _tileRenderList;

public:
	virtual void InitComponent(int32 x, int32 y);
	virtual void UpdateComponent(float deltaTime) override;
	virtual void RenderComponent(HDC hdc) override;

	void SetTileMap(Texture* texture);
	void SetTilePos(Vector2 pos) { iPosX = pos.x; iPosY = pos.y; }
	void SetTilePos(int32 posX, int32 posY) { iPosX = posX; iPosY = posY; }

	void AddTileInfo(Vector2 worldPos, Vector2 tilePos);
};