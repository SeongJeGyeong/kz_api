#include "pch.h"
#include "Sprite.h"

void Sprite::LoadSprite(wstring filePath, int32 frameSizeX, int32 frameSizeY, float magnification, int32 frameCount, float duration, bool loop, int32 alpha)
{
	Load(filePath, alpha);

	iSpriteSizeX = frameSizeX;
	iSpriteSizeY = frameSizeY;
	fMagnification = magnification;
	iFrameCount = frameCount;
	fDuration = duration;
	bLoop = loop;
}

void Sprite::RenderStretchedSprite(HDC hdc, Vector2 pos, int32 curFrame, bool isFlipped)
{
	int32 stretchedWidth = iSpriteSizeX * fMagnification;
	int32 stretchedHeight = iSpriteSizeY * fMagnification;

	int32 startX = (int32)pos.x - (stretchedWidth * 0.5f);
	int32 startY = (int32)pos.y - (stretchedHeight * 0.5f);
	int32 srcX = iSpriteSizeX;

	if (isFlipped)
	{
		srcX *= (iFrameCount - curFrame - 1);
	}
	else
	{
		srcX *= curFrame;
	}

	Super::RenderTexture(hdc, startX, startY, stretchedWidth, stretchedHeight, srcX, 0, iSpriteSizeX, iSpriteSizeY, isFlipped);
}

void Sprite::RenderRotatedSprite(HDC hdc, Vector2 pos, float radian, float scale, int32 curFrame, bool isFlipped)
{
	// 회전한 이미지의 축에 정렬된 AABB 계산(회전된 이미지를 가두는 AABB)
	RECT bounds = GetRotatedBounds(pos.x, pos.y, radian, iSpriteSizeX, iSpriteSizeY);

	int tempWidth = bounds.right - bounds.left;
	int tempHeight = bounds.bottom - bounds.top;

	// 중간 DC, 비트맵 설정
	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = tempWidth;
	bmi.bmiHeader.biHeight = -tempHeight; // top-down DIB
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;
	HBITMAP hDibBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
	if (!hDibBmp || !pBits)
		return;

	// 완전 투명하게 초기화
	memset(pBits, 0, tempWidth * tempHeight * 4);

	// 중간 DC 생성
	HDC rotatedDC = CreateCompatibleDC(hdc);
	HBITMAP oldBmp = (HBITMAP)SelectObject(rotatedDC, hDibBmp);

	// 임시 DC 좌표계로 조정된 점들
	POINT points[3];
	CalculateRotatedPoints(points, (float)(pos.x - bounds.left), (float)(pos.y - bounds.top), radian, iSpriteSizeX, iSpriteSizeY);

	if (isFlipped)
	{
		PlgBlt(rotatedDC, points, _flippedTextureHdc, (iFrameCount - curFrame - 1) * iSpriteSizeX, 0, iSpriteSizeX, iSpriteSizeY, nullptr, 0, 0);
	}
	else
	{
		PlgBlt(rotatedDC, points, _textureHdc, curFrame * iSpriteSizeX, 0, iSpriteSizeX, iSpriteSizeY, nullptr, 0, 0);
	}

	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = iAlpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc,
		bounds.left - (tempWidth * (scale - 1.f) / 2.f),
		bounds.top - (tempHeight * (scale - 1.f) / 2.f),
		tempWidth * scale,
		tempHeight * scale,
		rotatedDC,
		0,
		0,
		tempWidth,
		tempHeight,
		op);

	// 정리
	SelectObject(rotatedDC, oldBmp);
	DeleteObject(hDibBmp);
	DeleteDC(rotatedDC);
}

void Sprite::CalculateRotatedPoints(POINT points[3], float centerX, float centerY, float radian, int imgWidth, int imgHeight)
{
	float cosA = cos(radian);
	float sinA = sin(radian);

	// 이미지의 네 모서리 (중심 기준)
	float halfW = imgWidth / 2.0f;
	float halfH = imgHeight / 2.0f;

	// 좌상단 -> 우상단 -> 좌하단 순서로 3개 점만 필요 (PlgBlt 특성)
	// 좌상단 (-halfW, -halfH)
	float x1 = -halfW * cosA - (-halfH) * sinA;
	float y1 = -halfW * sinA + (-halfH) * cosA;
	points[0].x = (LONG)(centerX + x1);
	points[0].y = (LONG)(centerY + y1);
	// 우상단 (halfW, -halfH)
	float x2 = halfW * cosA - (-halfH) * sinA;
	float y2 = halfW * sinA + (-halfH) * cosA;
	points[1].x = (LONG)(centerX + x2);
	points[1].y = (LONG)(centerY + y2);
	// 좌하단 (-halfW, halfH)
	float x3 = -halfW * cosA - halfH * sinA;
	float y3 = -halfW * sinA + halfH * cosA;
	points[2].x = (LONG)(centerX + x3);
	points[2].y = (LONG)(centerY + y3);
}

RECT Sprite::GetRotatedBounds(int32 centerX, int32 centerY, float radian, int32 sizeX, int32 sizeY)
{
	float cosA = cos(radian);
	float sinA = sin(radian);
	float halfW = sizeX / 2.0f;
	float halfH = sizeY / 2.0f;

	// 네 모서리 모두 계산
	float corners[4][2] = {
		{-halfW, -halfH}, {halfW, -halfH}, {halfW, halfH}, {-halfW, halfH}
	};

	int minX = INT_MAX, minY = INT_MAX;
	int maxX = INT_MIN, maxY = INT_MIN;

	for (int i = 0; i < 4; i++) {
		float x = corners[i][0] * cosA - corners[i][1] * sinA + centerX;
		float y = corners[i][0] * sinA + corners[i][1] * cosA + centerY;

		minX = min(minX, (int)x);
		minY = min(minY, (int)y);
		maxX = max(maxX, (int)x);
		maxY = max(maxY, (int)y);
	}

	return { minX, minY, maxX, maxY };
}