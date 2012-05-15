#ifndef DTS_ROSSLER3EXPERIMENT
#define DTS_ROSSLER3EXPERIMENT

#include "Experiment.h"
#include "Rossler3.h"

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
        
        ProjectionTransformer<double> *t;
        t = new ProjectionTransformer<double>(*model);
        
        t->setIntParamValue("xCoordinate", 0);
        t->setIntParamValue("yCoordinate", 1);
        t->setIntParamValue("zCoordinate", 2);
                
        addTransformer( t );
        setTransformer("projection");      
    }
};

#endif
