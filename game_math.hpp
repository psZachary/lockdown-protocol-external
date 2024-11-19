#pragma once

#include <math.h>
#include <cstdint>
#include <iostream>
#include "overlay/imgui/imgui.h"

struct color_m {
	double r, g, b, a;
	color_m(double rgba) : r(rgba), g(rgba), b(rgba), a(rgba) {}
	color_m() : r(0.f), g(0.f), b(0.f), a(0.f) {}
	color_m(double r, double g, double b, double a) : r(r), g(g), b(b), a(a) {}

	operator double* () { return &r; }
	// add static white and black
	static color_m white() { return color_m{ 255.f, 255.f, 255.f, 255.f }; }
	static color_m black() { return color_m{ 0.f, 0.f, 0.f, 255.f }; }
	static color_m grey() { return color_m{ 255.f / 2.f, 255.f / 2.f, 255.f / 2.f, 255.f }; }
};
struct color_rgb {
	double r, g, b, a;
	operator color_m() const { return color_m{ r / 255.f, g / 255.f, b / 255.f, a / 255.f }; }
};
class quat
{
public:
	double x;
	double y;
	double z;
	double w;
};
struct matrix3x4_t
{
	quat vec0{};
	quat vec1{};
	quat vec2{};
};
struct matrix4x4_t
{
	matrix4x4_t()
		: m{ { 0, 0, 0, 0 },
			 { 0, 0, 0, 0 },
			 { 0, 0, 0, 0 },
			 { 0, 0, 0, 0 } }
	{}

	matrix4x4_t(const matrix4x4_t&) = default;

	union {
		struct {
			double _11, _12, _13, _14;
			double _21, _22, _23, _24;
			double _31, _32, _33, _34;
			double _41, _42, _43, _44;
		};
		double m[4][4];
	};
};
struct vector2 {
public:
	double x, y;
	inline double magnitude() {
		return (double)sqrt(pow(x, 2) + pow(y, 2));
	}
	inline double distance(vector2 vector_to)
	{
		return sqrt(pow(x - vector_to.x, 2) + pow(y - vector_to.y, 2));
	}
	vector2 normalize() {
		vector2 ret;
		ret.x = this->x; ret.y = this->y;
		double length = sqrt(x * x + y * y);
		if (length != 0.0f) {
			ret.x /= length;
			ret.y /= length;
		}
		return ret;
	}
	vector2 absolute() {
		vector2 ret;
		ret.x = this->x; ret.y = this->y;

		ret.x = fabs(ret.x);
		ret.y = fabs(ret.y);

		return ret;
	}
	vector2 operator/ (int other) {
		auto res = vector2();
		res.x = this->x / other;
		res.y = this->y / other;
		return res;
	}
	vector2 operator* (int other) {
		auto res = vector2();
		res.x = this->x * other;
		res.y = this->y * other;
		return res;
	}
	vector2 operator* (double other) {
		auto res = vector2();
		res.x = this->x * other;
		res.y = this->y * other;
		return res;
	}
	vector2 operator- (vector2 other) {
		auto res = vector2();
		res.x = this->x - other.x;
		res.y = this->y - other.y;
		return res;
	}
	vector2 operator+(const vector2& other) const {
		vector2 res;
		res.x = this->x + other.x;
		res.y = this->y + other.y;
		return res;
	}
	vector2 operator+ (double other) {
		auto res = vector2();
		res.x = this->x + other;
		res.y = this->y + other;
		return res;
	}
	vector2 operator- (double other) {
		auto res = vector2();
		res.x = this->x - other;
		res.y = this->y - other;
		return res;
	}
	bool operator== (vector2 other) {
		if (other.x == this->x && other.y == this->y) return true;
		return false;
	}
	bool operator< (vector2 other) {
		if (this->x < other.x && this->y < other.y) return true;
		return false;
	}
};
struct vector3 {
public:
	double x, y, z;
	inline double magnitude() {
		return (double)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	}
	inline double distance(vector3 vector_to)
	{
		return sqrt(pow(x - vector_to.x, 2) + pow(y - vector_to.y, 2) + pow(z - vector_to.z, 2));
	}
	inline vector3 normalize()
	{
		double mag = magnitude();
		if (mag > 0.00001)
		{
			x = x / mag;
			y = y / mag;
			z = z / mag;
		}
		else
		{
			x = 0;
			y = 0;
			z = 0;
		}
		return *this;
	}
	operator ImVec2() const { return ImVec2(static_cast<float>(x), static_cast<float>(y)); }
	friend std::ostream& operator<<(std::ostream& os, const vector3& obj) {
		os << "x: " << obj.x << " y: " << obj.y << " z: " << obj.z;
		return os;
	}

	vector3 operator- (vector3 other) {
		auto res = vector3();
		res.x = this->x - other.x;
		res.y = this->y - other.y;
		res.z = this->z - other.z;
		return res;
	}
	vector3 operator* (double other) {
		auto res = vector3();
		res.x = this->x * other;
		res.y = this->y * other;
		res.z = this->z * other;
		return res;
	}
	vector3 operator/ (double other) {
		auto res = vector3();
		res.x = this->x / other;
		res.y = this->y / other;
		res.z = this->z / other;
		return res;
	}
	vector3 operator+ (vector3 other) {
		auto res = vector3();
		res.x = this->x + other.x;
		res.y = this->y + other.y;
		res.z = this->z + other.z;
		return res;
	}
	vector3 operator* (vector3 other) {
		auto res = vector3();
		res.x = this->x * other.x;
		res.y = this->y * other.y;
		res.z = this->z * other.z;
		return res;
	}
	inline vector3& operator+=(const vector3& v) {
		x += v.x; y += v.y; z += v.z; return *this;
	}

	inline vector3& operator+=(const double& v) {
		x += v; y += v; z += v; return *this;
	}

	inline vector3& operator*=(const double& v) {
		x *= v; y *= v; z *= v; return *this;
	}

	inline vector3& operator-=(const vector3& v) {
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}
	inline double dot(vector3 other)
	{
		return x * other.x + y * other.y + z * other.z;
	}
	// handle vector2 cast()
	operator vector2() const { return vector2{ x, y }; }
	bool operator== (double other) {
		if (other == this->x && other == this->y && other == this->z) return true;
		return false;
	}
	bool operator== (vector3 other) {
		if (other.x == this->x && other.y == this->y && other.z == this->z) return true;
		return false;
	}
	vector3 operator+ (double other) {
		auto res = vector3();
		res.x = this->x + other;
		res.y = this->y + other;
		res.z = this->z + other;
		return res;
	}
	static vector3 zero() {
		return { 0.f,0.f,0.f };
	}
};

struct box {
	vector2 position;
	double width;
	double height;

	vector2 bottom_left() const {
		return vector2{ position.x, position.y + height };
	}
	vector2 top_right() const {
		return vector2{ position.x + width, position.y };
	}
	vector2 top_left() const {
		return position;
	}
	vector2 bottom_right() const {
		return vector2{ position.x + width, position.y + height };
	}
};

static matrix4x4_t matrix_multiply(matrix4x4_t m1, matrix4x4_t m2)
{
	matrix4x4_t out{};
	out._11 = m1._11 * m2._11 + m1._12 * m2._21 + m1._13 * m2._31 + m1._14 * m2._41;
	out._12 = m1._11 * m2._12 + m1._12 * m2._22 + m1._13 * m2._32 + m1._14 * m2._42;
	out._13 = m1._11 * m2._13 + m1._12 * m2._23 + m1._13 * m2._33 + m1._14 * m2._43;
	out._14 = m1._11 * m2._14 + m1._12 * m2._24 + m1._13 * m2._34 + m1._14 * m2._44;
	out._21 = m1._21 * m2._11 + m1._22 * m2._21 + m1._23 * m2._31 + m1._24 * m2._41;
	out._22 = m1._21 * m2._12 + m1._22 * m2._22 + m1._23 * m2._32 + m1._24 * m2._42;
	out._23 = m1._21 * m2._13 + m1._22 * m2._23 + m1._23 * m2._33 + m1._24 * m2._43;
	out._24 = m1._21 * m2._14 + m1._22 * m2._24 + m1._23 * m2._34 + m1._24 * m2._44;
	out._31 = m1._31 * m2._11 + m1._32 * m2._21 + m1._33 * m2._31 + m1._34 * m2._41;
	out._32 = m1._31 * m2._12 + m1._32 * m2._22 + m1._33 * m2._32 + m1._34 * m2._42;
	out._33 = m1._31 * m2._13 + m1._32 * m2._23 + m1._33 * m2._33 + m1._34 * m2._43;
	out._34 = m1._31 * m2._14 + m1._32 * m2._24 + m1._33 * m2._34 + m1._34 * m2._44;
	out._41 = m1._41 * m2._11 + m1._42 * m2._21 + m1._43 * m2._31 + m1._44 * m2._41;
	out._42 = m1._41 * m2._12 + m1._42 * m2._22 + m1._43 * m2._32 + m1._44 * m2._42;
	out._43 = m1._41 * m2._13 + m1._42 * m2._23 + m1._43 * m2._33 + m1._44 * m2._43;
	out._44 = m1._41 * m2._14 + m1._42 * m2._24 + m1._43 * m2._34 + m1._44 * m2._44;

	return out;
}