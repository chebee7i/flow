
#include "LorenzExperiment.h"
#include "Factory.h"

extern "C"
{
    Experiment<double>* maker()
    {
        return new LorenzExperiment;
    }

    class Proxy
    {
        public:
        Proxy()
        {
            Factory["Lorenz"] = maker;
        }
    };

    Proxy p;
}
