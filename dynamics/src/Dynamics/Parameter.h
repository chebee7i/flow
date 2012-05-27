//
// Module for Parameters and related functionality.
//


#ifndef PARAMETER_H
#define PARAMETER_H

#include <exception>
#include <string>

// This line causes problems when the file is included.
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

template <typename RealParam>
class ParameterClass
{

public:

    typedef ParameterType<bool> BoolParameter;    
    typedef ParameterType<int> IntParameter;
    typedef ParameterType<RealParam> RealParameter;

    typedef std::vector<BoolParameter> BoolParameters;
    typedef std::vector<IntParameter> IntParameters;
    typedef std::vector<RealParameter> RealParameters;
    
    std::vector<BoolParameter> const& getBoolParams() const;
    std::vector<IntParameter> const& getIntParams() const;   
    std::vector<RealParameter> const& getRealParams() const;

    bool getBoolParamValue(std::string const& name) const;    
    int getIntParamValue(std::string const& name) const;
    RealParam getRealParamValue(std::string const& name) const;
    
    int getBoolParamIndex(std::string const& name) const;    
    int getIntParamIndex(std::string const& name) const;
    int getRealParamIndex(std::string const& name) const;

    virtual void printBoolParams() const;
    virtual void printIntParams() const;
    virtual void printRealParams() const;

    // We could just overload setParamValue() but then the API is strange.
    void setBoolParamValue(std::string const& name, bool const value);
    void setIntParamValue(std::string const& name, int const value);    
    void setRealParamValue(std::string const& name, RealParam const value);

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
    std::vector<bool> boolParamValues;    
    std::vector<int> intParamValues;    
    std::vector<RealParam> realParamValues;

    /*
        Use this during construction. It will make sure the various private
        members relating to parameters are consistent.
    */
    // We could just overload addParameter() but then the API is strange.    
    void addBoolParameter(BoolParameter p);    
    void addIntParameter(IntParameter p);    
    void addRealParameter(RealParameter p);

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

    BoolParameters boolParams;
    IntParameters intParams;
    RealParameters realParams;

    // mapping from parameter names to its index in boolParams, etc
    Index boolParamIndex;
    Index intParamIndex;
    Index realParamIndex;

    void _setBoolParamValue(std::string const& name, bool const value);
    void _setIntParamValue(std::string const& name, int const value);
    void _setRealParamValue(std::string const& name, RealParam const value);        
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

template <typename RealParam>
inline
std::vector<ParameterType<bool> > const& ParameterClass<RealParam>::getBoolParams() const
{
    return boolParams;
}

template <typename RealParam>
inline
std::vector<ParameterType<int> > const& ParameterClass<RealParam>::getIntParams() const
{
    return intParams;
}

template <typename RealParam>
inline
std::vector<ParameterType<RealParam> > const& ParameterClass<RealParam>::getRealParams() const
{
    return realParams;
}



template <typename RealParam>
bool ParameterClass<RealParam>::getBoolParamValue(std::string const& name) const
{
    // TODO: throw a ParameterException if the parameter is unknown.
    Index::const_iterator it;
    it = boolParamIndex.find( name );
    unsigned int index = it->second;
    return boolParamValues[index];
}

template <typename RealParam>
int ParameterClass<RealParam>::getIntParamValue(std::string const& name) const
{
    // TODO: throw a ParameterException if the parameter is unknown.
    Index::const_iterator it;
    it = intParamIndex.find( name );
    unsigned int index = it->second;
    return intParamValues[index];
}

template <typename RealParam>
RealParam ParameterClass<RealParam>::getRealParamValue(std::string const& name) const
{
    // TODO: throw a ParameterException if the parameter is unknown.
    Index::const_iterator it;
    it = realParamIndex.find( name );
    unsigned int index = it->second;
    return realParamValues[index];
}






template <typename RealParam>
int ParameterClass<RealParam>::getBoolParamIndex(std::string const& name) const
{
    Index::const_iterator it;
    it = boolParamIndex.find( name );
    if (it == boolParamIndex.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}

template <typename RealParam>
int ParameterClass<RealParam>::getIntParamIndex(std::string const& name) const
{
    Index::const_iterator it;
    it = intParamIndex.find( name );
    if (it == intParamIndex.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}

template <typename RealParam>
int ParameterClass<RealParam>::getRealParamIndex(std::string const& name) const
{
    Index::const_iterator it;
    it = realParamIndex.find( name );
    if (it == realParamIndex.end())
    {
        return -1;
    }
    else
    {
        return it->second;
    }
}





template <typename RealParam>
void ParameterClass<RealParam>::printBoolParams() const
{
    std::cout << "{" << std::endl;
    typename BoolParameters::const_iterator it;
    for (it = boolParams.begin(); it != boolParams.end(); it++)
    {
        std::cout << "\t" << it->name << ": " << it->value << std::endl;
    }
    std::cout << "}" << std::endl;
}

template <typename RealParam>
void ParameterClass<RealParam>::printIntParams() const
{
    std::cout << "{" << std::endl;
    typename IntParameters::const_iterator it;
    for (it = intParams.begin(); it != intParams.end(); it++)
    {
        std::cout << "\t" << it->name << ": " << it->value << std::endl;
    }
    std::cout << "}" << std::endl;
}

template <typename RealParam>
void ParameterClass<RealParam>::printRealParams() const
{
    std::cout << "{" << std::endl;
    typename RealParameters::const_iterator it;
    for (it = realParams.begin(); it != realParams.end(); it++)
    {
        std::cout << "\t" << it->name << ": " << it->value << std::endl;
    }
    std::cout << "}" << std::endl;
}




template <typename RealParam>
void ParameterClass<RealParam>::addBoolParameter( BoolParameter p)
{
    typename Index::iterator it;
    it = boolParamIndex.find(p.name);
    if (it == boolParamIndex.end())
    {
        // Add new parameter
        boolParamIndex[p.name] = boolParams.size();
        boolParams.push_back(p);
        boolParamValues.push_back(p.value);
    }
    else
    {
        // Replace existing parameter
        boolParams[it->second] = p;
        boolParamValues[it->second] = p.value;
    }
}

template <typename RealParam>
void ParameterClass<RealParam>::addIntParameter( IntParameter p)
{
    typename Index::iterator it;
    it = intParamIndex.find(p.name);
    if (it == intParamIndex.end())
    {
        // Add new parameter
        intParamIndex[p.name] = intParams.size();
        intParams.push_back(p);
        intParamValues.push_back(p.value);
    }
    else
    {
        // Replace existing parameter
        intParams[it->second] = p;
        intParamValues[it->second] = p.value;
    }
}

template <typename RealParam>
void ParameterClass<RealParam>::addRealParameter( RealParameter p)
{
    typename Index::iterator it;
    it = realParamIndex.find(p.name);
    if (it == realParamIndex.end())
    {
        // Add new parameter
        realParamIndex[p.name] = realParams.size();
        realParams.push_back(p);
        realParamValues.push_back(p.value);
    }
    else
    {
        // Replace existing parameter
        realParams[it->second] = p;
        realParamValues[it->second] = p.value;
    }
}




template <typename RealParam>
void ParameterClass<RealParam>::_setBoolParamValue(std::string const& name, bool const value)
{
    typename Index::iterator it;
    it = boolParamIndex.find(name);
    if (it != boolParamIndex.end())
    {
        //boolParams[it->second].validate(value);
        boolParams[it->second].value = value;
        boolParamValues[it->second] = value;
    }
}

template <typename RealParam>
void ParameterClass<RealParam>::_setIntParamValue(std::string const& name, int const value)
{
    typename Index::iterator it;
    it = intParamIndex.find(name);
    if (it != intParamIndex.end())
    {
        //intParams[it->second].validate(value);    
        intParams[it->second].value = value;
        intParamValues[it->second] = value;
    }
}

template <typename RealParam>
void ParameterClass<RealParam>::_setRealParamValue(std::string const& name, RealParam const value)
{
    typename Index::iterator it;
    it = realParamIndex.find(name);
    if (it != realParamIndex.end())
    {
        //realParams[it->second].validate(value);
        realParams[it->second].value = value;
        realParamValues[it->second] = value; 
    }
}




template <typename RealParam>
inline
void ParameterClass<RealParam>::setBoolParamValue(std::string const& name, bool const value)
{
    _setBoolParamValue(name, value);
    updateVersion();
}

template <typename RealParam>
inline
void ParameterClass<RealParam>::setIntParamValue(std::string const& name, int const value)
{
    _setIntParamValue(name, value);
    updateVersion();
}

template <typename RealParam>
inline
void ParameterClass<RealParam>::setRealParamValue(std::string const& name, RealParam const value)
{
    _setRealParamValue(name, value);
    updateVersion();
}

#endif

