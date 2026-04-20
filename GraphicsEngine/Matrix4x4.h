#pragma once
#include <cmath>

namespace CommonUtilities
{
	template <typename T>
	class Matrix3x3;

	template <typename T>
	class Vector4;

	template <typename T>
	class Vector3;

	template <typename T>
	class Matrix4x4
	{
	public:
		Matrix4x4<T>();
		Matrix4x4<T>(
			const T& a11, const T& a12, const T& a13, const T& a14,
			const T& a21, const T& a22, const T& a23, const T& a24,
			const T& a31, const T& a32, const T& a33, const T& a34,
			const T& a41, const T& a42, const T& a43, const T& a44);
		Matrix4x4<T>(const Matrix4x4<T>& aMatrix) = default;
		Matrix4x4<T>(const Matrix3x3<T>& aMatrix);

		Matrix4x4<T>& operator=(const Matrix4x4<T>& aMatrix) = default;

		Matrix4x4<T> operator+(const Matrix4x4<T>& aAddend) const;
		Matrix4x4<T>& operator+=(const Matrix4x4<T>& aAddend);
		Matrix4x4<T> operator-(const Matrix4x4<T>& aSubtrahend) const;
		Matrix4x4<T>& operator-=(const Matrix4x4<T>& aSubtrahend);
		Matrix4x4<T> operator*(const Matrix4x4<T>& aFactor) const;
		Matrix4x4<T>& operator*=(const Matrix4x4<T>& aFactor);
		bool operator==(const Matrix4x4<T>& aComparitor) const;

		T& operator()(const int aRow, const int aColumn);
		const T& operator()(const int aRow, const int aColumn) const;

		Matrix4x4<T> GetTranspose() const;

		static Matrix4x4<T> GetFastInverse(const Matrix4x4<T>& aMatrix);

		static Matrix4x4<T> CreateRotationAroundX(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundY(const T aAngleInRadians);
		static Matrix4x4<T> CreateRotationAroundZ(const T aAngleInRadians);

	private:
		union
		{
			T linear[16];
			T nonLinear[4][4];
		};
	};

	template<typename T>
	inline Matrix4x4<T>::Matrix4x4()
		: linear{	T{1},T{0},T{0},T{0},
					T{0},T{1},T{0},T{0},
					T{0},T{0},T{1},T{0},
					T{0},T{0},T{0},T{1} }
	{ }

	template<typename T>
	inline Matrix4x4<T>::Matrix4x4(
		const T & a11, const T & a12, const T & a13, const T & a14,
		const T & a21, const T & a22, const T & a23, const T & a24,
		const T & a31, const T & a32, const T & a33, const T & a34,
		const T & a41, const T & a42, const T & a43, const T & a44 ) 
		: linear{	a11, a12, a13, a14,
					a21, a22, a23, a24,
					a31, a32, a33, a34,
					a41, a42, a43, a44 }
	{ }

	template<typename T>
	inline Matrix4x4<T>::Matrix4x4( const Matrix3x3<T>& aMatrix)
		: linear{	aMatrix(1,1), aMatrix(1,2), aMatrix(1,3), T{0},
					aMatrix(2,1), aMatrix(2,2), aMatrix(2,3), T{0},
					aMatrix(3,1), aMatrix(3,2), aMatrix(3,3), T{0},
					T{0},T{0},T{0},T{1} }
	{ }

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator+(const Matrix4x4<T>& aAddend) const
	{
		Matrix4x4 sum(*this);

		for (int i = 0; i < 16; ++i)
		{
			sum.linear[i] += aAddend.linear[i];
		}
		return sum;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator+=( const Matrix4x4<T>& aAddend)
	{
		*this = *this + aAddend;
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator-(const Matrix4x4<T>& aSubtrahend) const
	{
		Matrix4x4 difference(*this);

		for (int i = 0; i < 16; ++i)
		{
			difference.linear[i] -= aSubtrahend.linear[i];
		}
		return difference;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator-=(const Matrix4x4<T>& aSubtrahend)
	{
		*this = *this - aSubtrahend;
		return *this;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::operator*(const Matrix4x4<T>& aFactor) const
	{
		Matrix4x4 product;

		product.nonLinear[0][0] = nonLinear[0][0] * aFactor.nonLinear[0][0] + nonLinear[0][1] * aFactor.nonLinear[1][0] + nonLinear[0][2] * aFactor.nonLinear[2][0] + nonLinear[0][3] * aFactor.nonLinear[3][0];
		product.nonLinear[0][1] = nonLinear[0][0] * aFactor.nonLinear[0][1] + nonLinear[0][1] * aFactor.nonLinear[1][1] + nonLinear[0][2] * aFactor.nonLinear[2][1] + nonLinear[0][3] * aFactor.nonLinear[3][1];
		product.nonLinear[0][2] = nonLinear[0][0] * aFactor.nonLinear[0][2] + nonLinear[0][1] * aFactor.nonLinear[1][2] + nonLinear[0][2] * aFactor.nonLinear[2][2] + nonLinear[0][3] * aFactor.nonLinear[3][2];
		product.nonLinear[0][3] = nonLinear[0][0] * aFactor.nonLinear[0][3] + nonLinear[0][1] * aFactor.nonLinear[1][3] + nonLinear[0][2] * aFactor.nonLinear[2][3] + nonLinear[0][3] * aFactor.nonLinear[3][3];
																																															
		product.nonLinear[1][0] = nonLinear[1][0] * aFactor.nonLinear[0][0] + nonLinear[1][1] * aFactor.nonLinear[1][0] + nonLinear[1][2] * aFactor.nonLinear[2][0] + nonLinear[1][3] * aFactor.nonLinear[3][0];
		product.nonLinear[1][1] = nonLinear[1][0] * aFactor.nonLinear[0][1] + nonLinear[1][1] * aFactor.nonLinear[1][1] + nonLinear[1][2] * aFactor.nonLinear[2][1] + nonLinear[1][3] * aFactor.nonLinear[3][1];
		product.nonLinear[1][2] = nonLinear[1][0] * aFactor.nonLinear[0][2] + nonLinear[1][1] * aFactor.nonLinear[1][2] + nonLinear[1][2] * aFactor.nonLinear[2][2] + nonLinear[1][3] * aFactor.nonLinear[3][2];
		product.nonLinear[1][3] = nonLinear[1][0] * aFactor.nonLinear[0][3] + nonLinear[1][1] * aFactor.nonLinear[1][3] + nonLinear[1][2] * aFactor.nonLinear[2][3] + nonLinear[1][3] * aFactor.nonLinear[3][3];
																																															
		product.nonLinear[2][0] = nonLinear[2][0] * aFactor.nonLinear[0][0] + nonLinear[2][1] * aFactor.nonLinear[1][0] + nonLinear[2][2] * aFactor.nonLinear[2][0] + nonLinear[2][3] * aFactor.nonLinear[3][0];
		product.nonLinear[2][1] = nonLinear[2][0] * aFactor.nonLinear[0][1] + nonLinear[2][1] * aFactor.nonLinear[1][1] + nonLinear[2][2] * aFactor.nonLinear[2][1] + nonLinear[2][3] * aFactor.nonLinear[3][1];
		product.nonLinear[2][2] = nonLinear[2][0] * aFactor.nonLinear[0][2] + nonLinear[2][1] * aFactor.nonLinear[1][2] + nonLinear[2][2] * aFactor.nonLinear[2][2] + nonLinear[2][3] * aFactor.nonLinear[3][2];
		product.nonLinear[2][3] = nonLinear[2][0] * aFactor.nonLinear[0][3] + nonLinear[2][1] * aFactor.nonLinear[1][3] + nonLinear[2][2] * aFactor.nonLinear[2][3] + nonLinear[2][3] * aFactor.nonLinear[3][3];
																																															
		product.nonLinear[3][0] = nonLinear[3][0] * aFactor.nonLinear[0][0] + nonLinear[3][1] * aFactor.nonLinear[1][0] + nonLinear[3][2] * aFactor.nonLinear[2][0] + nonLinear[3][3] * aFactor.nonLinear[3][0];
		product.nonLinear[3][1] = nonLinear[3][0] * aFactor.nonLinear[0][1] + nonLinear[3][1] * aFactor.nonLinear[1][1] + nonLinear[3][2] * aFactor.nonLinear[2][1] + nonLinear[3][3] * aFactor.nonLinear[3][1];
		product.nonLinear[3][2] = nonLinear[3][0] * aFactor.nonLinear[0][2] + nonLinear[3][1] * aFactor.nonLinear[1][2] + nonLinear[3][2] * aFactor.nonLinear[2][2] + nonLinear[3][3] * aFactor.nonLinear[3][2];
		product.nonLinear[3][3] = nonLinear[3][0] * aFactor.nonLinear[0][3] + nonLinear[3][1] * aFactor.nonLinear[1][3] + nonLinear[3][2] * aFactor.nonLinear[2][3] + nonLinear[3][3] * aFactor.nonLinear[3][3];

		return product;
	}

	template<typename T>
	inline Matrix4x4<T>& Matrix4x4<T>::operator*=(const Matrix4x4<T>& aFactor)
	{
		*this = *this * aFactor;
		return *this;
	}

	template<typename T>
	inline Vector4<T> operator*(const Vector4<T>& aFactorOne, const Matrix4x4<T>& aFactorTwo)
	{
		const float x = aFactorTwo(1, 1) * aFactorOne.x + aFactorTwo(2, 1) * aFactorOne.y + aFactorTwo(3, 1) * aFactorOne.z + aFactorTwo(4, 1) * aFactorOne.w;
		const float y = aFactorTwo(1, 2) * aFactorOne.x + aFactorTwo(2, 2) * aFactorOne.y + aFactorTwo(3, 2) * aFactorOne.z + aFactorTwo(4, 2) * aFactorOne.w;
		const float z = aFactorTwo(1, 3) * aFactorOne.x + aFactorTwo(2, 3) * aFactorOne.y + aFactorTwo(3, 3) * aFactorOne.z + aFactorTwo(4, 3) * aFactorOne.w;
		const float w = aFactorTwo(1, 4) * aFactorOne.x + aFactorTwo(2, 4) * aFactorOne.y + aFactorTwo(3, 4) * aFactorOne.z + aFactorTwo(4, 4) * aFactorOne.w;

		return Vector4<T>(x, y, z, w);
	}

	template<typename T>
	inline bool Matrix4x4<T>::operator==(const Matrix4x4<T>& aComparitor) const
	{
		for (int i = 0; i < 16; ++i)
		{
			if (aComparitor.linear[i] != linear[i])
			{
				return false;
			}
		}
		return true;
	}

	template<typename T>
	inline T& Matrix4x4<T>::operator()( const int aRow, const int aColumn)
	{
		return nonLinear[aRow-1][aColumn-1];
	}

	template<typename T>
	inline const T& Matrix4x4<T>::operator()( const int aRow, const int aColumn) const
	{
		return nonLinear[aRow-1][aColumn-1];
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetTranspose() const
	{
		return Matrix4x4<T>(
			linear[0], linear[4], linear[8], linear[12],
			linear[1], linear[5], linear[9], linear[13],
			linear[2], linear[6], linear[10], linear[14],
			linear[3], linear[7], linear[11], linear[15]);
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::GetFastInverse(const Matrix4x4<T>& aMatrix)
	{
		Matrix4x4<T> matrix(aMatrix);

		matrix.linear[1] = aMatrix.linear[4];
		matrix.linear[2] = aMatrix.linear[8];
		matrix.linear[4] = aMatrix.linear[1];
		matrix.linear[6] = aMatrix.linear[9];
		matrix.linear[8] = aMatrix.linear[2];
		matrix.linear[9] = aMatrix.linear[6];

		Matrix3x3<T> transposeRotation(matrix);
		Vector3<T> inverseTranslation(-matrix.linear[12], -matrix.linear[13], -matrix.linear[14]);
		inverseTranslation = inverseTranslation * transposeRotation;

		matrix.linear[12] = inverseTranslation.x;
		matrix.linear[13] = inverseTranslation.y;
		matrix.linear[14] = inverseTranslation.z;

		return matrix;
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundX(const T aAngleInRadians)
	{
		return Matrix4x4<T>(
			T{ 1 }, T{ 0 }, T{ 0 }, T{ 0 },
			T{ 0 },  std::cos(aAngleInRadians), std::sin(aAngleInRadians), T{ 0 },
			T{ 0 }, -std::sin(aAngleInRadians), std::cos(aAngleInRadians), T{ 0 },
			T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 });
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundY(const T aAngleInRadians)
	{
		return Matrix4x4<T>(
			std::cos(aAngleInRadians), T{ 0 }, -std::sin(aAngleInRadians), T{ 0 },
			T{ 0 }, T{ 1 }, T{ 0 }, T{ 0 },
			std::sin(aAngleInRadians), T{ 0 },  std::cos(aAngleInRadians), T{ 0 },
			T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 });
	}

	template<typename T>
	inline Matrix4x4<T> Matrix4x4<T>::CreateRotationAroundZ(const T aAngleInRadians)
	{
		return Matrix4x4<T>(
			 std::cos(aAngleInRadians), std::sin(aAngleInRadians), T{ 0 }, T{ 0 },
			-std::sin(aAngleInRadians), std::cos(aAngleInRadians), T{ 0 }, T{ 0 },
			T{ 0 }, T{ 0 }, T{ 1 }, T{ 0 },
			T{ 0 }, T{ 0 }, T{ 0 }, T{ 1 });
	}
}