#include "pch.h"
#include "Texture.h"
#include "../Game/Game.h"

void Texture::Load(wstring filePath, Vector2 offset, int32 alpha)
{
	// WinAPI 텍스처 로딩하는 방법
	HDC hdc = ::GetDC(Game::GetInstance()->GetHwnd());

	_textureHdc = ::CreateCompatibleDC(hdc);
	_bitmap = (HBITMAP)::LoadImageW(
		nullptr,
		filePath.c_str(),
		IMAGE_BITMAP,
		0,
		0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION
	);
	if (_bitmap == 0)
	{
		::MessageBox(Game::GetInstance()->GetHwnd(), filePath.c_str(), L"Invalid Texture Load", MB_OK);
		return;
	}

	HBITMAP prev = (HBITMAP)::SelectObject(_textureHdc, _bitmap);
	::DeleteObject(prev);

	BITMAP bit = {};
	::GetObject(_bitmap, sizeof(BITMAP), &bit);

	iSizeX = bit.bmWidth;
	iSizeY = bit.bmHeight;
	iFrameSizeX = iSizeX;
	iFrameSizeY = iSizeY;
	vOffset = offset;
	iAlpha = alpha;

	// 좌우 반전된 hdc도 하나 저장
	_flippedTextureHdc = ::CreateCompatibleDC(_textureHdc);
	HBITMAP flippedBitmap = ::CreateCompatibleBitmap(_textureHdc, iSizeX, iSizeY);
	(HBITMAP)::SelectObject(_flippedTextureHdc, flippedBitmap);

	StretchBlt(
		_flippedTextureHdc,
		0, 0, iSizeX, iSizeY,
		_textureHdc,
		iSizeX - 1, 0, -iSizeX, iSizeY,
		SRCCOPY
	);

	DeleteObject(flippedBitmap);
}

void Texture::LoadSprite(wstring filePath, int32 tileSizeX, int32 tileSizeY, int32 frameCount, float duration, bool loop, Vector2 offset, int32 alpha)
{
	Load(filePath, offset, alpha);

	iFrameSizeX = tileSizeX;
	iFrameSizeY = tileSizeY;
	iFrameCount = frameCount;
	fDuration = duration;
	bLoop = loop;
}

void Texture::Render(HDC hdc, Vector2 pos)
{
	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = iAlpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc,
		(int32)pos.x + vOffset.x - (iSizeX * 0.5f) + vOffset.x,
		(int32)pos.y + vOffset.y - (iSizeY * 0.5f) + vOffset.y,
		iSizeX,
		iSizeY,
		_textureHdc,
		(int32)0,
		(int32)0,
		iSizeX,
		iSizeY,
		op);
}

void Texture::RenderStretched(HDC hdc, Vector2 pos, int32 stretchWidth, int32 stretchHeight)
{
	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = iAlpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc,
		(int32)pos.x - (stretchWidth * 0.5f) + vOffset.x,
		(int32)pos.y - (stretchHeight * 0.5f) + vOffset.y,
		stretchWidth,
		stretchHeight,
		_textureHdc,
		(int32)0,
		(int32)0,
		iSizeX,
		iSizeY,
		op);
}

void Texture::RenderStretchedSprite(HDC hdc, Vector2 pos, int32 magnification, bool isFlipped)
{
	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = iAlpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	int32 stretchedWidth = iFrameSizeX * magnification;
	int32 stretchedHeight = iFrameSizeY * magnification;

	if (isFlipped)
	{
		AlphaBlend(hdc,
			(int32)pos.x - (stretchedWidth * 0.5f) + vOffset.x,
			(int32)pos.y - (stretchedHeight * 0.5f) + vOffset.y,
			stretchedWidth,
			stretchedHeight,
			_flippedTextureHdc,
			(iFrameCount - iCurFrame - 1) * iFrameSizeX,
			0,
			iFrameSizeX,
			iFrameSizeY,
			op);
	}
	else
	{
		AlphaBlend(hdc,
			(int32)pos.x - (stretchedWidth * 0.5f) + vOffset.x,
			(int32)pos.y - (stretchedHeight * 0.5f) + vOffset.y,
			stretchedWidth,
			stretchedHeight,
			_textureHdc,
			iCurFrame * iFrameSizeX,
			0,
			iFrameSizeX,
			iFrameSizeY,
			op);
	}
}

void Texture::RenderSprite(HDC hdc, Vector2 pos, int32 tileSizeX, int32 tileSizeY, int32 originX, int32 originY)
{
	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = iAlpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	AlphaBlend(hdc,
		(int32)pos.x - (tileSizeX * 0.5f) + vOffset.x,
		(int32)pos.y - (tileSizeY * 0.5f) + vOffset.y,
		tileSizeX,
		tileSizeY,
		_textureHdc,
		originX,
		originY,
		tileSizeX,
		tileSizeY,
		op);
}

void Texture::RenderRotatedSprite(HDC hdc, Vector2 pos, float radian, float scale, int32 currentFrame, bool isFlipped)
{
	BLENDFUNCTION op;
	op.BlendOp = AC_SRC_OVER;
	op.BlendFlags = 0;
	op.SourceConstantAlpha = iAlpha;
	op.AlphaFormat = AC_SRC_ALPHA;

	RECT bounds = GetRotatedBounds(pos.x, pos.y, radian, iFrameSizeX, iFrameSizeY);

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
	CalculateRotatedPoints(points, (float)(pos.x - bounds.left), (float)(pos.y - bounds.top), radian, iFrameSizeX, iFrameSizeY);

	if (isFlipped)
	{
		PlgBlt(rotatedDC, points, _flippedTextureHdc, (iFrameCount - currentFrame - 1) * iFrameSizeX, 0, iFrameSizeX, iFrameSizeY, nullptr, 0, 0);
	}
	else
	{
		PlgBlt(rotatedDC, points, _textureHdc, currentFrame * iFrameSizeX, 0, iFrameSizeX, iFrameSizeY, nullptr, 0, 0);
	}

	AlphaBlend(hdc,
		bounds.left - (tempWidth * (scale - 1.f) / 2.f) + vOffset.x,
		bounds.top - (tempHeight * (scale - 1.f) / 2.f) + vOffset.y,
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

void Texture::CalculateRotatedPoints(POINT points[3], float centerX, float centerY, float radian, int imgWidth, int imgHeight)
{
	//float radian = angle * PI / 180.f;

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

RECT Texture::GetRotatedBounds(int32 centerX, int32 centerY, float radian, int32 sizeX, int32 sizeY)
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