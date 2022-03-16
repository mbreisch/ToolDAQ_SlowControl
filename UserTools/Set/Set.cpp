#include "Set.h"

Set::Set():Tool(){}


bool Set::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;
 
  m_data->CB= new Canbus();
  //m_data->CB->Connect();
	
  std::fstream infile("./configfiles/BreakOutBox/LastHV.txt", std::ios_base::in);
  if(infile.is_open())
  {
  	infile >> m_data->CB->get_HV_volts;
  	infile.close();
  }
	
  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;

  return true;
}


bool Set::Execute(){

	LoadConfig();

	if(m_data->SCMonitor.recieveFlag==0){m_data->vars.Set("StopLoop",1);}

	if(m_data->SCMonitor.recieveFlag==1){Setup();}
	
	if(m_data->SCMonitor.recieveFlag==2){return true;}

	return true;
}

bool Set::Finalise(){
/*
	if(m_data->CB->get_HV_volts!=0)
	{
		retval = m_data->CB->SetHV_voltage(0.0);
		if(retval==0)
		{
			m_data->CB->get_HV_volts = 0;
			std::fstream outfile("./configfiles/BreakOutBox/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
			outfile << m_data->CB->get_HV_volts;
			outfile.close();
		}else
		{
			std::cout << " There was an error (HV V set) with retval: " << retval << std::endl;
		}
	}	
*/	
	m_data->CB->get_HV_volts = 0;
  	return true;
}


bool Set::LoadConfig()
{
	m_data->SCMonitor.recieveFlag = 1;
	
	//Load HV 
	m_variables.Get("HV_state_set",m_data->SCMonitor.HV_state_set);
	m_variables.Get("HV_volts",m_data->SCMonitor.HV_volts);
	
	//Load LV
	m_variables.Get("LV_state_set",m_data->SCMonitor.LV_state_set);

	//Load Triggerboard
	m_variables.Get("Trig0_threshold",m_data->SCMonitor.Trig0_threshold);
	m_variables.Get("Trig1_threshold",m_data->SCMonitor.Trig1_threshold);
	m_variables.Get("TrigVref",m_data->SCMonitor.TrigVref);

	//Load Relays
	m_variables.Get("relayCh1",m_data->SCMonitor.relayCh1);
	m_variables.Get("relayCh2",m_data->SCMonitor.relayCh2);
	m_variables.Get("relayCh3",m_data->SCMonitor.relayCh3);
	m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
	m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
	m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);	
	
	//Load Limits
	m_variables.Get("LIMIT_temperature_low",m_data->SCMonitor.LIMIT_temperature_low);
	m_variables.Get("LIMIT_temperature_high",m_data->SCMonitor.LIMIT_temperature_high);
	m_variables.Get("LIMIT_humidity_low",m_data->SCMonitor.LIMIT_humidity_low);
	m_variables.Get("LIMIT_humidity_high",m_data->SCMonitor.LIMIT_humidity_high);
	m_variables.Get("LIMIT_Thermistor_temperature_low",m_data->SCMonitor.LIMIT_Thermistor_temperature_low);
	m_variables.Get("LIMIT_Thermistor_temperature_high",m_data->SCMonitor.LIMIT_Thermistor_temperature_high);
	
	return true;
}

bool Set::Setup()
{
	//------------------------------------Relay 1 Control
	m_data->SCMonitor.relayCh1_mon = m_data->CB->GetRelayState(1);
	if(m_data->SCMonitor.relayCh1!=m_data->SCMonitor.relayCh1_mon)
	{
		retval = m_data->CB->SetRelay(1,m_data->SCMonitor.relayCh1);
		if(retval!=0 && retval!=1)
		{
			std::cout << " There was an error (Relay 1) with retval: " << retval << std::endl;
		}
	}

	//------------------------------------Relay 2 Control
	m_data->SCMonitor.relayCh2_mon = m_data->CB->GetRelayState(2);
	if(m_data->SCMonitor.relayCh2!=m_data->SCMonitor.relayCh2_mon)
	{
		retval = m_data->CB->SetRelay(2,m_data->SCMonitor.relayCh2);
		if(retval!=0 && retval!=1)
		{
			std::cout << " There was an error (Relay 2) with retval: " << retval << std::endl;
		}
	}

	//------------------------------------Relay 3 Control
	m_data->SCMonitor.relayCh3_mon = m_data->CB->GetRelayState(3);
	if(m_data->SCMonitor.relayCh3!=m_data->SCMonitor.relayCh3_mon)
	{
		retval = m_data->CB->SetRelay(3,m_data->SCMonitor.relayCh3);
		if(retval!=0 && retval!=1)
		{
			std::cout << " There was an error (Relay 3) with retval: " << retval << std::endl;
		}
	}  
		
	retval = m_data->CB->SetLV(false);

	//------------------------------------HV Control
	bool tempHVmon;
	int tCB_HV = m_data->CB->GetHV_ONOFF();
	if(tCB_HV==0)
	{
		tempHVmon = false;
		m_data->SCMonitor.HV_mon = 0;
	}else if(tCB_HV==1)
	{
		tempHVmon = true;
		m_data->SCMonitor.HV_mon = 1;
	}  
	if(m_data->SCMonitor.HV_state_set!=tempHVmon)
	{
		retval = m_data->CB->SetHV_ONOFF(m_data->SCMonitor.HV_state_set);
		if(retval!=0 && retval!=1)
		{
			std::cout << " There was an error (Set HV) with retval: " << retval << std::endl;
		}
	}

	int retstate = m_data->CB->GetHV_ONOFF();
	m_data->SCMonitor.HV_return_mon = m_data->CB->ReturnedHvValue;
	if(abs(m_data->CB->get_HV_volts-m_data->SCMonitor.HV_return_mon)>0.5)
	{
		std::cout << "ERROR! " << "File gave " << m_data->CB->get_HV_volts << " Readback gave " << m_data->SCMonitor.HV_return_mon << std::endl;
		std::cout << "Setting them as the read back value" << std::endl;
		m_data->CB->get_HV_volts = m_data->SCMonitor.HV_return_mon;
	}
	if(m_data->SCMonitor.HV_volts!=m_data->CB->get_HV_volts)
	{
		retval = m_data->CB->SetHV_voltage(m_data->SCMonitor.HV_volts);
		if(retval==0)
		{
			m_data->CB->get_HV_volts = m_data->SCMonitor.HV_volts;
			std::fstream outfile("./configfiles/BreakOutBox/LastHV.txt", std::ios_base::out | std::ios_base::trunc);
			outfile << m_data->CB->get_HV_volts;
			outfile.close();
		}else
		{
			std::cout << " There was an error (HV V set) with retval: " << retval << std::endl;
		}
	}
		
	//------------------------------------LV Control
	bool tempLVmon;
	int tCB_LV = m_data->CB->GetLV_ONOFF();
	if(tCB_LV==0)
	{
		tempLVmon = false;
		m_data->SCMonitor.LV_mon = 0;
	}else if(tCB_LV==1)
	{
		tempLVmon = true;
		m_data->SCMonitor.LV_mon = 1;
	}else
	{
		std::cout << "No read came in, retval: " << tCB_LV << std::endl;
	}
	if(m_data->SCMonitor.LV_state_set!=tempLVmon)
	{
		retval = m_data->CB->SetLV(m_data->SCMonitor.LV_state_set);
		if(retval!=0 && retval!=1)
		{
			std::cout << " There was an error (Set LV) with retval: " << retval << std::endl;
		}
	}


	//------------------------------------Triggerboard Control
	float tempval;
	if(m_data->SCMonitor.Trig0_threshold!=m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref))
	{
		retval = m_data->CB->SetTriggerDac0(m_data->SCMonitor.Trig0_threshold, m_data->SCMonitor.TrigVref);
		if(retval!=0)
		{
			std::cout << " There was an error (DAC0) with retval: " << retval << std::endl;
		}
		tempval = m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref);
		if(std::abs(tempval - m_data->SCMonitor.Trig0_threshold)<0.001)
		{
			m_data->SCMonitor.Trig0_mon = tempval;
		}else
		{
			std::cout << " There was an error (DAC0) - 0xC0 hasn't been updated!" << std::endl;
		}
	}

	if(m_data->SCMonitor.Trig1_threshold!=m_data->CB->GetTriggerDac0(m_data->SCMonitor.TrigVref))
	{
		retval = m_data->CB->SetTriggerDac1(m_data->SCMonitor.Trig1_threshold, m_data->SCMonitor.TrigVref);
		if(retval!=0)
		{
			std::cout << " There was an error (DAC1) with retval: " << retval << std::endl;
		}
		tempval = m_data->CB->GetTriggerDac1(m_data->SCMonitor.TrigVref);
		if(std::abs(tempval - m_data->SCMonitor.Trig1_threshold)<0.001)
		{
			m_data->SCMonitor.Trig1_mon = tempval;
		}else
		{
			std::cout << " There was an error (DAC1) - 0xC0 hasn't been updated!" << std::endl;
		}
	} 
	m_data->SCMonitor.recieveFlag=2;

	return true;
}
