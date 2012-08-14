
#include "OwlExperiment.h"
#include "Factory.h"

extern "C"
{
    Experiment<double>* maker()
    {
        return new OwlExperiment;
    }

    class Proxy
    {
        public:
        Proxy()
        {
            Factory["Owl"] = maker;
        }
    };

    Proxy p;
}
