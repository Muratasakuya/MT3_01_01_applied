#pragma once
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

/// <summary>
/// 三次元ベクトル
/// </summary>
class Vec3f {
public:
	float x, y, z;

	// コンストラクタ
	Vec3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) : x(_x), y(_y), z(_z) {}

	// 加算演算子のオーバーロード
	Vec3f operator+(const Vec3f& other) const {
		return Vec3f(x + other.x, y + other.y, z + other.z);
	}

	// 減算演算子のオーバーロード
	Vec3f operator-(const Vec3f& other) const {
		return Vec3f(x - other.x, y - other.y, z - other.z);
	}

	// スカラー倍の演算子のオーバーロード
	Vec3f operator*(float scalar) const {
		return Vec3f(x * scalar, y * scalar, z * scalar);
	}

	// スカラー倍の演算子（右辺側）のオーバーロード
	friend Vec3f operator*(float scalar, const Vec3f& vector) {
		return Vec3f(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}
};