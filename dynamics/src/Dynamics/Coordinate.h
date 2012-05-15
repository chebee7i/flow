//
// Module for Coordinates and related functionality.
//

#ifndef COORDINATE_H
#define COORDINATE_H

#include <exception>
#include <string>

typedef std::map< std::string, unsigned int > Index;

//
// CoordinateType
//

template <typename ScalarParam>
struct CoordinateType
{
    typedef ScalarParam Scalar;

    std::string name;

    // These are suggestions only, nothing is enforced.
    Scalar defaultValue;
    Scalar minValue;
    Scalar maxValue;


    // Default constructor
    CoordinateType();

    // Standard constructor
    CoordinateType(std::string name, Scalar defaultValue, Scalar minValue, Scalar maxValue);

    // Copy constructor
    template <typename SParam>
    CoordinateType( CoordinateType<SParam> const& coord );
};

//
// CoordinateClass
//

template <typename ScalarParam>
class CoordinateClass
{

public:

    typedef CoordinateType<ScalarParam> Coordinate;
    typedef std::vector<Coordinate> Coordinates;

    std::vector<Coordinate> const& getCoords() const;
    virtual void printCoords() const;

protected:

    /*
        Use this during construction. It will make sure the various private
        members relating to coordinates are consistent.
    */
    void addCoordinate(Coordinate c);

private:

    Coordinates coords;

    // mapping from coordinate names to its index in 'coords'.
    Index coordIndex;
};




/**
 * Implementations
 */

//
// CoordinateType
//

template <typename ScalarParam>
CoordinateType<ScalarParam>::CoordinateType()
    : name(""),
      defaultValue(Scalar()),
      minValue(Scalar()),
      maxValue(Scalar())
{
}

template <typename ScalarParam>
CoordinateType<ScalarParam>::CoordinateType(std::string name,
                                            Scalar defaultValue,
                                            Scalar minValue,
                                            Scalar maxValue)
    : name(name),
      defaultValue(defaultValue),
      minValue(minValue),
      maxValue(maxValue)
{
}

template <typename ScalarParam>
template <typename SParam>
CoordinateType<ScalarParam>::CoordinateType(CoordinateType<SParam> const& coord)
    : name(coord.name),
      defaultValue(coord.defaultValue),
      minValue(coord.minValue),
      maxValue(coord.maxValue)
{
}


//
// CoordinateClass
//

template <typename ScalarParam>
inline
std::vector<CoordinateType<ScalarParam> > const& CoordinateClass<ScalarParam>::getCoords() const
{
    return coords;
}

template <typename ScalarParam>
void CoordinateClass<ScalarParam>::printCoords() const
{
}

template <typename ScalarParam>
void CoordinateClass<ScalarParam>::addCoordinate( CoordinateType<ScalarParam> c)
{
    typename Index::iterator it;
    it = coordIndex.find(c.name);
    if (it == coordIndex.end())
    {
        // Add new coordinate
        coordIndex[c.name] = coords.size();
        coords.push_back(c);
    }
    else
    {
        // Replace existing coordinate
        coords[it->second] = c;
    }
}



#endif
