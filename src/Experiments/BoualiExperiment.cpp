
#include "BoualiExperiment.h"
#include "Factory.h"

extern "C"
{
    Experiment<double>* maker()
    {
        return new BoualiExperiment;
    }

    class Proxy
    {
        public:
        Proxy()
        {
            Factory["Bouali"] = maker;
        }
    };

    Proxy p;
}
