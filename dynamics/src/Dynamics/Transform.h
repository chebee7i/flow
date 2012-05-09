#ifndef TRANSFORM_H
#define TRANSFORM_H

template <typename ScalarParam>
class BaseTransform
{

public:

    typedef ScalarParam Scalar;
    typedef DTS::Vector<ScalarParam> Vector;

    BaseTransform();
    virtual ~BaseTransform();

    /*
        The first method allocates space for the output. The second method
        writes to a preallocated vector, which is assumed to be distinct from
        the input vector. There will be problems if v == out.
    */
    Vector transform(Vector const& v);
    virtual void transform(Vector const& v, Vector & out);

};

template <typename ScalarParam>
DTS::Vector<ScalarParam> transform(DTS::Vector<ScalarParam> const& v)
{
    DTS::Vector<ScalarParam> out(3);
    transform(v, out);
    return out;
}

template <typename ScalarParam>
void transform(DTS::Vector<ScalarParam> const& v, DTS::Vector<ScalarParam> & out)
{
    // Take the first three components

    out[0] = v[0];
    out[1] = v[1];
    out[2] = v[2];
}


#endif

