
#include "CartesianGrid3D.h"

Integrator* maker()
{
    return new CartesianGrid3DIntegrator;
}

class CartesianGrid3DProxy
{
public:
    CartesianGrid3DProxy()
    {
        Factory["CartesianGrid3D"] = maker;
    }
};

CartesianGrid3DProxy p;


