
#include <algorithm>
#include <iostream>

//#include "RungeKutta4.h"
#include "plugins/LorenzExperiment.h"

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

void raw2()
{
    typedef double Scalar;
    
    Experiment<Scalar> *x = new LorenzExperiment();
//    RungeKutta4 *rk4 = dynamic_cast<RungeKutta4*>(x->integrator);
//    rk4->setRealParamValue("stepSize", .001);
    x->setIntegrator("rk4");
    std::cout << x->integrator->getName() << std::endl;
    
    
    std::cout << "New Vector\n";
    DTS::Vector<Scalar> v(4), v2(4);
    v[0] = 1;
    v[1] = 1;
    v[2] = 1;
    v[3] = 0;

    for (int i =0; i < 100000000; i++) {        
//    for (int i =0; i < 10; i++) {        
        x->integrator->step( v, v2 );
        v2 += v;
        v = v2;
    }
    std::cout << v2 << std::endl;
    x->model->printRealParams();
    
    delete x;

}

int main() 
{ 
    raw2();
    return 0; 
}
