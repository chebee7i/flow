//
// Module for Parameters and related functionality.
//

/*
    With the CoordinateClass (or ParameterClass), all coordinates
    (or all parameters) must be of the same type.  This is just how it
    was implemented and we can generalize this if need be.
*/


#ifndef PARAMETER_H
#define PARAMETER_H

#include <exception>
#include <memory>
#include <string>

typedef std::map< std::string, unsigned int > Index;

//
// Exceptions
//

class ParameterException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Unknown parameter.";
    }
};

class RangeException: public std::exception
{
    virtual const char* what() const throw()
    {
        return "Parameter value is outside its range.";
    }
};

//
// ParameterType
//

template <typename ScalarParam>
struct ParameterType
{
    typedef ScalarParam Scalar;
    typedef std::auto_ptr<ParameterType<ScalarParam> > Ptr;

    std::string name;
    Scalar value;

    Scalar minValue;
    Scalar maxValue;
    Scalar defaultValue;
    Scalar increment;

    // Default constructor
    ParameterType();

    // Standard constructor
    ParameterType(
        std::string name,
        Scalar value,
        Scalar minValue,
        Scalar maxValue,
        Scalar defaultValue,
        Scalar increment
    );

    // Copy constructor
    template <typename SParam>
    ParameterType( ParameterType<SParam> const& param );

    void validate(Scalar v);
};

//
// ParameterClass
//

template <typename ScalarParam>
class ParameterClass
{

public:

    typedef ParameterType<ScalarParam> Parameter;
    typedef std::vector<Parameter> Parameters;

    std::vector<Parameter> const& getParams() const;
    ScalarParam getParamValue(std::string const& name) const;
    virtual void printParams() const;

    void setParamValue(std::string const& name, ScalarParam const value);

protected:

    /*
        We store the current parameter values in a vector for quick lookup.
        The API guaranetees that this remains consistent whenever there is
        a change to a parameter value via _setParamValue. This vector should
        never be modified directly, and always accessed in a read-only manner.

        The reasoning behind this is that we will call the functor methods
        frequently, and they require access to the current parameter values.
        Thus, we provide optimized access to the parameter values.
    */
    std::vector<ScalarParam> paramValues;

    /*
        Use this during construction. It will make sure the various private
        members relating to parameters are consistent.
    */
    void addParameter(Parameter p);

    /*
        This is pure rather than implemented now since we want derived classes
        to be able to manage their own versions.  All we demand is that the
        version be some unsigned int.  It could, for example, be updated due
        to some other non-parameter change.

        Also, note that we only track changes to the parameter values, not
        their names, min/max values, default values, or even number of
        parameters.  These attributes are assumed fixed once the instance is
        created and initialized. If you want to deal with such modifications,
        more work will be necessary.
    */
    virtual unsigned int updateVersion() = 0;

private:

    Parameters params;

    // mapping from parameter names to its index in 'params'.
    Index paramIndex;

    void _setParamValue(std::string const& name, ScalarParam value);
};



/**
 * Implementations
 */



//
// ParameterType
//

template <typename ScalarParam>
ParameterType<ScalarParam>::ParameterType()
    : name(""),
      value(Scalar()),
      minValue(Scalar()),
      maxValue(Scalar()),
      defaultValue(Scalar()),
      increment(Scalar())
{
}

template <typename ScalarParam>
ParameterType<ScalarParam>::ParameterType(
    std::string name,
    Scalar value,
    Scalar minValue,
    Scalar maxValue,
    Scalar defaultValue,
    Scalar increment)
    : name(name),
      value(value),
      minValue(minValue),
      maxValue(maxValue),
      defaultValue(defaultValue),
      increment(increment)
{
    validate(value);
    validate(defaultValue);
}

template <typename ScalarParam>
template <typename SParam>
ParameterType<ScalarParam>::ParameterType( ParameterType<SParam> const& param )
    : name(param.name),
      value(param.value),
      minValue(param.minValue),
      maxValue(param.maxValue),
      defaultValue(param.defaultValue),
      increment(param.increment)
{
}

template <typename ScalarParam>
void ParameterType<ScalarParam>::validate(Scalar v)
{
    if (v < minValue || v > maxValue)
    {
        throw RangeException();
    }
}

//
// ParameterClass
//

template <typename ScalarParam>
inline
std::vector<ParameterType<ScalarParam> > const& ParameterClass<ScalarParam>::getParams() const
{
    return params;
}

template <typename ScalarParam>
ScalarParam ParameterClass<ScalarParam>::getParamValue(std::string const& name) const
{
    // TODO: throw a ParameterException if the parameter is unknown.
    Index::const_iterator it;
    it = paramIndex.find( name );
    unsigned int index = it->second;
    return paramValues[index];
}

template <typename ScalarParam>
void ParameterClass<ScalarParam>::printParams() const
{
    std::cout << "{" << std::endl;
    typename Parameters::const_iterator it;
    for (it = params.begin(); it != params.end(); it++)
    {
        std::cout << "\t" << it->name << ": " << it->value << std::endl;
    }
    std::cout << "}" << std::endl;
}

template <typename ScalarParam>
void ParameterClass<ScalarParam>::addParameter( ParameterType<ScalarParam> p)
{
    typename Index::iterator it;
    it = paramIndex.find(p.name);
    if (it == paramIndex.end())
    {
        // Add new parameter
        paramIndex[p.name] = params.size();
        params.push_back(p);
        paramValues.push_back(p.value);
    }
    else
    {
        // Replace existing parameter
        params[it->second] = p;
        paramValues[it->second] = p.value;
    }
}

template <typename ScalarParam>
void ParameterClass<ScalarParam>::_setParamValue(std::string const& name, ScalarParam value)
{
    typename Index::iterator it;
    it = paramIndex.find(name);
    if (it != paramIndex.end())
    {
        params[it->second].value = value;
        paramValues[it->second] = value;
    }
}

template <typename ScalarParam>
inline
void ParameterClass<ScalarParam>::setParamValue(std::string const& name, ScalarParam const value)
{
    _setParamValue(name, value);
    updateVersion();
}

#endif

