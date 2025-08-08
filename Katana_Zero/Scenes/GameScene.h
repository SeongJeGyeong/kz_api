#pragma once
#include "Scene.h"

class Player;
class Tile_FG;
class TileRenderer;
class Actor;


class GameScene : public Scene
{
	using Super = Scene;

public:
	GameScene(string mapFileName);

private:
	Player* _player = nullptr;
	//vector<Tile_FG*> _tileList;
	vector<Actor*> _colliderList;

	TileRenderer* _tileRenderer[ERenderLayer::LAYER_END];
	void LoadTiles(json tileData);
	void LoadColliders(json colliderData);
public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;
};