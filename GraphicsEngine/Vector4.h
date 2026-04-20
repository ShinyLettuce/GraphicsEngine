#pragma once
#include <cassert>
#include <cmath>

template <typename T>
class Vector4
{
public:
	T x;
	T y;
	T z;
	T w;

	Vector4();
	Vector4(const T& aX, const T& aY, const T& aZ, const T& aW);
	Vector4(const Vector4<T>& aVector) = default;
	Vector4<T>& operator=(const Vector4<T>& aVector4) = default;
	~Vector4() = default;

	template<class TargetType>
	Vector4<TargetType> ToType() const;
	Vector4<T> operator-() const;
	T LengthSqr() const;
	T Length() const;
	Vector4<T> GetNormalized() const;
	void Normalize();
	T Dot(const Vector4<T>& aVector) const;
};
template<typename T>
inline Vector4<T>::Vector4() :
	x(0),
	y(0),
	z(0),
	w(0)
{
}
template<typename T>
inline Vector4<T>::Vector4(const T& aX, const T& aY, const T& aZ, const T& aW) :
	x(aX),
	y(aY),
	z(aZ),
	w(aW)
{
}
template<typename T>
template<class TargetType>
inline Vector4<TargetType> Vector4<T>::ToType() const
{
	return Vector4<TargetType>(static_cast<TargetType>(x), static_cast<TargetType>(y), static_cast<TargetType>(z), static_cast<TargetType>(w));
}
template<typename T>
inline Vector4<T> Vector4<T>::operator-() const
{
	return Vector4<T>(-x, -y, -z, -w);
}
template<typename T>
inline T Vector4<T>::LengthSqr() const
{
	return x * x + y * y + z * z + w * w;
}
template<typename T>
inline T Vector4<T>::Length() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}
template<typename T>
inline Vector4<T> Vector4<T>::GetNormalized() const
{
	T length = Length();
	if (length == 0)
	{
		return *this;
	}
	return Vector4<T>(x / length, y / length, z / length, w / length);
}
template<typename T>
inline void Vector4<T>::Normalize()
{
	T length = Length();
	if (length == 0)
	{
		return;
	}
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}
template<typename T>
inline T Vector4<T>::Dot(const Vector4<T>& aVector) const
{
	return x * aVector.x + y * aVector.y + z * aVector.z + w * aVector.w;
}

template <typename T>
Vector4<T> operator+(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z, aVector0.w + aVector1.w);
}
template <typename T>
Vector4<T> operator-(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z, aVector0.w - aVector1.w);
}
template <typename T>
Vector4<T> operator*(const Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	return Vector4<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z, aVector0.w * aVector1.w);
}
template <typename T>
Vector4<T> operator*(const Vector4<T>& aVector, const T& aScalar)
{
	return Vector4<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar, aVector.w * aScalar);
}
template <typename T>
Vector4<T> operator*(const T& aScalar, const Vector4<T>& aVector)
{
	return aVector * aScalar;
}
template <typename T>
Vector4<T> operator/(const Vector4<T>& aVector, const T& aScalar)
{
	assert(aScalar != 0);
	return Vector4<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar, aVector.w / aScalar);
}
template <typename T>
void operator+=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
	aVector0.z = aVector0.z + aVector1.z;
	aVector0.w = aVector0.w + aVector1.w;
}
template <typename T>
void operator-=(Vector4<T>& aVector0, const Vector4<T>& aVector1)
{
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
	aVector0.z = aVector0.z - aVector1.z;
	aVector0.w = aVector0.w - aVector1.w;
}
template <typename T>
void operator*=(Vector4<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
	aVector.z = aVector.z * aScalar;
	aVector.w = aVector.w * aScalar;
}
template <typename T>
void operator /=(Vector4<T>& aVector, const T& aScalar)
{
	assert(aScalar != 0);
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
	aVector.z = aVector.z / aScalar;
	aVector.w = aVector.w / aScalar;
}

