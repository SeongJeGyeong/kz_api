#include "pch.h"
#include "UIWorkTool.h"
#include "UIBase.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UIBundle.h"
#include "UIProgressBar.h"

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
	for (auto& widget : _widgetList)
	{
		SAFE_DELETE(widget);
	}
	_widgetList.clear();
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

UIBundle* UIWorkTool::CreateBundle(Vector2 pos, string key, int32 width, int32 height, int32 count)
{
	UIBundle* bundle = new UIBundle(pos, key, width, height, count);
	_widgetList.emplace_back(bundle);
	return bundle;
}

UIProgressBar* UIWorkTool::CreateProgressBar(Vector2 pos, string key, int32 width, int32 height)
{
	UIProgressBar* progressBar = new UIProgressBar(pos, key, width, height);
	_widgetList.emplace_back(progressBar);
	return progressBar;
}
