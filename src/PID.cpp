#include "PID.h"

/**
 * TODO: Complete the PID class. You may add any additional desired functions.
 */

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_) {
  /**
   * TODO: Initialize PID coefficients (and errors, if needed)
   */
  Kp = Kp_;
  Ki = Ki_;
  Kd = Kd_;

  p_error = 0;
  i_error = 0;
  d_error = 0;
  k_1_error = 0;
  k_2_error = 0;
}

void PID::UpdateError(double new_error) {
  /**
   * TODO: Update PID errors based on cte.
   */
  /*
  d_error = new_error - p_error;
  p_error = new_error;
  i_error += new_error;  
  */
  p_error = new_error - k_1_error;
  i_error = new_error;
  d_error = new_error + k_2_error - 2 * k_1_error;  

  k_2_error = k_1_error;
  k_1_error = new_error;

}

double PID::TotalError() {
  /**
   * TODO: Calculate and return the total error
   */
  
  return Kp*p_error + Ki*i_error + Kd*d_error;  // TODO: Add your total error calc here!
}