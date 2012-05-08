#ifndef DTS_EXPERIMENT
#define DTS_EXPERIMENT

#include <exception>

#include <DynamicalModel.h>
#include <Integrator.h>
#include <Transform.h>

class IntegratorException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Integrator exists already.";
    }
};



template <typename ScalarParam>
class Experiment
{
public:
    typedef ScalarParam Scalar;
    typedef std::map< std::string, Integrator<ScalarParam>* > IntegratorMap;

    Experiment();
    virtual ~Experiment();

    // We use pointers so we can more easily change these at runtime.
    DynamicalModel<ScalarParam> *model;
    Integrator<ScalarParam> *integrator;
    BaseTransform<ScalarParam> *transformer;

    void addIntegrator(Integrator<ScalarParam>*);
    void setIntegrator(std::string const&);

protected:
    /*
         A map which holds the available integrators for this experiment.
         These integrators are managed by the Experiment and will be freed
         by its destructor.  The "name" of the integrator is used to populate
         the map. Note that one must perform a dynamic_cast in order to
         access integrator specialized features (like stepSize for rk4).
     */
    IntegratorMap integrators;

};

template <typename ScalarParam>
Experiment<ScalarParam>::Experiment()
 : model(0),
   integrator(0),
   transformer(0)
{
}

template <typename ScalarParam>
Experiment<ScalarParam>::~Experiment()
{
    delete model;

    typename IntegratorMap::iterator it;
    for ( it = integrators.begin(); it != integrators.end(); it++ )
    {
        delete it->second;
    }
}

template <typename ScalarParam>
void Experiment<ScalarParam>::setIntegrator(std::string const& name)
{
    typename IntegratorMap::iterator it = integrators.find(name);
    if ( it != integrators.end() && integrator != it->second )
    {
        // set it if its name exists and is not the existing integrator
        integrator = it->second;
        //updateVersion();
    }
}

template <typename ScalarParam>
inline
void Experiment<ScalarParam>::addIntegrator(Integrator<ScalarParam> *integrator)
{
    std::string name = integrator->getName();

    /* Four cases:

        1) integrator exists under same name
        2) integrator exists under different name
        3) integrator does not exist and name is new
        4) integrator does not exist and name is existing

    */

    typename IntegratorMap::iterator it;
    for ( it = integrators.begin(); it != integrators.end(); it++ )
    {
        if ( it->second == integrator )
        {
            if ( it->first != name )
            {
                // Case 2
                throw IntegratorException();
            }
            else
            {
                // Case 1
                break;
            }
        }
    }

    if ( it == integrators.end() )
    {
        // integrator does not exist

        for ( it = integrators.begin(); it != integrators.end(); it++ )
        {
            if ( it->first == name )
            {
                // Case 4
                delete it->second;
            }
        }
        // Cases 3 and 4
        integrators[name] = integrator;
    }
}

#endif

