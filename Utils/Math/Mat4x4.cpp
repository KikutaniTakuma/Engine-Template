#include "Mat4x4.h"
#include "Vector3.h"
#include <cmath>
#include <algorithm>
#include <Windows.h>
#include <immintrin.h>

const Mat4x4 Mat4x4::kIdentity_ = Mat4x4{
	std::array<Vector4, 4>{
		Vector4{1.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 1.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 1.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 1.0f}
	}
};

const Mat4x4 Mat4x4::kZero_ = Mat4x4{
	std::array<Vector4, 4>{
		Vector4{0.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 0.0f},
		Vector4{0.0f, 0.0f, 0.0f, 0.0f}
	}
};

Mat4x4::Mat4x4()
	:m()
{}

Mat4x4::Mat4x4(const Mat4x4& mat) {
	*this = mat;
}

Mat4x4::Mat4x4(Mat4x4&& mat) noexcept {
	*this = std::move(mat);
}

Mat4x4::Mat4x4(const std::array<Vector4, 4>& num) {
	m = num;
}

Mat4x4& Mat4x4::operator=(const Mat4x4& mat) {
	std::copy(mat.m.begin(), mat.m.end(), m.begin());

	return *this;
}

Mat4x4& Mat4x4::operator=(Mat4x4&& mat) noexcept {
	m = std::move(mat.m);

	return *this;
}

Mat4x4 Mat4x4::operator*(const Mat4x4& mat) const {
	Mat4x4 result;

	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			for (int i = 0; i < Mat4x4::WIDTH; i++) {
				result.m[y][x] += this->m[y][i] * mat.m[i][x];
			}
		}
	}

	return result;
}

Mat4x4& Mat4x4::operator*=(const Mat4x4& mat) {
	*this = *this * mat;

	return *this;
}

Mat4x4 Mat4x4::operator+(const Mat4x4& mat) const {
	Mat4x4 tmp;

	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			tmp[y][x] = this->m[y][x] + mat.m[y][x];
		}
	}

	return tmp;
}
Mat4x4& Mat4x4::operator+=(const Mat4x4& mat) {
	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			this->m[y][x] += mat.m[y][x];
		}
	}

	return *this;
}
Mat4x4 Mat4x4::operator-(const Mat4x4& mat) const {
	Mat4x4 tmp;

	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			tmp[y][x] = this->m[y][x] - mat.m[y][x];
		}
	}

	return tmp;
}
Mat4x4& Mat4x4::operator-=(const Mat4x4& mat) {
	for (int y = 0; y < Mat4x4::HEIGHT; y++) {
		for (int x = 0; x < Mat4x4::WIDTH; x++) {
			this->m[y][x] -= mat.m[y][x];
		}
	}

	return *this;
}

Vector4& Mat4x4::operator[](size_t index) {
	return m[index];
}

const Vector4& Mat4x4::operator[](size_t index) const {
	return m[index];
}

bool Mat4x4::operator==(const Mat4x4& mat) const {
	return m == mat.m;
}

bool Mat4x4::operator!=(const Mat4x4& mat) const {
	return m != mat.m;
}

const Mat4x4& Mat4x4::Identity() {
	*this = Mat4x4::kIdentity_;

	return *this;
}

const Mat4x4& Mat4x4::Translate(const Vector3& vec) {
	this->m = {};

	this->m[0][0] = 1.0f;
	this->m[1][1] = 1.0f;
	this->m[2][2] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[3][0] = vec.x;
	this->m[3][1] = vec.y;
	this->m[3][2] = vec.z;

	return *this;
}

const Mat4x4& Mat4x4::Scalar(const Vector3& vec) {
	this->m = {};

	this->m[0][0] = vec.x;
	this->m[1][1] = vec.y;
	this->m[2][2] = vec.z;
	this->m[3][3] = 1.0f;

	return *this;
}

const Mat4x4& Mat4x4::RotateX(float rad) {
	this->m = {};
	this->m[0][0] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[1][1] = std::cos(rad);
	this->m[1][2] = std::sin(rad);
	this->m[2][1] = -std::sin(rad);
	this->m[2][2] = std::cos(rad);

	return *this;
}

const Mat4x4& Mat4x4::RotateY(float rad) {
	this->m = {};
	this->m[1][1] = 1.0f;
	this->m[3][3] = 1.0f;

	this->m[0][0] = std::cos(rad);
	this->m[0][2] = -std::sin(rad);
	this->m[2][0] = std::sin(rad);
	this->m[2][2] = std::cos(rad);

	return *this;
}

const Mat4x4& Mat4x4::RotateZ(float rad) {
	this->m = {};
	this->m[2][2] = 1.0f;
	this->m[3][3] = 1.0f;
	
	this->m[0][0] = std::cos(rad);
	this->m[0][1] = -std::sin(rad);
	this->m[1][0] = std::sin(rad);
	this->m[1][1] = std::cos(rad);

	return *this;
}

const Mat4x4& Mat4x4::Affin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 rotate = MakeMatrixRotateX(rad.x) * MakeMatrixRotateY(rad.y) * MakeMatrixRotateZ(rad.z);

	*this = Mat4x4{ 
		std::array<Vector4, 4>{
		Vector4{scale.x * rotate.m[0][0], scale.x * rotate.m[0][1],scale.x * rotate.m[0][2], 0.0f},
		Vector4{scale.y * rotate.m[1][0], scale.y * rotate.m[1][1],scale.y * rotate.m[1][2], 0.0f },
		Vector4{scale.z * rotate.m[2][0], scale.z * rotate.m[2][1],scale.z * rotate.m[2][2], 0.0f},
		Vector4{translate.x, translate.y, translate.z, 1.0f}
		}
	};

	return *this;
}


const Mat4x4& Mat4x4::Inverse() {
	Mat4x4 tmp = *this;

	Mat4x4 identity = Mat4x4::kIdentity_;

	float toOne = tmp[0][0];

	float tmpNum = 0.0f;

	for (int i = 0; i < Mat4x4::HEIGHT; i++) {
		if (tmp.m[i][i] == 0.0f && i < Mat4x4::HEIGHT) {
			int pibIndex = i;
			float pibot = fabsf(tmp.m[i][i]);

			for (int y = i + 1; y < Mat4x4::HEIGHT; y++) {
				if (tmp.m[y][i] != 0.0f && pibot < fabsf(tmp.m[y][i])) {
					pibot = fabsf(tmp.m[y][i]);
					pibIndex = y;
				}
			}

			if (pibot == 0.0f) {
				return *this;
			}

			tmp.m[i].m.swap(tmp.m[pibIndex].m);
			identity.m[i].m.swap(identity.m[pibIndex].m);
		}

		toOne = tmp.m[i][i];
		for (int x = 0; x < Mat4x4::HEIGHT; x++) {
			tmp.m[i][x] /= toOne;
			identity.m[i][x] /= toOne;
		}

		for (int y = 0; y < Mat4x4::HEIGHT; ++y) {
			if (i == y) {
				continue;
			}

			tmpNum = -tmp.m[y][i];
			for (int x = 0; x < Mat4x4::WIDTH; x++) {
				tmp.m[y][x] += tmpNum * tmp.m[i][x];
				identity.m[y][x] += tmpNum * identity.m[i][x];
			}
		}
	}

	if (tmp != kIdentity_) {
		return *this;
	}

	*this = identity;

	return *this;
}


const Mat4x4& Mat4x4::Transepose() {
	std::swap(m[1][0], m[0][1]);
	std::swap(m[2][0], m[0][2]);
	std::swap(m[3][0], m[0][3]);
	std::swap(m[2][1], m[1][2]);
	std::swap(m[2][3], m[3][2]);
	std::swap(m[3][1], m[1][3]);

	return *this;
}

const Mat4x4& Mat4x4::PerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	m = {};

	m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f));
	m[1][1] = 1.0f / std::tan(fovY / 2.0f);
	m[2][2] = farClip / (farClip - nearClip);
	m[2][3] = 1.0f;
	m[3][2] = (-nearClip * farClip) / (farClip - nearClip);

	return *this;
}

const Mat4x4& Mat4x4::Orthographic(float left, float top, float right, float bottom, float nearClip, float farClip) {
	m = {};

	m[0][0] = 2.0f / (right - left);
	m[1][1] = 2.0f / (top - bottom);
	m[2][2] = 1.0f / (farClip - nearClip);
	m[3][3] = 1.0f;

	m[3][0] = (left + right) / (left - right);
	m[3][1] = (top + bottom) / (bottom - top);
	m[3][2] = nearClip / (nearClip - farClip);

	return *this;
}

const Mat4x4& Mat4x4::ViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	m = {};

	m[0][0] = width / 2.0f;
	m[1][1] = height/ -2.0f;
	m[2][2] = maxDepth - minDepth;
	m[3][3] = 1.0f;

	m[3][0] = left + (width / 2.0f);
	m[3][1] = top + (height / 2.0f);
	m[3][2] = minDepth;

	return *this;
}

Mat4x4 MakeMatrixInverse(Mat4x4 mat) {
	Mat4x4 tmp = mat;
	tmp.Inverse();
	return tmp;
}

Mat4x4 MakeMatrixTransepose(Mat4x4 mat) {
	Mat4x4 tmp = mat;
	tmp.Transepose();
	return tmp;
}

Mat4x4 MakeMatrixTranslate(Vector3 vec) {
	Mat4x4 mat;

	mat.Translate(vec);

	return mat;
}

Mat4x4 MakeMatrixScalar(Vector3 vec) {
	Mat4x4 mat;

	mat.Scalar(vec);

	return mat;
}

Mat4x4 MakeMatrixRotateX(float rad) {
	Mat4x4 tmp;

	tmp.RotateX(rad);

	return tmp;
}

Mat4x4 MakeMatrixRotateY(float rad) {
	Mat4x4 tmp;

	tmp.RotateY(rad);

	return tmp;
}

Mat4x4 MakeMatrixRotateZ(float rad) {
	Mat4x4 tmp;

	tmp.RotateZ(rad);

	return tmp;
}

Mat4x4 MakeMatrixRotate(const Vector3& rad) {
	Mat4x4 tmp;

	tmp.Affin(Vector3::identity, rad, Vector3::zero);

	return tmp;
}

Mat4x4 MakeMatrixAffin(const Vector3& scale, const Vector3& rad, const Vector3& translate) {
	Mat4x4 tmp;

	tmp.Affin(scale, rad, translate);

	return tmp;
}

Mat4x4 MakeMatrixPerspectiveFov(float fovY, float aspectRatio, float nearClip, float farClip) {
	Mat4x4 tmp;

	tmp.PerspectiveFov(fovY, aspectRatio, nearClip, farClip);

	return tmp;
}

Mat4x4 MakeMatrixOrthographic(float left, float right, float top, float bottom, float nearClip, float farClip) {
	Mat4x4 tmp;

	tmp.Orthographic(left, right, top, bottom, nearClip, farClip);

	return tmp;
}

Mat4x4 MakeMatrixViewPort(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Mat4x4 tmp;

	tmp.ViewPort(left, top, width, height, minDepth, maxDepth);

	return tmp;
}

Mat4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	Vector3 normal = from.Cross(to).Normalize();
	float theataCos = from.Normalize().Dot(to.Normalize());
	float theataSin = from.Normalize().Cross(to.Normalize()).Length();

	Mat4x4 result = Mat4x4{
		std::array<Vector4, 4>{
			Vector4{ 
				normal.x * normal.x * (1.0f - theataCos) + theataCos,
				normal.x * normal.y * (1.0f - theataCos) + normal.z * theataSin,
				normal.x * normal.z * (1.0f - theataCos) - normal.y * theataSin,
				0.0f
			},
			Vector4{
				normal.y* normal.x* (1.0f - theataCos) - normal.z * theataSin,
				normal.y* normal.y* (1.0f - theataCos) + theataCos,
				normal.y* normal.z* (1.0f - theataCos) + normal.x * theataSin,
				0.0f
			},
			Vector4{
				normal.z * normal.x * (1.0f - theataCos) + normal.y * theataSin,
				normal.z * normal.y * (1.0f - theataCos) - normal.x * theataSin,
				normal.z * normal.z * (1.0f - theataCos) + theataCos,
				0.0f
			},
			Vector4::wIdy
		}
	};


	return result;
}