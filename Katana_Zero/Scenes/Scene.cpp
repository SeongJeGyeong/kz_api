#include "pch.h"
#include "Scene.h"

void Scene::Init()
{
	_UI.Init();
}

void Scene::Destroy()
{
	_UI.Destroy();
}

void Scene::Update(float deltaTime)
{
	_UI.Update();
}

void Scene::PostUpdate(float deltaTime)
{
}

void Scene::Render(HDC hdc)
{
	_UI.Render(hdc);
}