#include "rclcpp/rclcpp.hpp"
#include <algorithm>

class PIDController {

    public:
        PIDController(double C, double P, double I, double D, double integral_bound);
        double computePID(double error, double dt);

    private:
        double C;
        double P;
        double I;
        double D;
        double integral_error = 0.0;
        double integral_bound;
        double prev_error = 0.0;


};