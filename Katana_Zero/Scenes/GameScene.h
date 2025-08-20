#pragma once
#include "Scene.h"

class Player;
class Enemy;
class Tile_FG;
class TileRenderer;
class Actor;
class Camera;
class Bullet;

class GameScene : public Scene
{
	using Super = Scene;

public:
	GameScene(string mapFileName);

private:
	Player* _player = nullptr;
	vector<Enemy*> _EnemyList;
	vector<Bullet*> _BulletList;
	vector<Actor*> _colliderList;

	TileRenderer* _tileRenderer;
	void LoadTiles(json tileData);
	void LoadColliders(json colliderData);
	void LoadActors(json actorData);

public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void CreateBullet(Vector2 pos, Vector2 dir, float length, float radian);
};