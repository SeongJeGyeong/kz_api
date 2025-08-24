#pragma once
#include "UIBase.h"

class Texture;

class UIImage : public UIBase
{
	using Super = UIBase;
private:
	Texture* _texture = nullptr;

public:
	UIImage(Vector2 pos, string key, int32 width = 0, int32 height = 0);
	virtual ~UIImage();

	void Update() override;
	void Render(HDC hdc) override;
};