#ifndef DTS_LORENZEXPERIMENT
#define DTS_LORENZEXPERIMENT

#include "Experiment.h"
#include "Rossler4.h"

#include "RungeKutta4.h"
#include "ProjectionTransformer.h"

class Rossler4Experiment : public Experiment<double>
{
public:
    Rossler4Experiment() : Experiment<double>()
    {
        model = new Rossler4();

        addIntegrator( new RungeKutta4(*model, .01) );
        setIntegrator("rk4");
        
        ProjectionTransformer<double> *t;
        t = new ProjectionTransformer<double>(*model);
        
        t->setIntParamValue("xCoordinate", 0);
        t->setIntParamValue("yCoordinate", 2);
        t->setIntParamValue("zCoordinate", 3);
                
        addTransformer( t );
        setTransformer("projection");      
    }
};

#endif
