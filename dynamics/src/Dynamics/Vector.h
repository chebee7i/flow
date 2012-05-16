#ifndef DTS_VECTOR_H
#define DTS_VECTOR_H

#include <exception>
#include <vector>
#include <iostream>



namespace DTS {

template <typename ScalarParam>
class Vector
{
    public:
    typedef ScalarParam Scalar;

    protected:
    /**
    * If the dimension is const, then we can't have a default constructor.
    * This ends up being painful if we want to have tools which store a vector
    * as a member since the dimensionality of that vector must change when
    * the experiment changes. So we make this flexible and allow resizing of
    * the dimension.  It will now be the developer's responsibility to make
    * sure the Vector is properly initialized.
    */
    int dimension;
    std::vector<ScalarParam> components;

    /* Constructors and destructors */
    public:
    Vector(void); // zero-dimensional vector
    Vector(int const dimension, ScalarParam const& value = ScalarParam());
    Vector(Vector<ScalarParam> const& v);
    Vector(std::vector<ScalarParam> const& v);
    virtual ~Vector();

    /* Generic Methods */
    int getDimension(void) const;
    void setDimension(int dimension);
    std::vector<ScalarParam> const& getComponents(void) const;
    std::vector<ScalarParam>& getComponents(void);
    Scalar operator[](int index) const; // Returns component as rvalue
    Scalar& operator[](int index); // Returns component as modifiable lvalue


    /* Math Methods */
	Vector operator+(void) const; // Unary plus operator; returns copy of vector
	Vector operator-(void) const; // Negation operator; returns copy of vector
    Vector& operator=(Vector const& other);  // Assignment
	Vector& operator+=(Vector const& other); // Addition assignment
	Vector& operator-=(Vector const& other); // Subtraction assignment
	Vector& operator*=(ScalarParam scalar); // Scalar multiplication assignment
	Vector& operator/=(ScalarParam scalar); // Scalar division assignment
};


/*******************************
 * Opertions on Vector objects *
 *******************************/

// ostream operator
template <typename ScalarParam>
std::ostream& operator<<(std::ostream& stream, const Vector<ScalarParam>& v)
{
	stream << "(";
	for (int i = 0; i < v.getDimension(); ++i)
	{
		stream << v[i];
		if (i != v.getDimension() - 1)
		{
			stream << ", ";
		}
	}
	stream << ")";
	return stream;
}

// Equality operator
template <typename ScalarParam>
inline bool operator==(const Vector<ScalarParam>& v1, const Vector<ScalarParam>& v2)
{
	bool result = v1.getDimension() == v2.getDimension();
	for(int i = 0; i < v1.getDimension(); ++i)
	{
		result &= v1[i] == v2[i];
	}
	return result;
}

// Inequality operator
template <typename ScalarParam>
inline bool operator!=(const Vector<ScalarParam>& v1, const Vector<ScalarParam>& v2)
{
	bool result = v1.getDimension() != v2.getDimension();
	for(int i = 0; i < v1.getDimension(); ++i)
	{
		result |= v1[i] != v2[i];
	}
	return result;
}

/*
 * For addition and subtraction, we do not check that the length of the
 * vectors is the same.  So: (1,2) + (1,2,3) = (2,4).  That is, we truncate
 * the tail of the longer vector since the dimension of the shorter vector
 * is used to construct the new vector.
 */

// Addition
template <typename ScalarParam>
inline Vector<ScalarParam> operator+(const Vector<ScalarParam>& v1, const Vector<ScalarParam>& v2)
{
	int dimension = v1.getDimension();
	int dimension2 = v2.getDimension();
	if (dimension2 < dimension)
	{
		dimension = dimension2;
	}

	Vector<ScalarParam> result( dimension );
	for(int i = 0; i < dimension; ++i)
	{
		result[i] = v1[i] + v2[i];
	}
	return result;
}

// Subtraction
template <typename ScalarParam>
inline Vector<ScalarParam> operator-(const Vector<ScalarParam>& v1, const Vector<ScalarParam>& v2)
{
	int dimension = v1.getDimension();
	int dimension2 = v2.getDimension();
	if (dimension2 < dimension)
	{
		dimension = dimension2;
	}

	Vector<ScalarParam> result( dimension );
	for(int i = 0; i < dimension; ++i)
	{
		result[i] = v1[i] - v2[i];
	}
	return result;
}

// Scalar multiplication (from the right)
template <typename ScalarParam>
inline Vector<ScalarParam> operator*(const Vector<ScalarParam>& v, ScalarParam scalar)
{
	int dimension = v.getDimension();
	Vector<ScalarParam> result( dimension );
	for(int i = 0; i < dimension; ++i)
	{
		result[i] = v[i] * scalar;
	}
	return result;
}

// Scalar multiplication (from the left)
template <typename ScalarParam>
inline Vector<ScalarParam> operator*(ScalarParam scalar, const Vector<ScalarParam>& v)
{
	int dimension = v.getDimension();
	Vector<ScalarParam> result( dimension );
	for(int i = 0; i < dimension; ++i)
	{
		result[i] = scalar * v[i];
	}
	return result;
}

// Scalar division (only from the right)
template <typename ScalarParam>
inline Vector<ScalarParam> operator/(const Vector<ScalarParam>& v, ScalarParam scalar)
{
	int dimension = v.getDimension();
	Vector<ScalarParam> result( dimension );
	for(int i = 0; i < dimension; ++i)
	{
		result[i] = v[i] / scalar;
	}
	return result;
}


/*******************
 * Implementations *
 *******************/


/* Constructors and destructors */

template <typename ScalarParam>
Vector<ScalarParam>::Vector(void)
: dimension(0)
{
	components.resize(dimension);
}

template <typename ScalarParam>
Vector<ScalarParam>::Vector(int const dimension, ScalarParam const& value)
: dimension(dimension)
{
    components.resize(dimension);
    for(int i = 0; i < dimension; ++i)
    {
        components[i] = value;
    }
}

template <typename ScalarParam>
Vector<ScalarParam>::Vector(Vector<ScalarParam> const& v)
: dimension( v.getDimension() )
{
    components.resize(dimension);
    for(int i = 0; i < dimension; ++i)
    {
        components[i] = v[i];
    }
}

template <typename ScalarParam>
Vector<ScalarParam>::Vector(std::vector<ScalarParam> const& v)
: dimension( v.size() )
{
    components.resize(dimension);
    for(int i = 0; i < dimension; ++i)
    {
        components[i] = v[i];
    }
}

template <typename ScalarParam>
Vector<ScalarParam>::~Vector()
{
}

/* Generic Methods */

template <typename ScalarParam>
int Vector<ScalarParam>::getDimension(void) const
{
    return dimension;
}

template <typename ScalarParam>
void Vector<ScalarParam>::setDimension(int dimension)
{
	this->dimension = dimension;
    components.resize(dimension);
}

template <typename ScalarParam>
std::vector<ScalarParam> const& Vector<ScalarParam>::getComponents(void) const
{
    return components;
}

template <typename ScalarParam>
std::vector<ScalarParam>& Vector<ScalarParam>::getComponents(void)
{
    return components;
}

template <typename ScalarParam>
ScalarParam Vector<ScalarParam>::operator[](int index) const
{
    return components[index];
}

template <typename ScalarParam>
ScalarParam& Vector<ScalarParam>::operator[](int index)
{
    return components[index];
}

/* Math Methods */

template <typename ScalarParam>
Vector<ScalarParam> Vector<ScalarParam>::operator+(void) const
{
    return *this;
}

template <typename ScalarParam>
Vector<ScalarParam> Vector<ScalarParam>::operator-(void) const
{
    Vector result(dimension);
    for(int i = 0; i < dimension; ++i)
    {
        result.components[i] = -components[i];
    }
    return result;
}

template <typename ScalarParam>
Vector<ScalarParam>& Vector<ScalarParam>::operator=(const Vector<ScalarParam>& other)
{
	for(int i = 0; i < dimension; ++i)
	{
		components[i] = other.components[i];
	}
	return *this;
}

template <typename ScalarParam>
Vector<ScalarParam>& Vector<ScalarParam>::operator+=(const Vector<ScalarParam>& other)
{
	for(int i = 0; i < dimension; ++i)
	{
		components[i] += other.components[i];
	}
	return *this;
}

template <typename ScalarParam>
Vector<ScalarParam>& Vector<ScalarParam>::operator-=(const Vector<ScalarParam>& other)
{
	for(int i = 0; i < dimension; ++i)
	{
		components[i] -= other.components[i];
	}
	return *this;
}

template <typename ScalarParam>
Vector<ScalarParam>& Vector<ScalarParam>::operator*=(ScalarParam scalar)
{
	for(int i = 0; i < dimension; ++i)
	{
		components[i] *= scalar;
	}
	return *this;
}

template <typename ScalarParam>
Vector<ScalarParam>& Vector<ScalarParam>::operator/=(ScalarParam scalar)
{
	for(int i = 0; i < dimension; ++i)
	{
		components[i] /= scalar;
	}
	return *this;
}

} // end namespace DTS

#endif
