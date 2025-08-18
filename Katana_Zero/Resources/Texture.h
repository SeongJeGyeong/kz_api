#pragma once

class Texture
{
protected:
	// 텍스처를 그리기위한 변수
	HDC		_textureHdc = 0;
	HDC		_flippedTextureHdc = 0;
	HBITMAP _bitmap;
	int32 iAlpha = 255;	// 투명하게 보여야하는 색상
	int32 iSizeX = 0;
	int32 iSizeY = 0;

public:
	void Load(wstring filePath, int32 alpha = 255);

	// 해당 리소스가 그려지는 부분
	void Render(HDC hdc, Vector2 pos);
	//void RenderStretchedSprite(HDC hdc, Vector2 pos, int32 magnification, bool isFlipped);
	//void RenderStretchedSprite(HDC hdc, Vector2 pos, int32 frameCount, int32 currentFrame, int32 frameSizeX, int32 frameSizeY, int32 originX, int32 originY, int32 magnification, bool isFlipped, Vector2 offset);
	//void RenderStretchedSprite(HDC hdc, int32 startPosX, int32 startPosY, int32 endPosX, int32 endPosY, int32 srcStartX, int32 srcStartY, int32 srcEndX, int32 srcEndY, bool isFlipped);
	void RenderTexture(HDC hdc, Vector2 pos, int32 width, int32 height);
	void RenderTexture(HDC hdc, int32 startPosX, int32 startPosY, int32 width, int32 height, int32 srcStartX, int32 srcStartY, int32 srcEndX, int32 srcEndY, bool isFlipped = false);
	void RenderSprite(HDC hdc, Vector2 pos, int32 tileSizeX, int32 tileSizeY, int32 originX, int32 originY);

	int32 GetTextureSizeX() { return iSizeX; }
	int32 GetTextureSizeY() { return iSizeY; }

	int32 GetAlpha() { return iAlpha; }
	void SetAlpha(int32 transparent) { iAlpha = transparent; }

	// 회전 스프라이트
public:
	//void RenderRotatedSprite(HDC hdc, Vector2 pos, float radian, float scale, int32 currentFrame, bool isFlipped);
	//void CalculateRotatedPoints(POINT points[3], float centerX, float centerY, float radian, int imgWidth, int imgHeight);
	//RECT GetRotatedBounds(int32 centerX, int32 centerY, float radian, int32 sizeX, int32 sizeY);
};

