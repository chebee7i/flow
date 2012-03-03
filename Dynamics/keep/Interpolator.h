#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <Geometry/Point.h>
#include <Geometry/Vector.h>

typedef double Scalar; // Scalar type of underlying point space
static const int dimension=3; // Always three-dimensional
typedef Geometry::Point<Scalar, dimension> Point; // Point type
typedef Geometry::Vector<Scalar,dimension> Vector; // Vector type

class Interpolator
{
protected:
    Vector (*exact)(const Point &p);
    Point& gridSpacing;
    Point& gridOffset;
		
public:
    Interpolator(Vector (*exact)(const Point &p), Point& gridSpacing, Point& gridOffset)
    : exact(exact), gridSpacing(gridSpacing), gridOffset(gridOffset)
    {
    }

    virtual ~Interpolator() 
    {
    }

    virtual Vector interpolate(const Point& p) const;
};

class TriLinearInterpolator : public Interpolator
{
public:
    TriLinearInterpolator(Vector (*exact)(const Point &p), Point& gridSpacing, Point& gridOffset)
    : Interpolator(exact, gridSpacing, gridOffset)
    {
    }

    virtual Vector interpolate(const Point& p) const;
};





Vector Interpolator::interpolate(const Point& p) const
{
    return exact(p);
}




#endif

