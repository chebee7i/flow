
#include <algorithm>
#include <iostream>

#include <Lorenz.h>
#include <RungeKutta4.h>
/*
int raw()
{
    Lorenz x;
    RungeKutta4 rk4 = RungeKutta4(x, .01);
    std::cout << rk4.getName() << std::endl;
    DTS::Vector<double> v(3);
    v[0] = 0;
    v[1] = 2;
    v[2] = 0;
    std::cout << "pre";
    std::cout << "\n" << v << std::endl;
    DTS::Vector<double> q = v + rk4.step( v );
    std::cout << "post";
    std::cout << "\n" << q << std::endl;
    x.printParams();
    std::cout << "dims: " << x.getDimension() << std::endl;
}
*/

int raw2()
{
    typedef double Scalar;
    
    Experiment<Scalar> *x = new LorenzExperiment();
    RungeKutta4 *rk4 = dynamic_cast<RungeKutta4*>(x->integrator);
    rk4->setParamValue("stepSize", .001);
    std::cout << x->integrator->getName() << std::endl;
    DTS::Vector<Scalar> v(3);
    v[0] = .1;
    v[1] = 2;
    v[2] = 0;
    std::cout << "pre";
    std::cout << "\n" << v << std::endl;
    DTS::Vector<Scalar> q = v + x->integrator->step( v );
    std::cout << "post";
    std::cout << "\n" << q << std::endl;
    x->model->printParams();
    std::cout << "dims: " << x->model->getDimension() << std::endl;
    
    delete x;
}

int main() 
{ 
    raw2();
    return 0; 
}
