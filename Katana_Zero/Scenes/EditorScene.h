#pragma once
#include "Scene.h"

class Actor;
class TileRenderer;
class Texture;
class EditorCamera;

enum EditorMode
{
	TILEMODE,
	COLLIDERMODE,
};

class EditorScene : public Scene
{
	using Super = Scene;

private:
	class EditorSub* _subWindow = nullptr;

	int32 iWorldGridMaxX;
	int32 iWorldGridMaxY;

	// vector�� �ε��� : ������ �� �׸��� �ε���
	// Vector2 : Ÿ�ϸ� �ؽ�ó�� Ÿ�� ��ǥ
	unordered_map<int32, TileInfo>_tileInfoMap[ERenderLayer::LAYER_END];
	vector<Texture*> _tileMapList;
	vector<ColliderInfo> _colliderList;

	//EditorCamera* _camera;
	EditorMode eMode = EditorMode::TILEMODE;
	EColliderMode eColMode = EColliderMode::COL_BOX;

	POINT _startPoint = { INT_MAX, INT_MAX };

private:
	void DrawGird(HDC hdc);
	void DrawTile(HDC hdc);
	void DrawCollider(HDC hdc);

public:
	virtual void Init() override;
	virtual void Destroy() override;
	virtual void Update(float deltaTime) override;
	virtual void Render(HDC hdc) override;

	// y * width + x
	int32 ConvertWorldGridToIndex(Vector2 pos) { return (int32)pos.y * iWorldGridMaxX + pos.x; }
	// x % width, y / width
	Vector2 ConvertIndexToWorldGrid(int32 idx)
	{
		float x = idx % iWorldGridMaxX;
		float y = idx / iWorldGridMaxX;
		return { x, y };
	}

	void SaveMap();
	void LoadMap();

	void TileModeUpdate();
	void ColliderModeUpdate();

	void RednerBox(HDC hdc, POINT startPoint, EColliderMode mode);
	void RednerLine(HDC hdc, POINT startPoint, EColliderMode mode);
};

