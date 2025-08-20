#include "pch.h"
#include "OBBCollider.h"
#include "../Game/Game.h"

void OBBCollider::InitOBB(float width, float height, float rotation)
{
	fWidth = width;
	fHeight = height;
	fRotation = rotation;
}

void OBBCollider::UpdateComponent(float deltaTime)
{
	Super::UpdateComponent(deltaTime);
}

void OBBCollider::RenderComponent(HDC hdc)
{
	if (!Game::GetInstance()->GetDebugMode()) return;
	Super::RenderComponent(hdc);

	pair<Vector2, Vector2> axes = GetAxes();

	vector<Vector2> vertices = GetVertices();
	POINT points[4];
	for (int i = 0; i < 4; ++i)
	{
		points[i].x = vertices[i].x;
		points[i].y = vertices[i].y;
	}

	HPEN pen = CreatePen(PS_SOLID, 1, bOverlapped ? RGB(0, 0, 255) : RGB(255, 0, 0));
	HPEN oldPen = (HPEN)SelectObject(hdc, pen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
	Polygon(hdc, points, 4);

	MoveToEx(hdc, GetPos().x, GetPos().y, nullptr);
	LineTo(hdc, GetPos().x + axes.first.x * 20, GetPos().y + axes.first.y * 20);

	MoveToEx(hdc, GetPos().x, GetPos().y, nullptr);
	LineTo(hdc, GetPos().x + axes.second.x * 20, GetPos().y + axes.second.y * 20);

	SelectObject(hdc, oldPen);
	SelectObject(hdc, oldBrush);
	DeleteObject(pen);
}

// x, y �� ���, ���� ���� ��ȯ
pair<Vector2, Vector2> OBBCollider::GetAxes() const
{
	float cosRotation = cosf(fRotation);
	float sinRotation = sinf(fRotation);

	return { Vector2(cosRotation, sinRotation),	// X�� ����
			Vector2(-sinRotation, cosRotation)};	// Y�� ����(X���� �ݽð�������� 90�� ȸ��)
}

// �� ���� ��ġ ���
vector<Vector2> OBBCollider::GetVertices() const
{
	float halfWidth = fWidth / 2;
	float halfHeight = fHeight / 2;
	Vector2 center = GetPos();

	Vector2 topL = Vector2(center.x - halfWidth, center.y - halfHeight);
	Vector2 topR = Vector2(center.x + halfWidth, center.y - halfHeight);
	Vector2 bottomL = Vector2(center.x - halfWidth, center.y + halfHeight);
	Vector2 bottomR = Vector2(center.x + halfWidth, center.y + halfHeight);

	return vector<Vector2> { topL.Rotate(fRotation), bottomL.Rotate(fRotation), bottomR.Rotate(fRotation), topR.Rotate(fRotation) };
}

// �� ����
pair<float, float> OBBCollider::ProjectionToAxis(const Vector2& axis) const
{
	// �� ������ �浹�ߴٸ�, �� ������ ������ �����ڸ��� ������ ��� ���Ϳ� �� ������ ������ ���͵��� ��ġ�� �ȴ�.
	vector<Vector2> vertices = GetVertices();
	float mn = vertices[0].Dot(axis);
	float mx = mn;

	for (int32 i = 1; i < vertices.size(); ++i)
	{
		float projection = vertices[i].Dot(axis);
		mn = min(mn, projection);
		mx = max(mx, projection);
	}

	return { mn, mx };
}