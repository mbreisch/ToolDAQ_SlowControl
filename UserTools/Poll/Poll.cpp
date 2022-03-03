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

  //LV
  int LVstate = m_data->CB->GetLV_ONOFF();
  m_data->SCMonitor.LV_mon = LVstate;

  
  std::vector<float> LVvoltage = m_data->CB->GetLV_voltage();
  m_data->SCMonitor.v33 = LVvoltage[0];
  m_data->SCMonitor.v25 = LVvoltage[1];
  m_data->SCMonitor.v12 = LVvoltage[2];

  //HV
  int HVstate = m_data->CB->GetHV_ONOFF();
  m_data->SCMonitor.HV_mon = HVstate;
  m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;

  //RHT
  vector<float> RHT = m_data->CB->GetTemp();
  m_data->SCMonitor.temperature_mon = RHT[0];
  m_data->SCMonitor.humidity_mon = RHT[1];
  
  //DAC0
  m_data->SCMonitor.Trig0_mon = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);

  //DAC1
  m_data->SCMonitor.Trig1_mon = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);

  //Relay
  m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
  m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
  m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);
  
  //Photodiode
  m_data->SCMonitor.light = m_data->CB->GetPhotodiode();
  
  m_data->SCMonitor.Print();
  usleep(1000000);

  return true;
}


bool Poll::Finalise(){

  m_data->CB->Disconnect();
  delete m_data->CB;
  m_data->CB=0;

  return true;
}
