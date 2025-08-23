#pragma once

class UIBase;
class UIButton;
class UIImage;

class UIWorkTool
{
public:
	void Init();
	void Destroy();

	void Update();
	void Render(HDC hdc);

	void DestroyAllWidget();

	UIButton* CreateButton(Vector2 pos, string key, wstring text = L"", int32 width = 0, int32 height = 0, int32 fontSize = 0, uint64 textColor = RGB(255, 255, 255));
	UIImage* CreateImage(Vector2 pos, string key, int32 width = 0, int32 height = 0);

private:
	vector<UIBase*> _widgetList;

};

