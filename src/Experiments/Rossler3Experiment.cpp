
#include "Rossler3Experiment.h"
#include "Factory.h"

extern "C"
{
    Experiment<double>* maker()
    {
        return new Rossler3Experiment;
    }

    class Proxy
    {
        public:
        Proxy()
        {
            Factory["Rossler"] = maker;
        }
    };

    Proxy p;
}
