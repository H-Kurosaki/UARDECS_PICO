/*
 * Copyright (c) 2010 by WIZnet <support@wiznet.co.kr>
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 * - 10 Apr. 2015
 * Added support for Arduino Ethernet Shield 2
 * by Arduino.org team
 */

#include <stdio.h>
#include <string.h>
#include "Arduino.h"

#include "_MyEthernet2/utility/__w5500.h"
//#if defined(W5500_ETHERNET_SHIELD)

// W5500 controller instance
W5500Class w5500;

// SPI details
SPISettings wiznet_SPI_settings(8000000, MSBFIRST, SPI_MODE0);
uint8_t SPI_CS;

void W5500Class::init(uint8_t ss_pin)
{
//Serial.begin(9600);
//char tempstr[30];
  byte mac0[6] ={0,0,0,0,0,0};
  byte mac1[6] ={0,0,0,0,0,0};
  SPI_CS = ss_pin;

  delay(1000);
  initSS();
  SPI.begin();
/*
MACアドレスの退避
Wiz550ioモジュールにはMACアドレスが内蔵されているが、このアドレスはswReset();を実行すると消えてしまう。
そのため、初期化の実行前に一度読み出して退避し、初期化後に書き戻す必要がある。
ただし起動直後のgetMACAddress()はなぜか頻繁に失敗し無意味な値を返す。
このままだと誤ったMACアドレスを読み出してしまうので、2回読んで同じ値が帰ることを確認する。
現時点で、失敗するのは最初の1回だけで2回以降の読み出し失敗の事例は無いと思われる。
*/
for(int i=0;i<20;i++)
	{
	w5500.getMACAddress(mac0);
	w5500.getMACAddress(mac1);
//	sprintf(tempstr, "pos2:%02X%02X%02X%02X%02X%02X",mac0[0],mac0[1],mac0[2],mac0[3],mac0[4],mac0[5]);
//	Serial.println(tempstr);
//	sprintf(tempstr, "pos3:%02X%02X%02X%02X%02X%02X",mac1[0],mac1[1],mac1[2],mac1[3],mac1[4],mac1[5]);
//	Serial.println(tempstr);
		if(mac0[0]==mac1[0]&&mac0[1]==mac1[1]&&mac0[2]==mac1[2]&&mac0[3]==mac1[3]&&mac0[4]==mac1[4]&&mac0[5]==mac1[5])
		{
//		Serial.println("ok");
		break;
		}
//	Serial.println("ng");	
	}
	
  w5500.swReset();
  w5500.setMACAddress(mac0);

	for (int i=0; i<MAX_SOCK_NUM; i++) {
    uint8_t cntl_byte = (0x0C + (i<<5));
    write( 0x1E, cntl_byte, 2); //0x1E - Sn_RXBUF_SIZE
    write( 0x1F, cntl_byte, 2); //0x1F - Sn_TXBUF_SIZE
  }
}

// 修正版 getTXFreeSize
uint16_t W5500Class::getTXFreeSize(SOCKET s)
{
    uint16_t val=0, val1=0;
    unsigned long start = millis();
    uint16_t retry_count = 0;
    
    do {
        val1 = readSnTX_FSR(s);
        if (val1 != 0)
            val = readSnTX_FSR(s);
        
        if (millis() - start > 100 || millis() < start) {
            setError(W5500_ERR_TIMEOUT_TX_SIZE);
            return 0;
        }
        
        retry_count++;
    } 
    while (val != val1 && retry_count < 1000);
    
    if (retry_count >= 1000) {
        setError(W5500_ERR_TIMEOUT_TX_SIZE);
        return 0;
    }
    
    return val;
}


// 修正版 getRXReceivedSize
uint16_t W5500Class::getRXReceivedSize(SOCKET s)
{
    uint16_t val=0, val1=0;
    unsigned long start = millis();
    uint16_t retry_count = 0;
    
    do {
        val1 = readSnRX_RSR(s);
        if (val1 != 0)
            val = readSnRX_RSR(s);
        
        // タイムアウトチェック
        if (millis() - start > 100 || millis() < start) {
            setError(W5500_ERR_TIMEOUT_RX_SIZE);  // エラー設定
            return 0;
        }
        
        retry_count++;
    } 
    while (val != val1 && retry_count < 1000);  // 追加の安全策
    
    if (retry_count >= 1000) {
        setError(W5500_ERR_TIMEOUT_RX_SIZE);
        return 0;
    }
    
    return val;
}


void W5500Class::send_data_processing(SOCKET s, const uint8_t *data, uint16_t len)
{
  // This is same as having no offset in a call to send_data_processing_offset
  send_data_processing_offset(s, 0, data, len);

}

void W5500Class::send_data_processing_offset(SOCKET s, uint16_t data_offset, const uint8_t *data, uint16_t len)
{
    uint16_t ptr = readSnTX_WR(s);
    uint8_t cntl_byte = (0x14+(s<<5));
    ptr += data_offset;
    write(ptr, cntl_byte, data, len);
    ptr += len;
    writeSnTX_WR(s, ptr);
}

void W5500Class::recv_data_processing(SOCKET s, uint8_t *data, uint16_t len, uint8_t peek)
{
    uint16_t ptr;
    ptr = readSnRX_RD(s);

    read_data(s, ptr, data, len);
    if (!peek)
    {
        ptr += len;
        writeSnRX_RD(s, ptr);
    }
}

void W5500Class::read_data(SOCKET s, volatile uint16_t src, volatile uint8_t *dst, uint16_t len)
{
    uint8_t cntl_byte = (0x18+(s<<5));
    read((uint16_t)src , cntl_byte, (uint8_t *)dst, len);
}

uint8_t W5500Class::write(uint16_t _addr, uint8_t _cb, uint8_t _data)
{
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();  
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    SPI.transfer(_data);
    resetSS();
    SPI.endTransaction();

    return 1;
}

uint16_t W5500Class::write(uint16_t _addr, uint8_t _cb, const uint8_t *_buf, uint16_t _len)
{
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i=0; i<_len; i++){
        SPI.transfer(_buf[i]);
    }
    resetSS();
    SPI.endTransaction();

    return _len;
}

uint8_t W5500Class::read(uint16_t _addr, uint8_t _cb)
{
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    uint8_t _data = SPI.transfer(0);
    resetSS();
    SPI.endTransaction();

    return _data;
}

uint16_t W5500Class::read(uint16_t _addr, uint8_t _cb, uint8_t *_buf, uint16_t _len)
{ 
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer(_addr >> 8);
    SPI.transfer(_addr & 0xFF);
    SPI.transfer(_cb);
    for (uint16_t i=0; i<_len; i++){
        _buf[i] = SPI.transfer(0);
    }
    resetSS();
    SPI.endTransaction();

    return _len;
}

// 修正版 execCmdSn
void W5500Class::execCmdSn(SOCKET s, SockCMD _cmd) {
    writeSnCR(s, _cmd);
    
    unsigned long start = millis();
    uint16_t retry_count = 0;
    
    while (readSnCR(s)) {
        if (millis() - start > 100 || millis() < start) {
            setError(W5500_ERR_TIMEOUT_CMD);
            // 強制クローズ
            writeSnCR(s, Sock_CLOSE);
            return;
        }
        
        if (++retry_count > 10000) {
            setError(W5500_ERR_TIMEOUT_CMD);
            writeSnCR(s, Sock_CLOSE);
            return;
        }
        
        delayMicroseconds(10);
    }
}

uint8_t W5500Class::readVersion(void)
{
    SPI.beginTransaction(wiznet_SPI_settings);
    setSS();
    SPI.transfer( 0x00 );
    SPI.transfer( 0x39 );
    SPI.transfer( 0x01);
    uint8_t _data = SPI.transfer(0);
    resetSS();
    SPI.endTransaction();

    return _data;
}
//---------------エラー処理追加(2025/11/04kurosaki)

// 静的メンバーの初期化
volatile W5500Error W5500Class::_lastError = W5500_ERR_NONE;

// エラー設定（内部用）
void W5500Class::setError(W5500Error err) {
  _lastError = err;
}

// 最後のエラーを取得
W5500Error W5500Class::getLastError() {
  return _lastError;
}

// エラーをクリア
void W5500Class::clearError() {
  _lastError = W5500_ERR_NONE;
}

// エラーコードを文字列に変換
const char* W5500Class::getErrorString(W5500Error err) {
  switch(err) {
    case W5500_ERR_NONE: return "No error";
    case W5500_ERR_TIMEOUT_RX_SIZE: return "RX size read timeout";
    case W5500_ERR_TIMEOUT_TX_SIZE: return "TX size read timeout";
    case W5500_ERR_TIMEOUT_CMD: return "Command execution timeout";
    case W5500_ERR_SPI_FAILURE: return "SPI communication failure";
    case W5500_ERR_TIMEOUT_OTHER: return "timeout failure";
    default: return "Unknown error";
  }
}


//#endif
