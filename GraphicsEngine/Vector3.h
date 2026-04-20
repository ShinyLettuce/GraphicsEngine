#pragma once
#include <cassert>
#include <cmath>

template <typename T>
class Vector3
{
public:
	T x;
	T y;
	T z;

	Vector3();
	Vector3(const T& aX, const T& aY, const T& aZ);
	Vector3(const Vector3<T>& aVector) = default;
	Vector3<T>& operator=(const Vector3<T>& aVector3) = default;
	~Vector3() = default;

	template<class TargetType>
	Vector3<TargetType> ToType() const;
	Vector3<T> operator-() const;
	T LengthSqr() const;
	T Length() const;
	Vector3<T> GetNormalized() const;
	void Normalize();
	T Dot(const Vector3<T>& aVector) const;
	Vector3<T> Cross(const Vector3<T>& aVector) const;
};
template<typename T>
inline Vector3<T>::Vector3() :
	x(0),
	y(0),
	z(0)
{

}
template<typename T>
inline Vector3<T>::Vector3(const T& aX, const T& aY, const T& aZ) :
	x(aX),
	y(aY),
	z(aZ)
{
}
template<typename T>
template<class TargetType>
inline Vector3<TargetType> Vector3<T>::ToType() const
{
	return Vector3<TargetType>(static_cast<TargetType>(x), static_cast<TargetType>(y), static_cast<TargetType>(z));
}
template<typename T>
inline Vector3<T> Vector3<T>::operator-() const
{
	return Vector3<T>(-x, -y, -z);
}
template<typename T>
inline T Vector3<T>::LengthSqr() const
{
	return x * x + y * y + z * z;
}
template<typename T>
inline T Vector3<T>::Length() const
{
	return sqrt(x * x + y * y + z * z);
}
template<typename T>
inline Vector3<T> Vector3<T>::GetNormalized() const
{
	T length = Length();
	if (length == 0)
	{
		return *this;
	}
	return Vector3<T>(x / length, y / length, z / length);
}
template<typename T>
inline void Vector3<T>::Normalize()
{
	T length = Length();
	if (length == 0)
	{
		return;
	}
	x /= length;
	y /= length;
	z /= length;
}
template<typename T>
inline T Vector3<T>::Dot(const Vector3<T>& aVector) const
{
	return x * aVector.x + y * aVector.y + z * aVector.z;
}

template<typename T>
inline Vector3<T> Vector3<T>::Cross(const Vector3<T>& aVector) const
{
	return Vector3<T>(y * aVector.z - z * aVector.y, z * aVector.x - x * aVector.z, x * aVector.y - y * aVector.x);
}

template <typename T>
Vector3<T> operator+(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y, aVector0.z + aVector1.z);
}
template <typename T>
Vector3<T> operator-(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y, aVector0.z - aVector1.z);
}
template <typename T>
Vector3<T> operator*(const Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	return Vector3<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y, aVector0.z * aVector1.z);
}
template <typename T>
Vector3<T> operator*(const Vector3<T>& aVector, const T& aScalar)
{
	return Vector3<T>(aVector.x * aScalar, aVector.y * aScalar, aVector.z * aScalar);
}
template <typename T>
Vector3<T> operator*(const T& aScalar, const Vector3<T>& aVector)
{
	return aVector * aScalar;
}
template <typename T>
Vector3<T> operator/(const Vector3<T>& aVector, const T& aScalar)
{
	assert(aScalar != 0);
	return Vector3<T>(aVector.x / aScalar, aVector.y / aScalar, aVector.z / aScalar);
}
template <typename T>
void operator+=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
	aVector0.z = aVector0.z + aVector1.z;
}
template <typename T>
void operator-=(Vector3<T>& aVector0, const Vector3<T>& aVector1)
{
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
	aVector0.z = aVector0.z - aVector1.z;
}
template <typename T>
void operator*=(Vector3<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
	aVector.z = aVector.z * aScalar;
}
template <typename T>
void operator /=(Vector3<T>& aVector, const T& aScalar)
{
	assert(aScalar != 0);
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
	aVector.z = aVector.z / aScalar;
}
