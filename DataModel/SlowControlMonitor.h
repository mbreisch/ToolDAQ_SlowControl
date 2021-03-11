#ifndef SlowControlMonitor_H
#define SlowControlMonitor_H

#include "zmq.hpp"
#include <SerialisableObject.h>
#include <iostream>

class SlowControlMonitor : public SerialisableObject{

 friend class boost::serialization::access;

 public:

  SlowControlMonitor();
  
  //Comms
  bool Send_Mon(zmq::socket_t* sock);
  bool Receive_Mon(zmq::socket_t* sock);
  bool Send_Config(zmq::socket_t* sock);
  bool Receive_Config(zmq::socket_t* sock);
  bool RelayControl(zmq::socket_t* sock);
  int recieveFlag = 0;
  std::string ident_string;
  
  //RHT
  float humidity_mon;
  float temperature_mon;
  
  //HV
  int HV_mon;
  bool HV_state_set;
  float HV_volts;

  //LV
  int LV_mon;
  bool LV_state_set;

  //Emergency variables
  float LIMIT_temperature_low = 0;
  float LIMIT_humidity_low = 0;
  float LIMIT_temperature_high = 0;
  float LIMIT_humidity_high = 0;  
  int FLAG_temperature = 0;
  int FLAG_humidity = 0;

  //relay
  bool relayCh1;
  bool relayCh2;
  bool relayCh3;
  bool relayCh1_mon;
  bool relayCh2_mon;
  bool relayCh3_mon;

  //Triggerboard
  float TrigVref;
  float Trig1_threshold;
  float Trig1_mon;
  float Trig0_threshold;
  float Trig0_mon;
  
  bool SetDefaults();
  bool Print();

 private:
 
 template <class Archive> void serialize(Archive& ar, const unsigned int version){

  ar & recieveFlag;
  ar & humidity_mon;
  ar & temperature_mon;
  ar & HV_mon;
  ar & HV_state_set;
  ar & HV_volts;
  ar & LV_mon;
  ar & LV_state_set;
  ar & LIMIT_temperature_low;
  ar & LIMIT_humidity_low;
  ar & LIMIT_temperature_high;
  ar & LIMIT_humidity_high;  
  ar & FLAG_temperature;
  ar & FLAG_humidity;
  ar & Trig1_threshold;
  ar & Trig1_mon;
  ar & Trig0_threshold;
  ar & Trig0_mon;
  ar & TrigVref;
  ar & relayCh1;
  ar & relayCh2;
  ar & relayCh3;
  ar & relayCh1_mon;
  ar & relayCh2_mon;
  ar & relayCh3_mon;
 
 }
 
};

#endif
