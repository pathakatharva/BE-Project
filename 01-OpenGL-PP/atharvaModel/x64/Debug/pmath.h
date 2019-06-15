// THIS HEADER FILE CONTAINS THE DEFINITIONS OF THE FUNCTIONS REQUIRED FOR MATRIX AND VECTOR COMPUTATIONS
// MOSTLY USED TO DEFINE MATRICES AND VECTORS FOR GRAPHICS PROGRAMMING 
#pragma once
#define _USE_MATH_DEFINES  1 // Include constants defined in math.h, linke the constant 'M_PI' is used in the function radians()
#include <math.h>


void myIdentity(float identityMatrix[]);
void myPerspective(float perspectiveMatrix[], float fovy, float aspect, float n, float f);
void myOrtho(float orthographicMatrix[], float left, float right, float bottom, float top, float n, float f);
void myTranslate(float translationMatrix[], float x, float y, float z);
void myRotateTriaxial(float rotationMatrix[], float angle_x, float angle_y, float angle_z);
void myRotate(float rotationMatrix[], float angle, float x, float y, float z);
void myScale(float scaleMatrix[], float x, float y, float z);

namespace pmath
{
	template <typename T, const int len> class vecN;
	template <typename T, const int len>
	class vecN
	{
	public:
		typedef class vecN<T, len> my_type;
		typedef T element_type;

		// Default constructor does nothing, just like built-in types
		inline vecN()
		{
			// Uninitialized variable
		}

		// Copy constructor
		inline vecN(const vecN& that)
		{
			assign(that);
		}

		// Construction from scalar
		inline vecN(T s)
		{
			int n;
			for (n = 0; n < len; n++)
			{
				data[n] = s;
			}
		}

		// Assignment operator
		inline vecN& operator=(const vecN& that)
		{
			assign(that);
			return *this;
		}

		inline vecN& operator=(const T& that)
		{
			int n;
			for (n = 0; n < len; n++)
				data[n] = that;

			return *this;
		}

		inline vecN operator+(const vecN& that) const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] + that.data[n];
			return result;
		}

		inline vecN& operator+=(const vecN& that)
		{
			return (*this = *this + that);
		}

		inline vecN operator-() const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = -data[n];
			return result;
		}

		inline vecN operator-(const vecN& that) const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] - that.data[n];
			return result;
		}

		inline vecN& operator-=(const vecN& that)
		{
			return (*this = *this - that);
		}

		inline vecN operator*(const vecN& that) const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] * that.data[n];
			return result;
		}

		inline vecN& operator*=(const vecN& that)
		{
			return (*this = *this * that);
		}

		inline vecN operator*(const T& that) const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] * that;
			return result;
		}

		inline vecN& operator*=(const T& that)
		{
			assign(*this * that);

			return *this;
		}

		inline vecN operator/(const vecN& that) const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] / that.data[n];
			return result;
		}

		inline vecN& operator/=(const vecN& that)
		{
			assign(*this / that);

			return *this;
		}

		inline vecN operator/(const T& that) const
		{
			my_type result;
			int n;
			for (n = 0; n < len; n++)
				result.data[n] = data[n] / that;
			return result;
		}

		inline vecN& operator/=(const T& that)
		{
			assign(*this / that);
			return *this;
		}

		inline T& operator[](int n) { return data[n]; }
		inline const T& operator[](int n) const { return data[n]; }

		inline static int size(void) { return len; }

		inline operator const T* () const { return &data[0]; }

		static inline vecN random()
		{
			vecN result;
			int i;

			for (i = 0; i < len; i++)
			{
				result[i] = vmath::random<T>();
			}
			return result;
		}

	protected:
		T data[len];

		inline void assign(const vecN& that)
		{
			int n;
			for (n = 0; n < len; n++)
				data[n] = that.data[n];
		}
	};


	template <typename T>
	class Tvec2 : public vecN<T, 2>
	{
	public:
		typedef vecN<T, 2> base;

		// Uninitialized variable
		inline Tvec2() {}
		// Copy constructor
		inline Tvec2(const base& v) : base(v) {}

		// vec2(x, y);
		inline Tvec2(T x, T y)
		{
			base::data[0] = x;
			base::data[1] = y;
		}
	};


	template <typename T>
	class Tvec3 : public vecN<T, 3>
	{
	public:
		typedef vecN<T, 3> base;

		// Uninitialized variable
		inline Tvec3() {}

		// Copy constructor
		inline Tvec3(const base& v) : base(v) {}

		// vec3(x, y, z);
		inline Tvec3(T x, T y, T z)
		{
			base::data[0] = x;
			base::data[1] = y;
			base::data[2] = z;
		}

		// vec3(v, z);
		inline Tvec3(const Tvec2<T>& v, T z)
		{
			base::data[0] = v[0];
			base::data[1] = v[1];
			base::data[2] = z;
		}

		// vec3(x, v)
		inline Tvec3(T x, const Tvec2<T>& v)
		{
			base::data[0] = x;
			base::data[1] = v[0];
			base::data[2] = v[1];
		}
	};
	typedef Tvec2<float> vec2;
	typedef Tvec3<float> vec3;

}
void createTransformationMatrix(pmath::vec3 translate,pmath:: vec3 scale, float  transformMatrix[],pmath:: vec3 rotate);
class Camera;//forward declaration
void createViewMatrix(float viewMatrix[], Camera cam);
void createSkyViewMatrix(float viewMatrix[], Camera cam);

//to be used in the case -> matrix2 = matrix1 * matrix2 (eg: modelViewMatrix = modelViewMatrix * rotationMatrix)
//then this function will be called like multiplyMatrices(rotationMatrix, modelViewMatrix);
void multiplyMatrices1(float [], float []);

//to be used in the case -> matrix3 = matrix1 * matrix2 (eg: modelViewProjectionMatrix = projectionMatrixMatrix * modelViewMatrix)
//then this function will be called like multiplyMatrices(modelViewProjectionMatrix, modelViewMatrix, projectionMatrix);
void multiplyMatrices2(float [], float [], float []);
float radians(float );





