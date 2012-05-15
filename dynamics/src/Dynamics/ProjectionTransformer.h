#ifndef DTS_PROJECTION_TRANSFORMER
#define DTS_PROJECTION_TRANSFORMER

#include "Coordinate.h"
#include "Parameter.h"
#include "Transformer.h"

template <typename ScalarParam>
class ProjectionTransformer : public Transformer<ScalarParam>
{

public:

    ProjectionTransformer(DynamicalModel<ScalarParam> const& model);
    virtual ~ProjectionTransformer();

    virtual void transform(typename DynamicalModel<ScalarParam>::Vector const& v, 
                           typename DynamicalModel<ScalarParam>::Vector & out);
                           
    virtual void invTransform(typename DynamicalModel<ScalarParam>::Vector const& v, 
                              typename DynamicalModel<ScalarParam>::Vector & out);
                              
    virtual std::string getParameterDisplay(int parameter);                           
};


//
// Implementation
//

template <typename ScalarParam>
ProjectionTransformer<ScalarParam>::ProjectionTransformer(DynamicalModel<ScalarParam> const& model)
: Transformer<ScalarParam>(model)
{
    this->setName("projection");
    
    // Determine appropriate defaults
    int d = model.getDimension();
    int x = 0;
    int y = (d > 1 ? 1 : -1);
    int z = (d > 2 ? 2 : -1);

    typedef typename ParameterClass<ScalarParam>::IntParameter IntParameter;

    addIntParameter( IntParameter("xDisplay", x, -1, d-1, x, 1) );
    addIntParameter( IntParameter("yDisplay", y, -1, d-1, y, 1) );
    addIntParameter( IntParameter("zDisplay", z, -1, d-1, z, 1) );        
}

template <typename ScalarParam>
ProjectionTransformer<ScalarParam>::~ProjectionTransformer()
{
}


template <typename ScalarParam>
inline
void ProjectionTransformer<ScalarParam>::transform(typename DynamicalModel<ScalarParam>::Vector const& v, 
                                                   typename DynamicalModel<ScalarParam>::Vector & out)
{
    // A value of -1 means it will be mapped to the value 0.
    int const& xIndex = this->intParamValues[0];
    int const& yIndex = this->intParamValues[1];
    int const& zIndex = this->intParamValues[2];    
    
    out[0] = ( xIndex == -1 ? 0 : v[ xIndex ] );
    out[1] = ( yIndex == -1 ? 0 : v[ yIndex ] );
    out[2] = ( zIndex == -1 ? 0 : v[ zIndex ] );
}

template <typename ScalarParam>
void ProjectionTransformer<ScalarParam>::invTransform(typename DynamicalModel<ScalarParam>::Vector const& v, 
                                                      typename DynamicalModel<ScalarParam>::Vector & out)
{
    // A value of -1 means it will be mapped to the value 0.
    int const& xIndex = this->intParamValues[0];
    int const& yIndex = this->intParamValues[1];
    int const& zIndex = this->intParamValues[2];    
    
    if (xIndex > -1) out[xIndex] = v[0];
    if (yIndex > -1) out[yIndex] = v[1];
    if (zIndex > -1) out[zIndex] = v[2];

    typename CoordinateClass<ScalarParam>::Coordinates coords = this->model.getCoords();
    for ( int i = 0; i < this->model.getDimension(); i++ )
    {
        if (i == xIndex || i == yIndex || i == zIndex)
        {
            continue;
        }        
        out[i] = coords[i].defaultValue;
    }
}

template <typename ScalarParam>
std::string ProjectionTransformer<ScalarParam>::getParameterDisplay(int parameterIndex)
{
    std::string name;
    typename CoordinateClass<ScalarParam>::Coordinates coords = this->model.getCoords();
    int coordinateIndex = this->intParamValues[parameterIndex];
    if (coordinateIndex == -1)
    {
        name = "0";   
    }
    else if (coordinateIndex >= 0 && coordinateIndex < (int)coords.size() )
    {
        name = coords[coordinateIndex].name;
    }
    return name;
}

#endif
