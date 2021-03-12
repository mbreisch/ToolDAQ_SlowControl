#include "Recieve.h"

Recieve::Recieve():Tool(){}


bool Recieve::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("Port",m_port)) m_port=4444;

  sock=new zmq::socket_t(*(m_data->context), ZMQ_DEALER);
  std::stringstream tmp;
  tmp<<"tcp://127.0.0.1:"<<m_port;
  sock->bind(tmp.str().c_str());
   
  items[0].socket = *sock;
  items[0].fd = 0;
  items[0].events = ZMQ_POLLIN;
  items[0].revents =0;
  
  return true;
}


bool Recieve::Execute(){

  int timer;
  if(m_data->SCMonitor.recieveFlag==0)
  {
    timer = 1000;
  }else
  {
    timer = 10;
  }

  zmq::poll(&items[0], 1, timer);

  if((items [0].revents & ZMQ_POLLIN)) 
  {
    m_data->SCMonitor.Receive_Config(sock);
  } 
  
  return true;
}


bool Recieve::Finalise(){
  delete sock;
  sock=0;

  return true;
}
