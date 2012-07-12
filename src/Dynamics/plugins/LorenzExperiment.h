#ifndef DTS_LORENZEXPERIMENT
#define DTS_LORENZEXPERIMENT

#include "Experiment.h"
#include "Lorenz.h"

#include "RungeKutta4.h"
#include "ProjectionTransformer.h"

class LorenzExperiment : public Experiment<double>
{
public:
    LorenzExperiment() : Experiment<double>()
    {
        model = new Lorenz();

        addIntegrator( new RungeKutta4(*model, .01) );
        setIntegrator("rk4");
        
        addTransformer( new ProjectionTransformer<double>(*model) );
        setTransformer("projection");      
    }
};

#endif
