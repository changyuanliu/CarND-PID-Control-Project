#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid;
  PID pid_speed;
  /**
   * TODO: Initialize the pid variable.
   */
  pid.Init(0.2, 2e-4, 1.0);
  pid_speed.Init(0.1, 1e-3, 0.0);



  h.onMessage([&pid, &pid_speed](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          double speed = std::stod(j[1]["speed"].get<string>());
          double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          // Speed target and actuator limits
          const double SPEED_TARGET = 20.0;
          const double ANGLE_MAX = 1.0;
          const double ANGLE_MIN = -1.0;
          const double THROTTLE_MAX = 1.0;
          const double THROTTLE_MIN = -0.2;          
          
          // Update errors and compute controller output
          // Since cte=cross_track_error, error used by PID is (0 - cte), which means (setpoint - measurement).
          pid.UpdateError(-cte);
          // Incremental PID
          steer_value += pid.TotalError();
          // Limit check
          if(steer_value < ANGLE_MIN)
          {
            steer_value = ANGLE_MIN;
          }
          else if(steer_value > ANGLE_MAX)
          {
            steer_value = ANGLE_MAX;
          }

          // Speed PID controller
          double throttle_value;
          double speed_setpoint = SPEED_TARGET;
          // Update errors and compute controller output
          double speed_error = speed_setpoint-speed;
          pid_speed.UpdateError(speed_error);
          // Incremental PID
          throttle_value += pid_speed.TotalError();
          // Limit check
          if(throttle_value < THROTTLE_MIN)
          {
            throttle_value = THROTTLE_MIN;
          }
          else if(throttle_value > THROTTLE_MAX)
          {
            throttle_value = THROTTLE_MAX;
          }

          // DEBUG
          std::cout << "CTE: " << cte << " Steering Value: " << steer_value 
                    << std::endl;
          std::cout << "Speed Error: " << speed_error << " Throtthle Value: " << throttle_value 
                    << std::endl;

          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle_value;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}