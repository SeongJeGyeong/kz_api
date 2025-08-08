#pragma once
#include "Actor.h"

class TileRenderer;

class Tile_FG : public Actor
{
	using Super = Actor;

private:
	int32 iTileSize = TILE_SIZE;
	Vector2 vTilePos;
	TileRenderer* _renderer = nullptr;

public:
	virtual void Init(Vector2 pos, Vector2 tilePos, int32 tilemapIndex);
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void SetTilePos(Vector2 pos) { vTilePos = pos; }
	Vector2 GetTilePos() { return vTilePos; }



	virtual void OnCollisionHit(Collider* other) override;
};

