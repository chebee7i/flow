#ifndef INTEGRATOR_H
#define INTEGRATOR_H

#include <exception>
#include <string>
#include <iostream>

// Project includes
//
#include <DynamicalModel.h>
#include <Parameter.h>


// Parameters for integrators are implemented via ParameterClass
// rather than as basic members. This is done so that code can work
// generic Integrator objects without needing to perform a dynamic_cast.
// They can simply query the integrator for its parameters.

class IntegratorException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Cannot integrate 0-dimensional model.";
    }
};

template <typename ScalarParam>
class Experiment;

template <typename ScalarParam>
class Integrator : public ParameterClass<ScalarParam>
{

friend class Experiment<ScalarParam>;

public:
    typedef DynamicalModel<ScalarParam> Model;
    typedef typename Model::Parameter Parameter;
    typedef typename Model::Scalar Scalar;
    typedef typename Model::Vector Vector;

    Integrator(Model const& model);
    virtual ~Integrator();

    Vector step(Vector const& v);
    virtual void step(Vector const& v, Vector & out) = 0;

    std::string const& getName() const;
    void setName(std::string const& name);

    unsigned int const& getVersion() const;

protected:
    Model const& model;
    std::string name;

    unsigned int updateVersion();

private:
    unsigned int version;

};

template <typename ScalarParam>
Integrator<ScalarParam>::Integrator(Model const& model)
: model(model),
  name("integrator"),
  version(0)
{
}

template <typename ScalarParam>
Integrator<ScalarParam>::~Integrator()
{
}

template <typename ScalarParam>
typename Integrator<ScalarParam>::Vector
Integrator<ScalarParam>::step(typename Integrator<ScalarParam>::Vector const& v)
{
    typename Integrator<ScalarParam>::Vector out(v.getDimension());
    step(v, out);
    return out;
}

template <typename ScalarParam>
inline
std::string const& Integrator<ScalarParam>::getName() const
{
    return name;
}

template <typename ScalarParam>
inline
void Integrator<ScalarParam>::setName(std::string const& value)
{
    name = value;
}


template <typename ScalarParam>
inline
unsigned int const& Integrator<ScalarParam>::getVersion() const
{
    return version;
}

template <typename ScalarParam>
inline
unsigned int Integrator<ScalarParam>::updateVersion()
{
    return ++version;
}

#endif
