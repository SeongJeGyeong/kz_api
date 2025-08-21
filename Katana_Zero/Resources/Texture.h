#pragma once

class Texture
{
protected:
	// �ؽ�ó�� �׸������� ����
	HDC		_textureHdc = 0;
	HDC		_flippedTextureHdc = 0;
	HBITMAP _bitmap;
	int32 iAlpha = 255;	// �����ϰ� �������ϴ� ����
	int32 iSizeX = 0;
	int32 iSizeY = 0;

public:
	void Load(wstring filePath, int32 alpha = 255);

	// �ش� ���ҽ��� �׷����� �κ�
	//void Render(HDC hdc, Vector2 pos);
	void RenderTexture(HDC hdc, Vector2 pos, int32 width = 0, int32 height = 0);
	void RenderTexture(HDC hdc, int32 startPosX, int32 startPosY, int32 width, int32 height, int32 srcStartX, int32 srcStartY, int32 srcEndX, int32 srcEndY, bool isFlipped = false);
	void RenderTextureNoAlpha(HDC hdc, Vector2 pos, int32 width = 0, int32 height = 0);
	void RenderProgress(HDC hdc, Vector2 pos, int32 width = 1, int32 height = 1);

	int32 GetTextureSizeX() { return iSizeX; }
	int32 GetTextureSizeY() { return iSizeY; }

	int32 GetAlpha() { return iAlpha; }
	void SetAlpha(int32 transparent) { iAlpha = transparent; }

	HDC GetTexture() const { return _textureHdc; }
	HBITMAP GetBitmap() const { return _bitmap; }

};

