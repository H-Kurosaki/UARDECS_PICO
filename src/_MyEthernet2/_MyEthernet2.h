/*
 * MyEthernet2
 * -----------
 * This library is a fork of the Ethernet2 library
 * (https://github.com/adafruit/Ethernet2/tree/master/src),
 * modified by H.kurosaki to add support for the Raspberry Pi Pico
 * in addition to the original AVR Arduino targets.
 *
 * It is intended to be used together with the UARDECS_PICO library,
 * and can be bundled directly inside it (under src/_MyEthernet2/).
 *
 * Modifier: H.kurosaki
 *
 * Summary of changes from the original Ethernet2:
 *   - Added support for the Raspberry Pi Pico (RP2040 / RP2350) in
 *     addition to AVR Arduino.
 *   - Added automatic MAC address handling for WIZ550io-style modules:
 *     the MAC stored in the module is read back and preserved across
 *     the chip soft-reset during init (see WIZ550io_WITH_MACADDRESS and
 *     W5500Class::init).
 *   - Added W5500 error management: a W5500Error status with
 *     getLastError() / clearError() / getErrorString() accessors,
 *     exposed both on W5500Class and as wrappers on EthernetClass.
 *   - Added timeout handling to blocking W5500 access loops
 *     (getTXFreeSize / getRXReceivedSize / execCmdSn) and to the UDP
 *     send / flush paths, to avoid low-frequency freezes.
 *   - Removed dead code: the unreachable non-WIZ550io begin() overloads
 *     (the #else branch, which was never compiled because
 *     WIZ550io_WITH_MACADDRESS is always defined) and the unused,
 *     duplicate src-level _util.h.
 *
 * Changes for bundling inside UARDECS_PICO (under src/_MyEthernet2/):
 *   - All source files were renamed with a leading underscore to avoid
 *     name collisions with other libraries (existing leading underscores
 *     were doubled), e.g. MyEthernet2 -> _MyEthernet2, _Dhcp -> __Dhcp.
 *   - All internal #include paths were fully qualified relative to the
 *     library src root (e.g. "_MyEthernet2/utility/__w5500.h").
 *   - All header include guards were renamed to a unique _MYETH2_*
 *     prefix to prevent guard-name collisions.
 *   - Public class/object names (EthernetClass, EthernetUDP,
 *     EthernetClient, EthernetServer, Ethernet, w5500, ...) are
 *     unchanged, so user-facing API is identical to the original.
 *
 * --- Original Ethernet2 modification history ---
 * modified 12 Aug 2013
 * by Soohwan Kim (suhwan@wiznet.co.kr)
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 *
 * 23 Dec. 2016
 * modified to macaddr auto-setting
 * by H.kurosaki
 */
#ifndef _MYETH2_MYETHERNET2_H
#define _MYETH2_MYETHERNET2_H

#include <inttypes.h>
#include "_MyEthernet2/utility/__w5500.h"
#include "IPAddress.h"
#include "_MyEthernet2/__EthernetClient.h"
#include "_MyEthernet2/__EthernetServer.h"
#include "_MyEthernet2/__Dhcp.h"
//changed to macaddr auto-setting
#define WIZ550io_WITH_MACADDRESS 

class EthernetClass {
private:
  IPAddress _dnsServerAddress;
  DhcpClass* _dhcp;
public:
	
 // W5500エラー管理のラッパー関数を追加(2025/11/05)
  W5500Error getLastError() { return w5500.getLastError(); }
  void clearError() { w5500.clearError(); }
  const char* getErrorString(W5500Error err) { return w5500.getErrorString(err); }
	
	
	
  uint8_t w5500_cspin;

  static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];

  EthernetClass() { _dhcp = NULL; w5500_cspin = 10; }
  void init(uint8_t _cspin = 10) { w5500_cspin = _cspin; }

  // Initialize function when use the ioShield serise (included WIZ550io)
  // WIZ550io has a MAC address which is written after reset.
  // Default IP, Gateway and subnet address are also writen.
  // so, It needs some initial time. please refer WIZ550io Datasheet in details.
  int begin(void);
  void begin(IPAddress local_ip);
  void begin(IPAddress local_ip, IPAddress dns_server);
  void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway);
  void begin(IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet);
  //changed to macaddr auto-setting
  void begin(uint8_t *mac_address, IPAddress local_ip, IPAddress dns_server, IPAddress gateway, IPAddress subnet); 
  
  int maintain();

  IPAddress localIP();
  IPAddress subnetMask();
  IPAddress gatewayIP();
  IPAddress dnsServerIP();

  friend class EthernetClient;
  friend class EthernetServer;
};

extern EthernetClass Ethernet;

#endif
