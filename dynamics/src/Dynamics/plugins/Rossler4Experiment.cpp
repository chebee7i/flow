
#include "Rossler4Experiment.h"
#include "Factory.h"

extern "C"
{
    Experiment<double>* maker()
    {
        return new Rossler4Experiment;
    }

    class Proxy
    {
        public:
        Proxy()
        {
            Factory["Rossler4"] = maker;
        }
    };

    Proxy p;
}
