#include "pch.h"
#include "UIWorkTool.h"
#include "UIBase.h"
#include "UIButton.h"
#include "UIImage.h"

void UIWorkTool::Destroy()
{
	DestroyAllWidget();
}

void UIWorkTool::Init()
{

}

void UIWorkTool::Update()
{
	for (auto iter : _widgetList)
	{
		if (iter->IsOpen())
		{
			iter->Update();
		}
	}
}

void UIWorkTool::Render(HDC hdc)
{
	for (auto iter : _widgetList)
	{
		if (iter->IsOpen())
		{
			iter->Render(hdc);
		}
	}
}

void UIWorkTool::DestroyAllWidget()
{
}

UIButton* UIWorkTool::CreateButton(Vector2 pos, string key, wstring text, int32 width, int32 height, int32 fontSize, uint64 textColor)
{
	UIButton* button = new UIButton(pos, key, width, height, fontSize, textColor);
	if (text != L"")
	{
		button->SetText(text);
	}
	_widgetList.emplace_back(button);
	return button;
}

UIImage* UIWorkTool::CreateImage(Vector2 pos, string key, int32 width, int32 height)
{
	UIImage* image = new UIImage(pos, key, width, height);
	_widgetList.emplace_back(image);
	return image;
}
