#pragma once
#include "UIBase.h"

class Texture;

class UIButton : public UIBase
{
	using Super = UIBase;
public:
	UIButton(Vector2 pos, string key, int32 width = 0, int32 height = 0);
	virtual ~UIButton() {};

private:
	function<void()> onClickedEvent;
	Texture* _texture = nullptr;
	wstring sButtonText = L"";

public:
	virtual void Update() override;
	virtual void Render(HDC hdc) override;
	void SetText(wstring text) { sButtonText = text; }
	void SetClickEvent(function<void()> click) { onClickedEvent = click; }

	void OnBtnHovered();
	void OnBtnUnHovered();
	//void OnBtnHovered();
};

