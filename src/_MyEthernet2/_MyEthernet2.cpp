/*
 modified 12 Aug 2013
 by Soohwan Kim (suhwan@wiznet.co.kr)

- 10 Apr. 2015
 Added support for Arduino Ethernet Shield 2
 by Arduino.org team

 */
 
#include "_MyEthernet2/_MyEthernet2.h"
#include "_MyEthernet2/__Dhcp.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 0, };



int EthernetClass::begin(void)
{
	clearError();
	
  byte mac_address[6] ={0,};
  if (_dhcp != NULL) {
    delete _dhcp;
  }
  _dhcp = new DhcpClass();

  // Initialise the basic info
  w5500.init(w5500_cspin);
  w5500.setIPAddress(IPAddress(0,0,0,0).raw_address());
  w5500.getMACAddress(mac_address);
  
  // Now try to get our config info from a DHCP server
  int ret = _dhcp->beginWithDHCP(mac_address);
  if(ret == 1)
  {
    // We've successfully found a DHCP server and got our configuration info, so set things
    // accordingly
    w5500.setIPAddress(_dhcp->getLocalIp().raw_address());
    w5500.setGatewayIp(_dhcp->getGatewayIp().raw_address());
    w5500.setSubnetMask(_dhcp->getSubnetMask().raw_address());
    _dnsServerAddress = _dhcp->getDnsServerIp();
  }

  return ret;
}

void EthernetClass::begin(IPAddress local_ip)
{
	clearError();

  // Assume the DNS server will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress dns_server = local_ip;
  dns_server[3] = 1;
  begin(local_ip, dns_server);
}

void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server)
{
	clearError();

  // Assume the gateway will be the machine on the same network as the local IP
  // but with last octet being '1'
  IPAddress gateway = local_ip;
  gateway[3] = 1;
  begin(local_ip, dns_server, gateway);
}

void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway)
{
	clearError();

  IPAddress subnet(255, 255, 255, 0);
  begin(local_ip, dns_server, gateway, subnet);
}

void EthernetClass::begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	clearError();

  w5500.init(w5500_cspin);
  w5500.setIPAddress(local_ip.raw_address());
  w5500.setGatewayIp(gateway.raw_address());
  w5500.setSubnetMask(subnet.raw_address());
  _dnsServerAddress = dns_server;
}

//changed to macaddr auto-setting
void EthernetClass::begin(uint8_t *mac, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet)
{
	clearError();
	
uint8_t inmac[6];

  w5500.init(w5500_cspin);
  w5500.getMACAddress(inmac);

  if(inmac[0]==0&&
  	 inmac[1]==0&&
  	 inmac[2]==0&&
  	 inmac[3]==0&&
  	 inmac[4]==0&&
  	 inmac[5]==0)
  	{
  	//no internal mac address
  	w5500.setMACAddress(mac);
  	}
  else
  	{
  	w5500.getMACAddress(mac);
  	}
  
  w5500.setIPAddress(local_ip.raw_address());
  w5500.setGatewayIp(gateway.raw_address());
  w5500.setSubnetMask(subnet.raw_address());
  _dnsServerAddress = dns_server;
}




int EthernetClass::maintain(){
  int rc = DHCP_CHECK_NONE;
  if(_dhcp != NULL){
    //we have a pointer to dhcp, use it
    rc = _dhcp->checkLease();
    switch ( rc ){
      case DHCP_CHECK_NONE:
        //nothing done
        break;
      case DHCP_CHECK_RENEW_OK:
      case DHCP_CHECK_REBIND_OK:
        //we might have got a new IP.
        w5500.setIPAddress(_dhcp->getLocalIp().raw_address());
        w5500.setGatewayIp(_dhcp->getGatewayIp().raw_address());
        w5500.setSubnetMask(_dhcp->getSubnetMask().raw_address());
        _dnsServerAddress = _dhcp->getDnsServerIp();
        break;
      default:
        //this is actually a error, it will retry though
        break;
    }
  }
  return rc;
}

IPAddress EthernetClass::localIP()
{
  IPAddress ret;
  w5500.getIPAddress(ret.raw_address());
  return ret;
}

IPAddress EthernetClass::subnetMask()
{
  IPAddress ret;
  w5500.getSubnetMask(ret.raw_address());
  return ret;
}

IPAddress EthernetClass::gatewayIP()
{
  IPAddress ret;
  w5500.getGatewayIp(ret.raw_address());
  return ret;
}

IPAddress EthernetClass::dnsServerIP()
{
  return _dnsServerAddress;
}

EthernetClass Ethernet;
