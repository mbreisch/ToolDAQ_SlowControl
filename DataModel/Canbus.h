#ifndef CANBUS_H
#define CANBUS_H

#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <sstream>
#include <vector>
#include <iomanip>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <net/if.h>
#include <fcntl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>
#include <wiringPi.h>

#include "yocto_api.h"
#include "yocto_temperature.h"
#include "yocto_genericsensor.h"

#define CANID_DELIM '#'
#define CAN_DLC 8
#define DELIM "#"
#define DATA_SEPERATOR '.'
#define C40N_MAX 4000.0 // V
#define HV_MAX 3276.0
#define DAC_VMAX 5.0
#define DAC_VREF 2.048
#define DV 50.0 // V
#define RLY1 25
#define RLY2 28
#define RLY3 29
#define TIMEOUT_RS 100
#define CONVERSION 0.00518


using namespace std;

class Canbus{

public:

	Canbus();

	bool Connect(); //done 
	bool Disconnect(); //done
	int SendMessage(unsigned int id, unsigned long long msg);
	char* ReceiveMessage(unsigned int id, unsigned long long msg);

	//LV
	int SetLV(bool state); //done
	int GetLV_ONOFF(); //done
	vector<float> GetLV_voltage(); //calc

	//Light
	float GetPhotodiode(); // calc
	
	//HV
	int SetHV_ONOFF(bool state); //done
	int SetHV_voltage(float volts); //done
	int GetHV_ONOFF(); //done
	float get_HV_volts = 0;
	float ReturnedHvValue=0;

	//RHT
	vector<float> GetTemp(); //done

	//Trigger DAC
	float GetTriggerDac0(float TrigVref);
	float GetTriggerDac1(float TrigVref);
	int SetTriggerDac0(float threshold, float TrigVref);
	int SetTriggerDac1(float threshold, float TrigVref);
	
	//Relay
	int SetRelay(int idx, bool state);
	bool GetRelayState(int idx);


	int s;
	int nbytes;
	int required_mtu;
	struct ifreq ifr;
	struct sockaddr_can addr;
	struct canfd_frame frame;

	unsigned char asc2nib(char c){
		if((c>='0') && (c<='9'))
		{
		  return c - '0';
		}
		if((c>='A') && (c <='F'))
		{
		  return c - 'A' + 10;
		}
		if((c>='a') && (c<='f'))
		{
		  return c - 'a' + 10;
		}
		return 16;
	}

	char* parseFrame(unsigned int id, unsigned long long msg)
	{
		char* t_id = (char *)malloc(64);
		char* t_msg = (char *)malloc(64);
		char* r_frame = (char*)malloc(128);

		stringstream ss;
		ss << std::setfill('0') << std::setw(3) << std::hex << id;

		stringstream ss2;
		ss2 << std::setfill('0') << std::setw(16) << std::hex << msg;

		strcpy(r_frame,ss.str().c_str());
		strcat(r_frame,DELIM);
		strcat(r_frame,ss2.str().c_str());

		//std::cout << r_frame << std::endl;

		return r_frame;
	}


	int createCanFrame(unsigned int id, unsigned long long msg, struct canfd_frame *cf)
	{
		char *t_frame = (char *)malloc(128);
		t_frame = parseFrame(id,msg);

		int i, idx, dlc, len;
		int maxdlen = CAN_MAX_DLEN;
  		int ret = CAN_MTU;
		unsigned char temp;

		len = strlen(t_frame);
		memset(cf, 0, sizeof(*cf));

		if (len<4)
		{
			return 0;
		}
		if (t_frame[3] == CANID_DELIM)
		{
			idx = 4;
			for (i=0; i<3; i++)
			{
		  		if((temp = asc2nib(t_frame[i])) > 0x0F)
		  		{
		    		return 0;
		  		}
		  		cf->can_id |= (temp << (2-i)*4);
			}
		}else if (t_frame[8] == CANID_DELIM)
		{
			idx = 9;
			for (i=0; i<8; i++)
			{
		  		if((temp = asc2nib(t_frame[i])) > 0x0F)
		  		{
				    return 0;
				}
		  		cf->can_id |= (temp << (7-i)*4);
			}
			if(!(cf->can_id & CAN_ERR_FLAG))
			{
		  		cf->can_id |= CAN_EFF_FLAG;
			}
		} else 
		{
			return 0;
		}

		if ((t_frame[idx] == 'R') || (t_frame[idx] == 'r')) // RTR frame
		{
			cf->can_id |= CAN_RTR_FLAG;

			/* check for optional DLC value for CAN 2.0B frames */
			if (t_frame[++idx] && (temp = asc2nib(t_frame[idx])) <= CAN_MAX_DLC)
			cf->len = temp;

			return ret;
		}
		
		if (t_frame[idx] == CANID_DELIM) // CAN FD frame escape char '##'
		{
			maxdlen = CANFD_MAX_DLEN;
			ret = CANFD_MTU;

			/* CAN FD frame <canid>##<flags><data> */
			if ((temp = asc2nib(t_frame[idx+1])) > 0x0F)
			return 0;

			cf->flags = temp;
			idx += 2;
		}
		
		for (i=0, dlc=0; i<maxdlen; i++)
		{
			if(t_frame[idx] == DATA_SEPERATOR)
			{
		  		idx++;
			}
			if(idx >= len)
			{
		  		break;
			}
			if((temp = asc2nib(t_frame[idx++])) > 0x0F)
			{
		  		return 0;
			}
			cf->data[i] = (temp << 4);
			if((temp = asc2nib(t_frame[idx++])) > 0x0F)
			{
		  		return 0;
			}
			cf->data[i] |= temp;
			dlc++;
		}
		cf->len = dlc;

		return ret;
	}

	unsigned int parseResponseID(char* response)
	{
	  unsigned int retID;

	  char ch_id[4];
	  for(int i=0; i<3; i++)
	  {
	    ch_id[i] = response[i];
	  }
	  ch_id[3] = '\0';
	  retID = strtoul(ch_id,nullptr,16);

	  return retID;

	}

	unsigned long long parseResponseMSG(char* response)
	{
	  unsigned long long retMSG;

	  char ch_msg[17];
	  for(int i=0; i<16; i++)
	  {
	    ch_msg[i] = response[i+4];
	  }
	  ch_msg[16] = '\0';
	  retMSG = strtoull(ch_msg,nullptr,16);

	  return retMSG;
	}
};

#endif
