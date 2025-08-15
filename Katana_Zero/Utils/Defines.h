#pragma once
//#include <windef.h>

constexpr int GWinSizeX = 1280;
constexpr int GWinSizeY = 720;

constexpr int SubWinSizeX = 992;
constexpr int SubWinSizeY = 492;

const int TILE_SIZE = 32;
const int TileMapWidth = 42;
const int TileMapHeight = 23;

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;
using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

#define SMALL_NUMBER			(1.e-8f)
#define PI						(3.141592f)

float RadianToDegree(float radian);
float DegreeToRadian(float degree);
bool IsInPoint(RECT rect, POINT pos);

enum ECollisionLayer
{
	PLAYER,
	ENEMY,
	GROUND,
	PLATFORM,
	WALL,
	CEILING,
	STAIR,
	PLAYER_HITBOX,
	ENEMY_HITBOX,
	END,
};

enum ECollisionResponse
{
	C_IGNORE,
	C_OVERLAP,
	C_BLOCK
};

enum EColliderType
{
	AABB,
	OBB,
	LINE
};

enum ESceneType
{
	LOBBY,
	EDITOR,
	GAME,
	NONE,
};

enum ERenderLayer
{
	BACKGROUND,
	FOREGROUND,
	ACTOR,
	LAYER_END,
};

enum EColliderMode
{
	COL_BOX,
	COL_PLATFORM,
	COL_WALL_V,
	COL_WALL_H,
	COL_STAIR,
	COL_PORTAL,
	COL_END,
};

enum EPlayerState
{
	PLAYER_IDLE,
	PLAYER_IDLE_TO_RUN,
	PLAYER_RUN,
	PLAYER_RUN_TO_IDLE,
	PLAYER_JUMP,
	PLAYER_FALL,
	PLAYER_PRECROUCH,
	PLAYER_CROUCH,
	PLAYER_POSTCROUCH,
	PLAYER_ATTACK,
	PLAYER_ROLL,

	PLAYER_HURT_BEGIN,
	PLAYER_HURT_LOOP,
	PLAYER_HURT_GROUND,
	PLAYER_HURT_RECOVER,

	PLAYER_END
};

struct ColliderInfo
{
	EColliderMode mode;
	POINT vStart;
	POINT vEnd;
};

struct SpriteInfo
{
	int32 iTileSizeX;
	int32 iTileSizeY;
	int32 iLastFrame = 0;
	int32 iCurFrame = 0;
	float dur = 0;
	bool loop = false;
};

struct Vector2
{
	// ��ǥ
	float x = 0;
	float y = 0;

	Vector2() : x(0), y(0) {}
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2(POINT pt) : x((float)pt.x), y((float)pt.y) {}

	// ��Ģ���� ����
	// +, - , *, / 
	Vector2 operator+(const Vector2& other)
	{
		Vector2 ret;
		ret.x = x + other.x;
		ret.y = y + other.y;
		return ret;
	}
	Vector2 operator-(const Vector2& other)
	{
		Vector2 ret;
		ret.x = x - other.x;
		ret.y = y - other.y;
		return ret;
	}
	Vector2 operator*(float ratio)
	{
		Vector2 ret;
		ret.x = x * ratio;
		ret.y = y * ratio;
		return ret;
	}

	void operator+=(const Vector2& other)
	{
		x += other.x;
		y += other.y;
	}
	void operator-=(const Vector2& other)
	{
		x -= other.x;
		y -= other.y;
	}
	void operator*=(float ratio)
	{
		x *= ratio;
		y *= ratio;
	}
	bool operator==(const Vector2& other) const
	{
		return (x == other.x && y == other.y);
	}

	// ������ ���� ���ϱ�
	// ���� ���캸��, Legnth, LengthSquared (���ʹ� ���� ���̸� �����ϴ� �Լ��� 2����)
	// sqrt(��Ʈ) ������ �������� ��α� ������, �� ��¥ ���̰� �ʿ��� ��찡 �ƴϸ�,
	// LengthSquared (���� ����) ������ ����ϴ°� �� ����.
	float Length()
	{
		return ::sqrt(x * x + y * y);
	}

	// ������ ���� ����
	float LengthSquared()
	{
		return (x * x + y * y);
	}

	// �ڱ��ڽ��� ����ȭ
	void Normalize()
	{
		// ������ ũ�⸦ ���ؼ�, ũ�⸸ŭ ������ ���ָ� �ȴ�.
		float length = Length();

		// �𸮾� ������ Ǯ ���¼ҽ�. �𸮾� �����ؼ� �����ص� �����ϴ�.
		//length �� 0�̰ų� ��û ������ �ִ�
		if (length < SMALL_NUMBER)
			return;	// ���� ����ȭ���ϰ� ����

		// ����ȭ.
		x /= length;
		y /= length;
	}

	// ����ȭ�� ����� ����
	Vector2 GetNormalize()
	{
		Vector2 normalize = *this;
		normalize.Normalize();
		return normalize;
	}

	// ����
	float Dot(const Vector2& other)
	{
		return x * other.x + y * other.y;
	}

	// radian ������ŭ ȸ����Ű��.
	Vector2 Rotate(float radian)
	{
		float cosA = ::cosf(radian);
		float sinA = ::sinf(radian);

		return Vector2(x * cosA - y * sinA, x * sinA + y * cosA);
	}

	// ����
	float Cross(const Vector2& other)
	{
		//2D ��, ��Į�󰪸� ��ȯ�� �Ѵ�. 
		// ���� ���Ͱ� �ʿ��Ѱ� �ƴϰ�, ��ȣ�� �Ǵ��ϴ� ��Į���� �ʿ��ϴ�.
		return x * other.y - y * other.x;
	}
};

struct TileInfo
{
	int32 iTilesetIndex;
	Vector2 vTilePos;
};

struct CollisionInfo
{
	ECollisionLayer collisionLayer;
	bool isColliding;           // �ٴڰ� �浹�ߴ���
	Vector2 collisionPoint;     // �浹 ����
	Vector2 vHitNormal;            // �ٴ��� ���� ���� (���� ����)
	float penetrationDepth;    // �ٴڿ� �Ĺ��� ����
	class Actor* groundActor;        // �浹�� �ٴ� ����
	int32 hitCorner;
};

//struct PlayerGroundCollisionResult
//{
//	ECollisionLayer collisionLayer;
//	bool isColliding;           // �ٴڰ� �浹�ߴ���
//	Vector2 collisionPoint;     // �浹 ����
//	Vector2 normal;            // �ٴ��� ���� ���� (���� ����)
//	float penetrationDepth;    // �ٴڿ� �Ĺ��� ����
//	class Actor* groundActor;        // �浹�� �ٴ� ����
//	int32 hitCorner;
//};

// ���а� �簢���� �浹üũ
bool LineIntersectsAABB(Vector2 p0, Vector2 p1, const RECT& rect, Vector2& outNormal, Vector2& outPos, float& t);
// ���а� ������ �浹üũ
bool LineIntersectsLine(Vector2 p0, Vector2 p1, Vector2 q0, Vector2 q1, Vector2& outNormal, Vector2& outPos, float& t);