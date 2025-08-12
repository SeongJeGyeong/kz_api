#pragma once

class Texture
{
private:
	// �ؽ�ó�� �׸������� ����
	HDC		_textureHdc = 0;
	HDC		_flippedTextureHdc = 0;
	HBITMAP _bitmap;
	int32 iAlpha = 255;	// �����ϰ� �������ϴ� ����
	int32 iSizeX = 0;
	int32 iSizeY = 0;
	Vector2 vOffset;	// ���Ϳ��� ��µ� ��ġ ����

	// ��������Ʈ
private:
	int32 iFrameSizeX;
	int32 iFrameSizeY;
	int32 iFrameCount = 0;
	int32 iCurFrame = 0;
	float fDuration = 0;
	bool bLoop = false;

public:
	void Load(wstring filePath, Vector2 offset = Vector2(0, 0), int32 alpha = 255);
	void LoadSprite(wstring filePath, int32 tileSizeX, int32 tileSizeY, int32 frameCount, float duration, bool loop, Vector2 offset = Vector2(0, 0), int32 alpha = 255);

	// �ش� ���ҽ��� �׷����� �κ�
	void Render(HDC hdc, Vector2 pos);
	void RenderStretched(HDC hdc, Vector2 pos, int32 stretchWidth, int32 stretchHeight);
	void RenderStretchedSprite(HDC hdc, Vector2 pos, int32 magnification, bool isFlipped);
	void RenderSprite(HDC hdc, Vector2 pos, int32 tileSizeX, int32 tileSizeY, int32 originX, int32 originY);

	int32 GetTextureSizeX() { return iSizeX; }
	int32 GetTextureSizeY() { return iSizeY; }

	Vector2 GetOffset() const { return vOffset; }
	void SetOffset(const Vector2& offset) { vOffset = offset; }

	int32 GetAlpha() { return iAlpha; }
	void SetAlpha(int32 transparent) { iAlpha = transparent; }

	int32 GetCurFrameCount() { return iCurFrame; }
	void SetCurFrameCount(int32 frame) { iCurFrame = frame; }
	int32 GetMaxFrameCount() { return iFrameCount; }
	float GetDuration() { return fDuration; }

	bool IsSpriteEnd() { return iCurFrame >= iFrameCount; }
	bool IsLoop() { return bLoop; }

	// ȸ�� ��������Ʈ
public:
	void RenderRotatedSprite(HDC hdc, Vector2 pos, float radian, float scale, int32 currentFrame, bool isFlipped);
	void CalculateRotatedPoints(POINT points[3], float centerX, float centerY, float radian, int imgWidth, int imgHeight);
	RECT GetRotatedBounds(int32 centerX, int32 centerY, float radian, int32 sizeX, int32 sizeY);
};

