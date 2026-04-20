#pragma once
#include <cassert>
#include <cmath>

template <typename T>
class Vector2
{
public:
	T x;
	T y;

	Vector2();
	Vector2(const T& aX, const T& aY);

	Vector2(const Vector2<T>& aVector) = default;
	Vector2<T>& operator=(const Vector2<T>& aVector2) = default;
	~Vector2() = default;


	template<class TargetType>
	Vector2<TargetType> ToType() const;
	Vector2<T> operator-() const;
	T LengthSqr() const;
	T Length() const;
	Vector2<T> GetNormalized() const;
	void Normalize();
	T Dot(const Vector2<T>& aVector) const;
};
template<typename T>
inline Vector2<T>::Vector2() :
	x(0),
	y(0)
{
}
template<typename T>
inline Vector2<T>::Vector2(const T& aX, const T& aY) :
	x(aX),
	y(aY)
{
}
template<typename T>
template<class TargetType>
inline Vector2<TargetType> Vector2<T>::ToType() const
{
	return Vector2<TargetType>(static_cast<TargetType>(x), static_cast<TargetType>(y));
}
template<typename T>
inline Vector2<T> Vector2<T>::operator-() const
{
	return Vector2<T>(-x, -y);
}
template<typename T>
inline T Vector2<T>::LengthSqr() const
{
	return x * x + y * y;
}
template<typename T>
inline T Vector2<T>::Length() const
{
	return sqrt(x * x + y * y);
}
template<typename T>
inline Vector2<T> Vector2<T>::GetNormalized() const
{
	T length = Length();
	if (length == 0)
	{
		return *this;
	}
	return Vector2<T>(x / length, y / length);
}
template<typename T>
inline void Vector2<T>::Normalize()
{
	T length = Length();
	if (length == 0)
	{
		return;
	}
	x /= length;
	y /= length;
}
template<typename T>
inline T Vector2<T>::Dot(const Vector2<T>& aVector) const
{
	return x * aVector.x + y * aVector.y;
}

template <typename T>
Vector2<T> operator+(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(aVector0.x + aVector1.x, aVector0.y + aVector1.y);
}
template <typename T>
Vector2<T> operator-(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(aVector0.x - aVector1.x, aVector0.y - aVector1.y);
}
template <typename T>
Vector2<T> operator*(const Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	return Vector2<T>(aVector0.x * aVector1.x, aVector0.y * aVector1.y);
}
template <typename T>
Vector2<T> operator*(const Vector2<T>& aVector, const T& aScalar)
{
	return Vector2<T>(aVector.x * aScalar, aVector.y * aScalar);
}
template <typename T>
Vector2<T> operator*(const T& aScalar, const Vector2<T>& aVector)
{
	return aVector * aScalar;
}
template <typename T>
Vector2<T> operator/(const Vector2<T>& aVector, const T& aScalar)
{
	assert(aScalar != 0);
	return Vector2<T>(aVector.x / aScalar, aVector.y / aScalar);
}
template <typename T>
void operator+=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	aVector0.x = aVector0.x + aVector1.x;
	aVector0.y = aVector0.y + aVector1.y;
}
template <typename T>
void operator-=(Vector2<T>& aVector0, const Vector2<T>& aVector1)
{
	aVector0.x = aVector0.x - aVector1.x;
	aVector0.y = aVector0.y - aVector1.y;
}
template <typename T>
void operator*=(Vector2<T>& aVector, const T& aScalar)
{
	aVector.x = aVector.x * aScalar;
	aVector.y = aVector.y * aScalar;
}
template <typename T>
void operator /=(Vector2<T>& aVector, const T& aScalar)
{
	assert(aScalar != 0);
	aVector.x = aVector.x / aScalar;
	aVector.y = aVector.y / aScalar;
}

