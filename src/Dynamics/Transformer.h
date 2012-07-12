#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <cmath>
#include <exception>

#include "Geometry/Vector.h"

#include "DynamicalModel.h"
#include "Vector.h"
#include "Parameter.h"

class TransformerException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Cannot transform 0-dimensional model.";
    }
};

template <typename ScalarParam>
class Experiment;

template <typename ScalarParam>
class Transformer : public ParameterClass<ScalarParam>
{

friend class Experiment<ScalarParam>;

public:
    typedef DynamicalModel<ScalarParam> Model;
    typedef typename DynamicalModel<ScalarParam>::Scalar Scalar;
    typedef typename DynamicalModel<ScalarParam>::Vector Vector;
    
    Transformer(Model const& model);
    virtual ~Transformer();

    /*
        The first method allocates space for the output. The second method
        writes to a preallocated vector, which is assumed to be distinct from
        the input vector. There will be problems if v == out.
    */
    Vector transform(Vector const& v) const;
    virtual void transform(Vector const& v, Vector & out) const;
    
    Vector invTransform(Vector const& v) const;
    virtual void invTransform(Vector const& v, Vector & out) const;

    /*
        Interaction with Geometry::Vector
    */
    Geometry::Vector<ScalarParam,3> transform2(Vector const& v) const;
    virtual void transform(Vector const& v, Geometry::Vector<ScalarParam, 3> & out) const;

    Vector invTransform(Geometry::Vector<ScalarParam,3> const& v) const;
    virtual void invTransform(Geometry::Vector<ScalarParam,3> const& v, Vector & out) const;

    /* Generally you need to be careful.  If the coordinate ranges from 0, 2PI
     * and you map it to polar coordinates, then its range is now 0. So
     * the default point, center point, and radius is necessarily transformation
     * dependent. Default implementation is to transform the first three
     * coordinates.
     */
    virtual Vector getDefaultPoint(void) const;
    virtual Vector getCenterPoint(void) const;
    virtual Scalar getRadius(void) const;

    // generic methods
    std::string const& getName() const;
    void setName(std::string const& name);

    unsigned int const& getVersion() const;
    
    // These should be implemented in Parameter.h as virtual methods.
    virtual std::string getParameterDisplay(int parameter);
    virtual std::string getParameterDisplay(std::string parameterName);    

protected:
    Model const& model;
    std::string name;

    unsigned int updateVersion();

private:
    unsigned int version;

};


//
// Implementation
//


template <typename ScalarParam>
Transformer<ScalarParam>::Transformer(Model const& model)
: model(model),
  name("transformer"),
  version(0)
{
}

template <typename ScalarParam>
Transformer<ScalarParam>::~Transformer()
{
}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::transform(Vector const& v) const
{
    Vector out(3);
    transform(v, out);
    return out;
}

template <typename ScalarParam>
void Transformer<ScalarParam>::transform(Vector const& v, Vector & out) const
{
    // Take the first three components

    out[0] = v[0];
    out[1] = v[1];
    if (model.getDimension() > 2)
    {
        out[2] = v[2];
    }
    else
    {
        out[2] = 0;
    }
}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::invTransform(Vector const& v) const
{
    Vector out(model.getDimension());
    invTransform(v, out);
    return out;
}

template <typename ScalarParam>
void Transformer<ScalarParam>::invTransform(Vector const& v, Vector & out) const
{
    // Take the first three components
    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
    
    typename CoordinateClass<ScalarParam>::Coordinates coords = model.getCoords();
    for ( int i = 3; i < model.getDimension(); i++ )
    {
        out[i] = coords[i].defaultValue;
    }
}

template <typename ScalarParam>
Geometry::Vector<ScalarParam,3> Transformer<ScalarParam>::transform2(Vector const& v) const
{
    Geometry::Vector<ScalarParam,3> out;
    transform(v, out);
    return out;
}

template <typename ScalarParam>
void Transformer<ScalarParam>::transform(Vector const& v, Geometry::Vector<ScalarParam,3> & out) const
{
    // Take the first three components

    out[0] = v[0];
    out[1] = v[1];
    if (model.getDimension() > 2)
    {
        out[2] = v[2];
    }
    else
    {
        out[2] = 0;
    }
}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::invTransform(Geometry::Vector<ScalarParam,3> const& v) const
{
    Vector out(model.getDimension());
    invTransform(v, out);
    return out;
}

template <typename ScalarParam>
void Transformer<ScalarParam>::invTransform(Geometry::Vector<ScalarParam,3> const& v, Vector & out) const
{
    // Take the first three components

    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
    
    typename CoordinateClass<ScalarParam>::Coordinates coords = model.getCoords();
    for ( int i = 3; i < model.getDimension(); i++ )
    {
        out[i] = coords[i].defaultValue;
    }
}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::getDefaultPoint(void) const
{
    const DTS::Vector<double> defaultPoint = model.getDefaultPoint();
    return transform(defaultPoint);

}

template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Vector Transformer<ScalarParam>::getCenterPoint(void) const
{
    const DTS::Vector<double> centerPoint = model.getCenterPoint();
    return transform(centerPoint);
}    
    
template <typename ScalarParam>
typename DynamicalModel<ScalarParam>::Scalar Transformer<ScalarParam>::getRadius(void) const
{   
    // return the largest non-infinite radius from the first 3 coordinates
    
    typedef typename CoordinateClass<ScalarParam>::Coordinates Coords;
    Coords const coords = model.getCoords();

    int iEnd;
    if (model.getDimension() > 2)
    {
        iEnd = 3;
    }
    else
    {
        iEnd = 2;
    }
    
    ScalarParam radius = 0;
    ScalarParam tempRadius;
    for (int i = 0; i < iEnd; i++)
    {
        tempRadius = coords[0].maxValue - coords[0].minValue;
        if (tempRadius > radius && !std::isinf(tempRadius) )
        {
            radius = tempRadius;
        }
    }

    // we actually computed diameter
    radius /= 2;

    // testing if it is essentially zero
    if ( radius < .0001 ) 
    {
        // default value
        radius = 30;
    }

    return radius;
}

template <typename ScalarParam>
inline
std::string const& Transformer<ScalarParam>::getName() const
{
    return name;
}

template <typename ScalarParam>
inline
void Transformer<ScalarParam>::setName(std::string const& value)
{
    name = value;
}

template <typename ScalarParam>
inline
unsigned int const& Transformer<ScalarParam>::getVersion() const
{
    return version;
}

template <typename ScalarParam>
inline
unsigned int Transformer<ScalarParam>::updateVersion()
{
    return ++version;
}

template <typename ScalarParam>
std::string Transformer<ScalarParam>::getParameterDisplay(int parameterIndex)
{
    // Need to update this to work with all parameter types
    // Right now it (and the one below) assume integer parameters.
    // Also consider moving it to ParameterClass as virtual methods.
    return "";
}

template <typename ScalarParam>
std::string Transformer<ScalarParam>::getParameterDisplay(std::string parameterName)
{
    typename ParameterClass<double>::IntParameters iparams = this->getIntParams();
    int paramIndex = this->getIntParamIndex(parameterName);
    std::string name;
    if (paramIndex >= 0)
    {
        name = getParameterDisplay(paramIndex);
    }
    return name;
}

#endif

