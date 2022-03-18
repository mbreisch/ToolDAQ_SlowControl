#include "Poll.h"

Poll::Poll():Tool(){}


bool Poll::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;
  

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  return true;
}


bool Poll::Execute(){

  if(m_data->SCMonitor.recieveFlag==0){return true;}
  
  //LV
  std::cout << "Polling LV" << std::endl;
  m_data->SCMonitor.LV_mon = m_data->CB->GetLV_ONOFF();

  std::cout << "Polling LV values" << std::endl;
  std::vector<float> LVvoltage = m_data->CB->GetLV_voltage();
  m_data->SCMonitor.v33 = LVvoltage[0];
  m_data->SCMonitor.v25 = LVvoltage[1];
  m_data->SCMonitor.v12 = LVvoltage[2];

  //HV
  std::cout << "Polling HV state" << std::endl;
  m_data->SCMonitor.HV_mon = m_data->CB->GetHV_ONOFF();
  std::cout << "Polling HV value" << std::endl;
  m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;

  //RHT
  std::cout << "Polling RHT" << std::endl;
  vector<float> RHT = m_data->CB->GetTemp();
  m_data->SCMonitor.temperature_mon = RHT[0];
  m_data->SCMonitor.humidity_mon = RHT[1];
  std::cout << "Polling Thermistor" << std::endl;
  m_data->SCMonitor.temperature_thermistor = m_data->CB->GetThermistor();
  
  //DAC0
  std::cout << "Polling Trig0_mon" << std::endl;
  m_data->SCMonitor.Trig0_mon = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);

  //DAC1
  std::cout << "Polling Trig1_mon" << std::endl;
  m_data->SCMonitor.Trig1_mon = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);

  //Relay
  std::cout << "Polling Relays" << std::endl;
  m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
  m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
  m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);
  
  //Photodiode
  std::cout << "Polling Light" << std::endl;
  m_data->SCMonitor.light = m_data->CB->GetPhotodiode();
  
  std::cout << "Polling Saltbridge" << std::endl;
  m_data->SCMonitor.saltbridge = m_data->CB->GetSaltbridge();
  
  m_data->SCMonitor.Print();
  usleep(10000000);

  return true;
}


bool Poll::Finalise(){

  m_data->CB->Disconnect();
  delete m_data->CB;
  m_data->CB=0;

  return true;
}
