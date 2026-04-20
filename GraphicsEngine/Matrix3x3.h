#pragma once
#include <cmath>

template<typename T>
class Matrix4x4;

template<typename T>
class Vector3;

template <typename T>
class Matrix3x3
{
public:
	Matrix3x3<T>();
	Matrix3x3<T>(
		const T& a11, const T& a12, const T& a13,
		const T& a21, const T& a22, const T& a23,
		const T& a31, const T& a32, const T& a33);
	Matrix3x3<T>(const Matrix3x3<T>& aMatrix) = default;
	Matrix3x3<T>(const Matrix4x4<T>& aMatrix);

	Matrix3x3<T>& operator=(const Matrix3x3<T>& aMatrix) = default;

	Matrix3x3<T> operator+(const Matrix3x3<T>& aAddend) const;
	Matrix3x3<T>& operator+=(const Matrix3x3<T>& aAddend);
	Matrix3x3<T> operator-(const Matrix3x3<T>& aSubtrahend) const;
	Matrix3x3<T>& operator-=(const Matrix3x3<T>& aSubtrahend);
	Matrix3x3<T> operator*(const Matrix3x3<T>& aFactor) const;
	Matrix3x3<T>& operator*=(const Matrix3x3<T>& aFactor);
	bool operator==(const Matrix3x3<T>& aComparitor) const;

	T& operator()(const int aRow, const int aColumn);
	const T& operator()(const int aRow, const int aColumn) const;

	Matrix3x3<T> GetTranspose() const;

	static Matrix3x3<T> CreateRotationAroundX(const T aAngleInRadians);
	static Matrix3x3<T> CreateRotationAroundY(const T aAngleInRadians);
	static Matrix3x3<T> CreateRotationAroundZ(const T aAngleInRadians);
private:
	union
	{
		T linear[9];
		T nonLinear[3][3];
	};
};

template<typename T>
inline Matrix3x3<T>::Matrix3x3()
	: linear{ T{1},T{0},T{0},
				T{0},T{1},T{0},
				T{0},T{0},T{1} }
{
}

template<typename T>
inline Matrix3x3<T>::Matrix3x3(
	const T& a11, const T& a12, const T& a13,
	const T& a21, const T& a22, const T& a23,
	const T& a31, const T& a32, const T& a33)
	: linear{ a11, a12, a13,
				a21, a22, a23,
				a31, a32, a33 }
{
}

template<typename T>
inline Matrix3x3<T>::Matrix3x3(const Matrix4x4<T>& aMatrix)
	: linear{ aMatrix(1,1), aMatrix(1,2), aMatrix(1,3),
				aMatrix(2,1), aMatrix(2,2), aMatrix(2,3),
				aMatrix(3,1), aMatrix(3,2), aMatrix(3,3) }
{
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::operator+(const Matrix3x3<T>& aAddend) const
{
	Matrix3x3 sum(*this);

	for (int i = 0; i < 9; ++i)
	{
		sum.linear[i] += aAddend.linear[i];
	}
	return sum;
}
template<typename T>
inline Matrix3x3<T>& Matrix3x3<T>::operator+=(const Matrix3x3<T>& aAddend)
{
	*this = *this + aAddend;
	return *this;
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::operator-(const Matrix3x3<T>& aSubtrahend) const
{
	Matrix3x3 difference(*this);

	for (int i = 0; i < 9; ++i)
	{
		difference.linear[i] -= aSubtrahend.linear[i];
	}
	return difference;
}
template<typename T>
inline Matrix3x3<T>& Matrix3x3<T>::operator-=(const Matrix3x3<T>& aSubtrahend)
{
	*this = *this - aSubtrahend;
	return *this;
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::operator*(const Matrix3x3<T>& aFactor) const
{
	Matrix3x3 product;

	product.nonLinear[0][0] = nonLinear[0][0] * aFactor.nonLinear[0][0] + nonLinear[0][1] * aFactor.nonLinear[1][0] + nonLinear[0][2] * aFactor.nonLinear[2][0];
	product.nonLinear[0][1] = nonLinear[0][0] * aFactor.nonLinear[0][1] + nonLinear[0][1] * aFactor.nonLinear[1][1] + nonLinear[0][2] * aFactor.nonLinear[2][1];
	product.nonLinear[0][2] = nonLinear[0][0] * aFactor.nonLinear[0][2] + nonLinear[0][1] * aFactor.nonLinear[1][2] + nonLinear[0][2] * aFactor.nonLinear[2][2];

	product.nonLinear[1][0] = nonLinear[1][0] * aFactor.nonLinear[0][0] + nonLinear[1][1] * aFactor.nonLinear[1][0] + nonLinear[1][2] * aFactor.nonLinear[2][0];
	product.nonLinear[1][1] = nonLinear[1][0] * aFactor.nonLinear[0][1] + nonLinear[1][1] * aFactor.nonLinear[1][1] + nonLinear[1][2] * aFactor.nonLinear[2][1];
	product.nonLinear[1][2] = nonLinear[1][0] * aFactor.nonLinear[0][2] + nonLinear[1][1] * aFactor.nonLinear[1][2] + nonLinear[1][2] * aFactor.nonLinear[2][2];

	product.nonLinear[2][0] = nonLinear[2][0] * aFactor.nonLinear[0][0] + nonLinear[2][1] * aFactor.nonLinear[1][0] + nonLinear[2][2] * aFactor.nonLinear[2][0];
	product.nonLinear[2][1] = nonLinear[2][0] * aFactor.nonLinear[0][1] + nonLinear[2][1] * aFactor.nonLinear[1][1] + nonLinear[2][2] * aFactor.nonLinear[2][1];
	product.nonLinear[2][2] = nonLinear[2][0] * aFactor.nonLinear[0][2] + nonLinear[2][1] * aFactor.nonLinear[1][2] + nonLinear[2][2] * aFactor.nonLinear[2][2];

	return product;

}
template<typename T>
inline Matrix3x3<T>& Matrix3x3<T>::operator*=(const Matrix3x3<T>& aFactor)
{
	*this = *this * aFactor;
	return *this;
}
template<typename T>
inline Vector3<T> operator*(const Vector3<T>& aFactorOne, const Matrix3x3<T>& aFactorTwo)
{
	const float x = aFactorTwo(1, 1) * aFactorOne.x + aFactorTwo(2, 1) * aFactorOne.y + aFactorTwo(3, 1) * aFactorOne.z;
	const float y = aFactorTwo(1, 2) * aFactorOne.x + aFactorTwo(2, 2) * aFactorOne.y + aFactorTwo(3, 2) * aFactorOne.z;
	const float z = aFactorTwo(1, 3) * aFactorOne.x + aFactorTwo(2, 3) * aFactorOne.y + aFactorTwo(3, 3) * aFactorOne.z;

	return Vector3<T>(x, y, z);
}
template<typename T>
inline bool Matrix3x3<T>::operator==(const Matrix3x3<T>& aComparitor) const
{
	for (int i = 0; i < 9; ++i)
	{
		if (aComparitor.linear[i] != linear[i])
		{
			return false;
		}
	}
	return true;
}
template<typename T>
inline T& Matrix3x3<T>::operator()(const int aRow, const int aColumn)
{
	return nonLinear[aRow - 1][aColumn - 1];
}
template<typename T>
inline const T& Matrix3x3<T>::operator()(const int aRow, const int aColumn) const
{
	return nonLinear[aRow - 1][aColumn - 1];
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::GetTranspose() const
{
	return Matrix3x3<T>(
		linear[0], linear[3], linear[6],
		linear[1], linear[4], linear[7],
		linear[2], linear[5], linear[8]);
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundX(const T aAngleInRadians)
{
	return Matrix3x3<T>(
		T{ 1 }, T{ 0 }, T{ 0 },
		T{ 0 }, std::cos(aAngleInRadians), std::sin(aAngleInRadians),
		T{ 0 }, -std::sin(aAngleInRadians), std::cos(aAngleInRadians));
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundY(const T aAngleInRadians)
{
	return Matrix3x3<T>(
		std::cos(aAngleInRadians), T{ 0 }, -std::sin(aAngleInRadians),
		T{ 0 }, T{ 1 }, T{ 0 },
		std::sin(aAngleInRadians), T{ 0 }, std::cos(aAngleInRadians));
}
template<typename T>
inline Matrix3x3<T> Matrix3x3<T>::CreateRotationAroundZ(const T aAngleInRadians)
{
	return Matrix3x3<T>(
		std::cos(aAngleInRadians), std::sin(aAngleInRadians), T{ 0 },
		-std::sin(aAngleInRadians), std::cos(aAngleInRadians), T{ 0 },
		T{ 0 }, T{ 0 }, T{ 1 });
}
