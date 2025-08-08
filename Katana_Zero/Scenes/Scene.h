#pragma once

#include "../UI/UIWorkTool.h"

class Scene
{
public:
	virtual ~Scene(){}

	virtual void Init();
	virtual void Destroy();
	virtual void Update(float deltaTime);
	virtual void PostUpdate(float deltaTime);
	virtual void Render(HDC hdc);

	ESceneType GetSceneType() { return _sceneType; }

protected:
	UIWorkTool _UI;
	ESceneType _sceneType = ESceneType::NONE;
};

