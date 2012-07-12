
#ifndef DTS_EXPERIMENTFACTORY
#define DTS_EXPERIMENTFACTORY

#include "Experiment.h"

/** Note: maker_t is a typedef to a member function that
 *	returns a pointer to an object of type Experiment<Scalar>.
 *	Then, maker_t* is a pointer to the member function.
 **/
 
typedef double Scalar;
 
typedef Experiment<Scalar>* (maker_t)();

typedef std::map<std::string, maker_t*> ExperimentFactory;

///< Global object for creating dynamical models
extern ExperimentFactory Factory; 

#endif
