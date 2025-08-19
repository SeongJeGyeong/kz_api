#include "pch.h"
#include "Defines.h"

float RadianToDegree(float radian)
{
	float degree = radian * (180.0f / std::numbers::pi_v<float>);

	degree = fmodf(degree, 360.0f);
	if (degree < 0.0f)
		degree += 360.0f;

	return degree;
}

float DegreeToRadian(float degree)
{
	float radian = degree * (std::numbers::pi_v<float> / 180.0f);

	radian = fmodf(radian, std::numbers::pi_v<float> *2);
	if (radian < 0.0f)
		radian += std::numbers::pi_v<float> *2;

	return radian;
}

bool IsInPoint(RECT rect, POINT pos)
{
	if (pos.x < rect.left)
		return false;

	if (pos.x > rect.right)
		return false;

	if (pos.y < rect.top)
		return false;

	if (pos.y > rect.bottom)
		return false;
	return true;
}

bool LineIntersectsAABB(Vector2 p0, Vector2 p1, const RECT& rect, Vector2& outNormal, Vector2& outPos, float& t)
{
	Vector2 dir = p1 - p0;

	float tMin = 0.0f;
	float tMax = 1.0f;
	Vector2 normal;

	// x�� �˻�
	if (dir.x != 0)
	{
		float tx1 = (rect.left - p0.x) / dir.x;
		float tx2 = (rect.right - p0.x) / dir.x;

		Vector2 nx = (tx1 < tx2) ? Vector2(-1, 0) : Vector2(1, 0);
		if (tx1 > tx2) std::swap(tx1, tx2);

		if (tx1 > tMin)
		{
			normal = nx;
		}

		tMin = max(tMin, tx1);
		tMax = min(tMax, tx2);

		if (tMin > tMax)
			return false; // �浹 �� ��
	}
	else if (p0.x < rect.left || p0.x > rect.right)
	{
		return false; // Y������ �����ϰ� �ۿ� ����
	}

	// y�� �˻�
	if (dir.y != 0)
	{
		float ty1 = (rect.top - p0.y) / dir.y;
		float ty2 = (rect.bottom - p0.y) / dir.y;

		// ���� �������κ��� �������ִ� �Ÿ� ����
		// ty1 �� ty2���� �۴ٸ�, ������ �Ʒ� �����̴�.
		Vector2 ny = (ty1 < ty2) ? Vector2(0, -1) : Vector2(0, 1);

		if (ty1 > ty2) std::swap(ty1, ty2);

		if (ty1 > tMin)
		{
			normal = ny;
		}

		tMin = max(tMin, ty1);
		tMax = min(tMax, ty2);

		if (tMin > tMax)
			return false; // �浹 �� ��
	}
	else if (p0.y < rect.top || p0.y > rect.bottom)
	{
		return false;
	}

	// ������ �簢�� ���ο� �ִ� ��� ����ó��
	// ������ ������⿡ ���� �� �������� ���ݴ� ������ �������� normal �� �������ش�.
	if (normal == Vector2(0, 0))
	{
		if (fabs(dir.x) > fabs(dir.y))
		{
			normal = (dir.x > 0) ? Vector2(-1, 0) : Vector2(1, 0);
		}
		else
		{
			normal = (dir.y > 0) ? Vector2(0, -1) : Vector2(0, 1);
		}
	}

	//if (tMin > tMax || tMin > 1.0f || tMin < 0.0f)
	//    return false;

	// �浹 ��� ����
	outPos = p0 + dir * tMin;

	// ����ó�� �ϳ���.
	outPos += normal * 0.01f; // �ణ�� �������� �־� ��Ȯ�� �浹 ��ġ�� ����

	outNormal = normal;

	t = tMin;

	return true;
}

bool LineIntersectsLine(Vector2 p0, Vector2 p1, Vector2 q0, Vector2 q1, Vector2& outNormal, Vector2& outPos, float& t)
{
	Vector2 playerDir = p1 - p0;
	Vector2 colliderDir = q1 - q0;

	float cross = playerDir.x * colliderDir.y - playerDir.y * colliderDir.x;
	// 1e-6f : 0.000001
	// �� ������ ���� ������ ���
	if (fabs(cross) < 1e-6f) return false;

	Vector2 diff = q0 - p0;
	float t1 = (diff.x * colliderDir.y - diff.y * colliderDir.x) / cross; // �÷��̾� ���ο����� ���� ����
	float t2 = (diff.x * playerDir.y - diff.y * playerDir.x) / cross;	// �浹ü ���ο����� ���� ����

	if (t1 >= 0.0f && t1 <= 1.0f && t2 >= 0.0f && t2 <= 1.0f)
	{
		// ���� ���
		outPos = p0 + playerDir * t1;

		// ���� ���� ��� (���� �浹ü�� ���� ����)
	   // ������ ���⿡ �����̰�, �÷��̾ ���� ������ ���ϵ���
		Vector2 perpendicular = Vector2(-colliderDir.y, colliderDir.x).GetNormalize();

		// �÷��̾ ������ ��� �ʿ��� �����ϴ��� Ȯ��
		Vector2 playerDir = (p1 - p0).GetNormalize();
		if (perpendicular.Dot(playerDir) > 0)
			perpendicular = Vector2(colliderDir.y, -colliderDir.x).GetNormalize();

		outNormal = perpendicular;
		t = t1;
		outPos += outNormal * 0.01f; // �ణ�� �������� �־� ��Ȯ�� �浹 ��ġ�� ����

		return true;
	}

	return false;
}

void GetRotatedCorners(POINT outPoints[4], float centerX, float centerY, float radian, int imgWidth, int imgHeight)
{
	float cosA = cos(radian);
	float sinA = sin(radian);

	float halfW = imgWidth / 2.0f;
	float halfH = imgHeight / 2.0f;

	// �� �𼭸� (�»��, ����, ���ϴ�, ���ϴ�)
	float corners[4][2] = {
		{-halfW, -halfH},
		{ halfW, -halfH},
		{ halfW,  halfH},
		{-halfW,  halfH}
	};

	for (int i = 0; i < 4; i++)
	{
		float x = corners[i][0] * cosA - corners[i][1] * sinA;
		float y = corners[i][0] * sinA + corners[i][1] * cosA;

		outPoints[i].x = (LONG)(centerX + x);
		outPoints[i].y = (LONG)(centerY + y);
	}
}