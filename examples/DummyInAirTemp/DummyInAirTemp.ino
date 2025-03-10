/////////////////////////////////////////
//UARDECS Sample Program "DummyInAirTemp"
//UECS ccm "InAirTemp" sending test Ver1.4
//By H.kurosaki 1024/4/22
//////////////////////////////////////////
//[概要]
//Web上から任意の値を指定し、InAirTempとして出力します。
//コンパイル環境の構築や他機種からソースコード移植を行う場合の注意点を一番後ろに記載しています

#include <Uardecs_pico.h>

/////////////////////////////////////
//IP reset jupmer pin setting
//IPアドレスリセット用ジャンパーピン設定
/////////////////////////////////////

const byte U_InitPin = 3;
const byte U_InitPin_Sense=HIGH;//ソフトウェア上でSafeModeを抜けたい場合はLOWにしてください

////////////////////////////////////
//Node basic infomation
///////////////////////////////////
const char U_name[] PROGMEM= "UARDECS Node v.1.1";//MAX 20 chars
const char U_vender[] PROGMEM= "XXXXXXXX Co.";//MAX 20 chars
const char U_uecsid[] PROGMEM= "000000000000";//12 chars fixed
const char U_footnote[] PROGMEM= "UARDECS Sample Program DummyInAirTemp";
//const int U_footnoteLetterNumber = 48;//Abolished after Ver 0.6
char U_nodename[20] = "Sample";//MAX 19chars
UECSOriginalAttribute U_orgAttribute;
//////////////////////////////////
// html page1 setting
//////////////////////////////////
const int U_HtmlLine = 1; //Total number of HTML table rows.
const char NAME2[] PROGMEM= "SetTemp";

const char CENT[] PROGMEM= "C";
const char NONES[] PROGMEM= "";
const char note1[] PROGMEM= "Set output temp";

signed long setTemp;
const char** dummy = NULL;

struct UECSUserHtml U_html[U_HtmlLine]={
{NAME2, UECSINPUTDATA, CENT, note1, dummy, 0, &(setTemp), -100, 1000, 1},
};

//////////////////////////////////
// UserCCM setting
//////////////////////////////////

//define CCMID for identify
//CCMID_dummy must put on last
enum {
CCMID_InAirTemp,
CCMID_cnd,
CCMID_dummy,
};


const int U_MAX_CCM = CCMID_dummy;
UECSCCM U_ccmList[U_MAX_CCM];


const char ccmNameTemp[] PROGMEM= "Temperature";
const char ccmTypeTemp[] PROGMEM= "InAirTemp";
const char ccmUnitTemp[] PROGMEM= "C";

const char ccmNameCnd[] PROGMEM= "NodeCondition";
const char ccmTypeCnd[] PROGMEM= "cnd.mIC";
const char ccmUnitCnd[] PROGMEM= "";


void UserInit(){
//MAC address is printed on sticker of Ethernet Shield.
//You must assign unique MAC address to each nodes.
//MACアドレス設定、必ずEthernet Shieldに書かれた値を入力して下さい。
//全てのノードに異なるMACアドレスを設定する必要があります。
U_orgAttribute.mac[0] = 0x11;
U_orgAttribute.mac[1] = 0x22;
U_orgAttribute.mac[2] = 0x33;
U_orgAttribute.mac[3] = 0x44;
U_orgAttribute.mac[4] = 0x55;
U_orgAttribute.mac[5] = 0x66;

//Set ccm list
UECSsetCCM(true, CCMID_InAirTemp, ccmNameTemp, ccmTypeTemp, ccmUnitTemp, 29, 1, A_1S_0);
UECSsetCCM(true,  CCMID_cnd      , ccmNameCnd , ccmTypeCnd , ccmUnitCnd , 29, 0, A_10S_0);
}


void OnWebFormRecieved(){
  U_ccmList[CCMID_InAirTemp].value=setTemp;
}

void UserEverySecond(){}
void UserEveryMinute(){}
void UserEveryLoop(){}

void loop(){
UECSloop();
}


void setup(){
UECSsetup();
U_ccmList[CCMID_InAirTemp].value=setTemp;
}


/*
(1)コンパイル環境の構築(Arduino IDE1.8.19で確認済み)
	(a)RP2040用ボードライブラリのインストール
	ArduinoIDEを起動しファイル→環境設定→追加のボードマネージャのURLに以下のURLを入力します
	https://github.com/WIZnet-ArduinoEthernet/arduino-pico/releases/download/global/package_rp2040-ethernet_index.json

	(b)ツール→ボード→ボードマネージャより
	"Raspberry Pi Pico/RP2040 Ethernet"を検索しインストールします。

	(c)ツール→ボード→Raspberry Pi RP2040 Boards
	から使用する適切な機種名を選んでください(Wiznet W5500-EVB-Picoなど)
	
	(d)https://github.com/UECS/MyEthernet2
	"MyEthernet2"をインストールします。zipファイルでダウンロードしたあと、解凍してlibrariesフォルダ内に置いてください。
	
	以上の処理(一度行えば次から不要です)で最低限このファイルがコンパイル可能な環境が整います。

	以下の操作はボードを接続する事に毎回実行してください。
	(e)PCのUSBポートにボードを接続し、ツール→ボードより機種名が合っていることを確認します。

	(f)コンパイル前にツール→シリアルポートから接続中のボードのシリアルポート番号を選んでください。
	(ボードの個体ごとに違う値になる可能性があります)

	(g)IDE画面左上の→マークをクリックするとコンパイルとプログラムの転送が始まります。

	(h)転送が終わると自動的にボードがリセットされプログラムが実行されます。

(2)Arduino用のソースコードをUARDECS_PICOに移植する場合の注意点
	Arduino用のUARDECSおよびUARDECS_MEGAとソースコードレベルで互換性があります。
	ヘッダファイル(ライブラリ)の変更のみで大抵はコンパイルをそのまま通ります。
	ヘッダファイルには最低限"#include <Uardecs_pico.h>"の記述のみ必要ですが、
	Ethernet3.hは必須なので意図的に記述しています。

	ただし、ハードウェアに依存する部分は変更が必要です。
	本ライブラリが動作対象として想定しているPICO互換機のW5500-EVB-Picoでは以下のピンが最初から専有されています。
	(I/O)	PIN_ID	Note
	I	GPIO16	Connected to MISO on W5500
	O	GPIO17	Connected to CSn on W5500
	O	GPIO18	Connected to SCLK on W5500
	O	GPIO19	Connected to MOSI on W5500
	O	GPIO20	Connected to RSTn on W5500
	I	GPIO21	Connected to INTn on W5500
	I	GPIO24	VBUS sense - high if VBUS is present, else low
	O	GPIO25	Connected to user LED
	I	GPIO29	Used in ADC mode (ADC3) to measure VSYS/3

	Arduinoで動いていたデバイスも新たにRaspberry PI PICOで動かす場合は相性のテストが不可欠です。
	例えばI2Cを使用する場合、Arduinoと異なりbegin時にピン番号の指定が必要など、細部に差があります。
	Raspberry PI PICOは処理速度やメモリの搭載量でArduinoを超えますが電圧が3.3Vであること、
	ピンに流せる電流が4mA以下であることに注意してください。
	
(2)EEPROMについて
	本ライブラリはArduinoに搭載されていたEEPROMの代用としてRaspberry Pi Picoのプログラムフラッシュの未使用領域4096byteを保存領域に使用します。
	また、SRAMを4096byte分キャッシュ領域として使用する(起動後に確保されるので注意)ので、この分だけ利用可能なメモリが減少します。

	EEPROMの耐久性は書き換え回数のみはArduinoと同等（100,000回)ですが、4096byte単位でしか書き換えができないため、
	一部書き換えただけでも4096byteの領域全体の耐久性を損耗します。
	これらは書き換え頻度が少ないIPアドレスの保存やその他設定データの保存には深刻な影響はないかもしれません。
	しかし、データのロギングなど一定時間間隔で書き込むような用途には著しく不利になります。
	(おそらく)byte単位で書き換えが可能なArduinoより遥かに少ない回数で限界に達します。
	プログラム中でEEPROMを高頻度に書き換える場合、別途不揮発性のデータが保存可能なデバイスを接続する設計にすることを推奨します。
*/
