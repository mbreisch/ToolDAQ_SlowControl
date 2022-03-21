#include "Emergency.h"

Emergency::Emergency():Tool(){}


bool Emergency::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;
  
  m_variables.Get("LIMIT_temperature_low",m_data->SCMonitor.LIMIT_temperature_low );
  m_variables.Get("LIMIT_temperature_high",m_data->SCMonitor.LIMIT_temperature_high );
  m_variables.Get("LIMIT_humidity_low",m_data->SCMonitor.LIMIT_humidity_low );
  m_variables.Get("LIMIT_humidity_high",m_data->SCMonitor.LIMIT_humidity_high );
  m_variables.Get("LIMIT_Thermistor_temperature_low",m_data->SCMonitor.LIMIT_Thermistor_temperature_low);
  m_variables.Get("LIMIT_Thermistor_temperature_high",m_data->SCMonitor.LIMIT_Thermistor_temperature_high);
  
  return true;
}


bool Emergency::Execute(){
  if(m_data->SCMonitor.recieveFlag==0){return true;}
  
  bool retchk;
  retchk = TEMPCHK();
  if(retchk==false)
  {
    //report error behavior 
    //m_data->SCMonitor.errorcodes.push_back(0xCC01EE01);
  }
  retchk = TEMPCHK_Thermistor();
  if(retchk==false)
  {
    //report error behavior 
    //m_data->SCMonitor.errorcodes.push_back(0xCC07EE01);
  }
  retchk = HUMIDITYCHK();
  if(retchk==false)
  {
    //report error behavior
    //m_data->SCMonitor.errorcodes.push_back(0xCC02EE01);
  } 
  retchk = SALTBRIDGECHK();
  if(retchk==false)
  {
    //report error behavior
   // m_data->SCMonitor.errorcodes.push_back(0xCC08EE01);
  }  

  return true;
}


bool Emergency::Finalise(){

  return true;
}

bool Emergency::TEMPCHK(){
  int retval=-2;
  if(m_data->SCMonitor.temperature_mon < m_data->SCMonitor.LIMIT_temperature_low)
  {
    m_data->SCMonitor.FLAG_temperature = 0;
    return true;
  }else if(m_data->SCMonitor.temperature_mon >= m_data->SCMonitor.LIMIT_temperature_low && m_data->SCMonitor.temperature_mon < m_data->SCMonitor.LIMIT_temperature_high)
  {
    //Slow shutdown
    m_data->SCMonitor.FLAG_temperature = 1;
    
    return true;
  }else if(m_data->SCMonitor.temperature_mon >= m_data->SCMonitor.LIMIT_temperature_high)
  {
    int tries = 0;
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(1,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(2,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(3,false); tries++;}
    
    if(tries>=max_tries && retval!=0)
    {
       m_data->SCMonitor.FLAG_temperature = 3;
       //m_data->SCMonitor.errorcodes.push_back(0xCC03EE01);
    }
    
    m_data->SCMonitor.FLAG_temperature = 2;
    
    return true; 
  }else
  {
    return false;
  }
}

 
bool Emergency::HUMIDITYCHK(){
  int retval=-2;
  if(m_data->SCMonitor.humidity_mon < m_data->SCMonitor.LIMIT_humidity_low)
  {
    m_data->SCMonitor.FLAG_humidity = 0;
    return true;
  }else if(m_data->SCMonitor.humidity_mon >= m_data->SCMonitor.LIMIT_humidity_low && m_data->SCMonitor.humidity_mon < m_data->SCMonitor.LIMIT_humidity_high)
  {
    //Slow shutdown
    m_data->SCMonitor.FLAG_humidity = 1;
    
    return true;
  }else if(m_data->SCMonitor.humidity_mon >= m_data->SCMonitor.LIMIT_humidity_high)
  {
    int tries = 0;
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(1,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(2,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(3,false); tries++;}
    
    if(tries>=max_tries && retval!=0)
    {
       m_data->SCMonitor.FLAG_humidity = 3;
       //m_data->SCMonitor.errorcodes.push_back(0xCC04EE01);
    }
    
    m_data->SCMonitor.FLAG_humidity = 2;
    
    return true; 
  }else
  {
    return false;
  }
}  


bool Emergency::TEMPCHK_Thermistor(){
  int retval=-2;
  if(m_data->SCMonitor.temperature_thermistor < m_data->SCMonitor.LIMIT_Thermistor_temperature_low )
  {
    m_data->SCMonitor.FLAG_temperature_Thermistor  = 0;
    return true;
  }else if(m_data->SCMonitor.temperature_thermistor >= m_data->SCMonitor.LIMIT_Thermistor_temperature_low  && m_data->SCMonitor.temperature_thermistor < m_data->SCMonitor.LIMIT_Thermistor_temperature_high )
  {
    //Slow shutdown
    m_data->SCMonitor.FLAG_temperature_Thermistor  = 1;
    
    return true;
  }else if(m_data->SCMonitor.temperature_thermistor >= m_data->SCMonitor.LIMIT_Thermistor_temperature_high )
  {
    int tries = 0;
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(1,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(2,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(3,false); tries++;}
    
    if(tries>=max_tries && retval!=0)
    {
       m_data->SCMonitor.FLAG_temperature_Thermistor  = 3;
       //m_data->SCMonitor.errorcodes.push_back(0xCC05EE01);
    }
    
    m_data->SCMonitor.FLAG_temperature_Thermistor  = 2;
    
    return true; 
  }else
  {
    return false;
  }
}

 
bool Emergency::SALTBRIDGECHK(){
  int retval=-2;
  if(m_data->SCMonitor.saltbridge  < 50000)
  {
    int tries = 0;
    int max_tries = 50;
    //Instant shutdown
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(1,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(2,false); tries++;}
    while(retval!=0 && tries<max_tries) {retval = m_data->CB->SetRelay(3,false); tries++;}
    
    if(tries>=max_tries && retval!=0)
    {
       m_data->SCMonitor.FLAG_saltbridge = 3;
       //m_data->SCMonitor.errorcodes.push_back(0xCC06EE01);
    }
    
    m_data->SCMonitor.FLAG_saltbridge = 2;
    
    return true; 
  }else
  {
     m_data->SCMonitor.FLAG_saltbridge = 0;
    return true;
  }
}  
