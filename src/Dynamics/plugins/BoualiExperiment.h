#ifndef FLOW_BOUALIZEXPERIMENT
#define FLOW_BOUALIZEXPERIMENT

#include "Experiment.h"
#include "Bouali.h"

#include "RungeKutta4.h"
#include "ProjectionTransformer.h"

class BoualiExperiment : public Experiment<double>
{
public:
    BoualiExperiment() : Experiment<double>()
    {
        model = new Bouali();

        addIntegrator( new RungeKutta4(*model, .01) );
        setIntegrator("rk4");

        addTransformer( new ProjectionTransformer<double>(*model) );
        setTransformer("projection");
    }
};

#endif
