
#include <iostream>

#include "BaseModel.h"
#include "TrilinearInterpolator.h"

typedef Interpolator::Point Point;
typedef Interpolator::Vector Vector;
typedef Interpolator::Scalar Scalar;

Vector TrilinearInterpolator::_interpolate(const Point& p) const
{
    Point *p1, *p2;

    // construct cube coordinates

    Scalar x0 = floor( p[0] / gridSpacing[0] ) * gridSpacing[0] + gridOffset[0];
    Scalar x1 = x0 + gridSpacing[0];
    Scalar y0 = floor( p[1] / gridSpacing[1] ) * gridSpacing[1] + gridOffset[1];
    Scalar y1 = y0 + gridSpacing[1];
    Scalar z0 = floor( p[2] / gridSpacing[2] ) * gridSpacing[2] + gridOffset[2];
    Scalar z1 = z0 + gridSpacing[2];

    /**
        f is exact function
        g is linearly interpolated function

        dx   = x1 - x0
        r    = (x - x0) / dx                
        g(x) = (1 - r) f(x) + r f(x+dx)
             = [(x1 - x) f(x) + (x-x0) f(x+dx)] / dx

    **/

    // interpolate x-coordinate

    p1 = new Point(x0,y0,z0);
    p2 = new Point(x1,y0,z0);
    Vector C_x00 = (x1 - p[0]) * model->exact( *p1 ) + 
                   (p[0] - x0) * model->exact( *p2 );
    C_x00 /= gridSpacing[0];

    p1 = new Point(x0,y0,z1);
    p2 = new Point(x1,y0,z1);
    Vector C_x01 = (x1 - p[0]) * model->exact( *p1 ) + 
                   (p[0] - x0) * model->exact( *p2 );
    C_x01 /= gridSpacing[0];

    p1 = new Point(x0,y1,z0);
    p2 = new Point(x1,y1,z0);
    Vector C_x10 = (x1 - p[0]) * model->exact( *p1 ) + 
                   (p[0] - x0) * model->exact( *p2 );
    C_x10 /= gridSpacing[0];

    p1 = new Point(x0,y1,z1);
    p2 = new Point(x1,y1,z1);
    Vector C_x11 = (x1 - p[0]) * model->exact( *p1 ) +
                   (p[0] - x0) * model->exact( *p2 );
    C_x11 /= gridSpacing[0];

    // interpolate y-coordinate

    Vector C_xy0 = (y1 - p[1]) * C_x00 + (p[1] - y0) * C_x10;
    C_xy0 /= gridSpacing[1];

    Vector C_xy1 = (y1 - p[1]) * C_x01 + (p[1] - y0) * C_x11;
    C_xy1 /= gridSpacing[1];

    // interpolate z-coordinate

    Vector C_xyz = (z1 - p[2]) * C_xy0 + (p[2] - z0) * C_xy1;
    C_xyz /= gridSpacing[2];

    return C_xyz;
}

