#ifndef DTS_ROSSLER3EXPERIMENT
#define DTS_ROSSLER3EXPERIMENT

#include "Experiment.h"
#include "Models/Rossler3.h"

#include "RungeKutta4.h"
#include "ProjectionTransformer.h"

class Rossler3Experiment : public Experiment<double>
{
public:
    Rossler3Experiment() : Experiment<double>()
    {
        model = new Rossler3();

        addIntegrator( new RungeKutta4(*model, .1) );
        setIntegrator("rk4");
        
        addTransformer( new ProjectionTransformer<double>(*model) );
        setTransformer("projection");  
    }
};

#endif
