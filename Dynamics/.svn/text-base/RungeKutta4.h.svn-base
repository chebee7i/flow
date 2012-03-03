/***********************************************************************
RungeKutta4 - Class to integrate ordinary differential equations using a
fourth-order fixed-step Runge-Kutta method.
Copyright (c) 2006 Oliver Kreylos
***********************************************************************/

#ifndef RUNGEKUTTA4_INCLUDED
#define RUNGEKUTTA4_INCLUDED

#include "Integrator.h"
#include "IntegrationStepSize.h"

template <class VectorFieldParam>
class RungeKutta4
{
  /* Embedded classes: */
 public:
  typedef VectorFieldParam VectorField; // Type of vector field evaluation functor
  typedef typename Integrator::Scalar Scalar; // Scalar type of underlying point space
  typedef typename Integrator::Point Point; // Point type
  typedef typename Integrator::Vector Vector; // Vector type

  void setStepSize(Scalar value) { stepSize = value; }

  /* Elements: */
 private:
  const VectorField& vectorField; // The vector field
  mutable Scalar stepSize; // The integration step size       

  /* Constructors and destructors: */
 public:
  RungeKutta4(const VectorField& sVectorField,Scalar sStepSize)
    :vectorField(sVectorField),
    stepSize(sStepSize)
    {
    };
	
  /* Methods: */
  Vector step(const Point& p) const // Computes one Runge-Kutta integration step vector
    {
      /* Get the integration step size from singleton: */
      stepSize = IntegrationStepSize::instance()->getValue();

      /* Calculate first half-step vector: */
      Vector v0=vectorField(p);
      v0*=stepSize*Scalar(0.5);
		
      /* Move to second evaluation point: */
      Point p1=p;
      p1+=v0;
		
      /* Calculate second half-step vector: */
      Vector v1=vectorField(p1);
      v1*=stepSize*Scalar(0.5);
		
      /* Move to third evaluation point: */
      Point p2=p;
      p2+=v1;
		
      /* Calculate third half-step vector: */
      Vector v2=vectorField(p2);
      v2*=stepSize;
		
      /* Move to fourth evaluation point: */
      Point p3=p;
      p3+=v2;
		
      /* Calculate third half-step vector: */
      Vector v3=vectorField(p3);
      v3*=stepSize;
		
      /* Calculate step vector: */
      v1*=Scalar(2);
      v2+=v1;
      v2+=v0;
      v2*=Scalar(2);
      v3+=v2;
      v3/=Scalar(6);
		
      return v3;
    };
};

#endif
