#pragma once
class UIBase
{
public:
	UIBase(Vector2 pos);
	virtual ~UIBase() {};

	virtual void Init() {}
	virtual void Update() {}
	virtual void Render(HDC hdc) {};
	virtual void Open() { bIsOpen = true; }
	virtual void Close() { bIsOpen = false; }

	bool IsOpen() const { return bIsOpen; }
	void SetOpen(bool open) { bIsOpen = open; }

	void SetIsStretch(bool stretch, int32 sizeX, int32 sizeY);

protected:
	bool bIsStretch = false;
	bool bIsOpen = true;
	Vector2 vPos = {};

	int32 iSizeX;
	int32 iSizeY;
};

