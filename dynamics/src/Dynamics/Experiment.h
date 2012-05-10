#ifndef DTS_EXPERIMENT
#define DTS_EXPERIMENT

#include <exception>

#include <DynamicalModel.h>
#include <Integrator.h>
#include <Transformer.h>

class IntegratorUnknownException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Unknown integrator.";
    }
};

class IntegratorExistsException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Integrator exists already.";
    }
};

class IntegratorWrongModelException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "integrator model != experiment model.";
    }
};

class TransformerUnknownException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Unknown transformer.";
    }
};

class TransformerExistsException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Transformer exists already.";
    }
};

class TransformerWrongModelException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "transformer model != experiment model.";
    }
};



/** Base class for experiments.
 *
 * The Experiment class defines the interface for the various dynamical
 * systems, integrators, coordinate transformations, etc.  Experiments 
 * must also register themselves with the Factory object. This enables the
 * experiments to be compiled as plugins and dynamically loaded by
 * the main Vrui application without the user even being aware of the details
 * regarding the various systems (or even the number of systems).
 *
 * The following code demonstrates how to register a class constructor with
 * the factory (taken from auto-generated LorenzExperiment.cpp code):
 * \code
extern "C"
{
    Experiment<Scalar>* maker()
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
 * \endcode
 * Of course the dynamical systems code will, as a general rule, be automatically
 * generated prior to compiling. So the user should not have to worry about
 * implementing this.
 */

template <typename ScalarParam>
class Experiment
{
public:
    typedef ScalarParam Scalar;
    typedef std::map< std::string, Integrator<ScalarParam>* > IntegratorMap;
    typedef std::map< std::string, Transformer<ScalarParam>* > TransformerMap;    

    Experiment();
    virtual ~Experiment();
    
    void setIntegrator(std::string const&);
    void setTransformer(std::string const&);
    
    void addIntegrator(Integrator<ScalarParam>*);
    void addTransformer(Transformer<ScalarParam>*);    

    bool hasChanged();

protected:
    // We use pointers so we can more easily change these at runtime.
    // However, the dynamical model should be treated as a const pointer.
    // Changing where it points is bad since the integrator and transformer
    // store a reference to the model.
    DynamicalModel<ScalarParam> * model;
    Integrator<ScalarParam> *integrator;
    Transformer<ScalarParam> *transformer;

    /*
         A map which holds the available integrators for this experiment.
         These integrators are managed by the Experiment and will be freed
         by its destructor.  The "name" of the integrator is used to populate
         the map.
     */
    IntegratorMap integrators;
    TransformerMap transformers;    
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
    
    if ( it == integrators.end() ) throw IntegratorUnknownException();
    
    if ( integrator != it->second )
    {
        // set it if its name exists and is not the existing integrator
        integrator = it->second;
        //updateVersion();
    }
}

template <typename ScalarParam>
void Experiment<ScalarParam>::setTransformer(std::string const& name)
{
    typename TransformerMap::iterator it = transformers.find(name);
    
    if ( it == transformers.end() ) throw TransformerUnknownException();
    
    if ( transformer != it->second )
    {
        // set it if its name exists and is not the existing transformer
        transformer = it->second;
        //updateVersion();
    }
}

template <typename ScalarParam>
inline
void Experiment<ScalarParam>::addIntegrator(Integrator<ScalarParam> *integrator)
{
    std::string name = integrator->getName();
    
    if ( &(integrator->model) != model )
    {
        throw IntegratorWrongModelException();
    }

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
                throw IntegratorExistsException();
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

template <typename ScalarParam>
inline
void Experiment<ScalarParam>::addTransformer(Transformer<ScalarParam> *transformer)
{
    std::string name = transformer->getName();
    
    if ( &(transformer->model) != model )
    {
        throw TransformerWrongModelException();
    }

    /* Four cases:

        1) transformer exists under same name
        2) transformer exists under different name
        3) transformer does not exist and name is new
        4) transformer does not exist and name is existing

    */

    typename TransformerMap::iterator it;
    for ( it = transformers.begin(); it != transformers.end(); it++ )
    {
        if ( it->second == transformer )
        {
            if ( it->first != name )
            {
                // Case 2
                throw TransformerExistsException();
            }
            else
            {
                // Case 1
                break;
            }
        }
    }

    if ( it == transformers.end() )
    {
        // transformer does not exist

        for ( it = transformers.begin(); it != transformers.end(); it++ )
        {
            if ( it->first == name )
            {
                // Case 4
                delete it->second;
            }
        }
        // Cases 3 and 4
        transformers[name] = transformer;
    }
}


template <typename ScalarParam>
bool Experiment<ScalarParam>::hasChanged()
{
    return true;
}


#endif

