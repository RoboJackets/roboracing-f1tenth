#include "pid_controller.hpp"




PIDController::PIDController(double C, double P, double I, double D, double integral_bound)
{
    this->C = C;
    this->P = P;
    this-> I = I;
    this->D = D;
    this->integral_bound = integral_bound;
}


double PIDController::computePID(double error, double dt)
{   
    integral_error += error;
    double error_delta = (error - prev_error) / dt;
    return C * P * error + integral_error * I + error_delta * D;
}






