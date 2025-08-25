#pragma once
#include "Scene.h"

class Player;
class Enemy;
class TileRenderer;
class Actor;
class Camera;
class Bullet;
class Boss;
class Axe;
class Texture;
class UIProgressBar;
class UIBundle;

class GameScene : public Scene
{
	using Super = Scene;

public:
	GameScene(string mapName, string mapFileName, string nextStage, string bgmName);
	function<void(string)> OnRetryGame;
	function<void()> OnExitToMainMenu;

private:
	Player* _player = nullptr;
	vector<Enemy*> _EnemyList;
	vector<Bullet*> _BulletList;
	vector<Actor*> _colliderList;
	Boss* _boss = nullptr;
	Axe* _axe = nullptr;

	UIWorkTool _pauseUI;

	Texture* _slowMotionMask = nullptr;

	TileRenderer* _tileRenderer;

	bool bIsPaused = false;

	float fTimeLimit = 120.f;
	float fCurrentElapsedTime = 120.f;
	UIProgressBar* _timerProgressBar = nullptr;
	UIBundle* _slowMotionBattery = nullptr;
	UIImage* _gameOverBox = nullptr;
	float fGameOverWait = 0.f;

	string _nextStage;
	RECT _gameOverRect = { (LONG)(GWinSizeX * 0.5f - 100.f), (LONG)(GWinSizeY * 0.5f - 50.f), (LONG)(GWinSizeX * 0.5f + 100.f), (LONG)(GWinSizeY * 0.5f + 50.f) };
	wstring _gameOverText;

private:
	void LoadTiles(json tileData);
	void LoadColliders(json colliderData);
	void LoadActors(json actorData);

public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual void Update(float deltaTime) override;
	virtual void PostUpdate(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	void UIInit();

	void CreateBullet(Vector2 pos, Vector2 dir, float length, float radian);
	void SpawnAxe(Vector2 pos, Actor* owner, Vector2 dir, bool throwOrSwing);
};