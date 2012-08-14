#ifndef DTS_OWLEXPERIMENT
#define DTS_OWLEXPERIMENT

#include "Experiment.h"
#include "Models/Owl.h"

#include "RungeKutta4.h"
#include "ProjectionTransformer.h"

class OwlExperiment : public Experiment<double>
{
public:
    OwlExperiment() : Experiment<double>()
    {
        model = new Owl();

        addIntegrator( new RungeKutta4(*model, .01) );
        setIntegrator("rk4");
        
        addTransformer( new ProjectionTransformer<double>(*model) );
        setTransformer("projection");      
    }
};

#endif
