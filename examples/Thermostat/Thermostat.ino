/////////////////////////////////////////
//UARDECS Sample Program "Thermostat"
//UECS ccm "InAirTemp" recieving test Ver1.4
//By H.kurosaki 2024/4/22
//////////////////////////////////////////
//[概要]
//InAirTempを受信し、設定された温度より低い場合cndに1を、それ以外で0を出力します。
//温度の閾値はWeb上から入力できます。出力を強制的にON/OFFすることもできます。
//
//[注意]
//購入直後のマイコンボードではWeb上に不定な文字列が表示されることがあります。
//ノード名や設定値を入力することで正常に動作します。

//コンパイル環境の構築や他機種からソースコード移植を行う場合の注意点を一番後ろに記載しています


#include <Uardecs_pico.h>

/*
#define NONE -1
#define A_1S_0 0
#define A_1S_1 1
#define A_10S_0 2
#define A_10S_1 3
#define A_1M_0 4
#define A_1M_1 5
#define S_1S_0 6
#define S_1M_0 7
#define B_0 8
#define B_1 9

#define UECSSHOWDATA 0
#define UECSINPUTDATA 1
#define UECSSELECTDATA 2
#define UECSSHOWSTRING 3
*/

/////////////////////////////////////
//IP reset jupmer pin setting
//IPアドレスリセット用ジャンパーピン設定
/////////////////////////////////////
//Pin ID. This pin is pull-upped automatically.
//ピンIDを入力、このピンは自動的にプルアップされます
//ピンIDは変更可能です
const byte U_InitPin = 3;
const byte U_InitPin_Sense=HIGH;//ソフトウェア上でSafeModeを抜けたい場合はLOWにしてください

//When U_InitPin status equals this value,IP address is set "192.168.1.7".
//U_InitPinに指定したピンがこの値になった時、IPアドレスが"192.168.1.7"に初期化されます。
//購入直後のArduinoは初期化が必要です。

////////////////////////////////////
//Node basic infomation
//ノードの基本情報
///////////////////////////////////
const char U_name[] PROGMEM= "UARDECS Node v.1.1";//MAX 20 chars
const char U_vender[] PROGMEM= "XXXXXXXX Co.";//MAX 20 chars
const char U_uecsid[] PROGMEM= "000000000000";//12 chars fixed
const char U_footnote[] PROGMEM= "UARDECS Sample Program Thermostat";
char U_nodename[20] = "Sample";//MAX 19chars (This value enabled in safemode)
UECSOriginalAttribute U_orgAttribute;//この定義は弄らないで下さい
//////////////////////////////////
// html page1 setting
//Web上の設定画面に関する宣言
//////////////////////////////////
#define DECIMAL_DIGIT	1 //小数桁数

//Total number of HTML table rows.
//web設定画面で表示すべき項目の総数
const int U_HtmlLine = 4;

//●表示素材の定義(1)数値表示
//UECSSHOWDATA
const char NAME0[] PROGMEM= "Temperature";
const char UNIT0[] PROGMEM= "C";
const char NOTE0[] PROGMEM= "SHOWDATA";

//表示用の値を格納する変数
//小数桁数が1の場合、123が12.3と表示される
signed long showValueTemp;

//●表示素材の定義(2)選択肢表示
//UECSSELECTDATA
const char NAME1[] PROGMEM= "UserSwitch";
const char NOTE1[] PROGMEM= "SELECTDATA";
const char UECSOFF[] PROGMEM= "OFF";
const char UECSON[] PROGMEM= "ON";
const char UECSAUTO[] PROGMEM= "AUTO";
const char *stringSELECT[3]={
UECSOFF,
UECSON,
UECSAUTO,
};

//入力された選択肢の位置を受け取る変数
//UECSOFFが0、UECSONで1、UECSAUTOで2になる
signed long setONOFFAUTO;

//●表示素材の定義(3)数値入力
//UECSINPUTDATA
const char NAME2[] PROGMEM= "SetTemp";
const char UNIT2[] PROGMEM= "C";
const char NOTE2[] PROGMEM= "INPUTDATA";

//入力された数値を受け取る変数
//小数桁数が1の場合、例えばWeb上で12.3が入力されると123が代入される
signed long setONTempFromWeb;

//●表示素材の定義(4)文字表示
//UECSSHOWSTRING
const char NAME3[] PROGMEM= "Now status";
const char NOTE3[] PROGMEM= "SHOWSTRING";
const char SHOWSTRING_OFF[] PROGMEM= "OUTPUT:OFF";
const char SHOWSTRING_ON [] PROGMEM= "OUTPUT:ON";
const char *stringSHOW[2]={
SHOWSTRING_OFF,
SHOWSTRING_ON,
};
signed long showValueStatus;

//●ダミー素材の定義
//dummy value
const char NONES[] PROGMEM= "";
const char** DUMMY = NULL;

//表示素材の登録
struct UECSUserHtml U_html[U_HtmlLine]={
//{名前,入出力形式	,単位	,詳細説明,選択肢文字列	,選択肢数,値			,最小値,最大値,小数桁数}
{NAME0,	UECSSHOWDATA	,UNIT0	,NOTE0	, DUMMY		, 0	, &(showValueTemp)	, 0, 0, DECIMAL_DIGIT},
{NAME1,	UECSSELECTDATA	,NONES	,NOTE1	, stringSELECT	, 3	, &(setONOFFAUTO)	, 0, 0, 0},
{NAME2, UECSINPUTDATA	,UNIT2	,NOTE2	, DUMMY		, 0	, &(setONTempFromWeb)	, 100, 1000, DECIMAL_DIGIT},
{NAME3, UECSSHOWSTRING	,NONES	,NOTE3	, stringSHOW	, 2	, &(showValueStatus)	, 0, 0, 0},
};

//////////////////////////////////
// UserCCM setting
// CCM用の素材
//////////////////////////////////

//define CCMID for identify
//CCMID_dummy must put on last
//可読性を高めるためCCMIDという記号定数を定義しています
enum {
CCMID_InAirTemp,
CCMID_cnd,
CCMID_dummy, //CCMID_dummyは必ず最後に置くこと
};
//This sentence is same as follows:
//上記マクロは以下と同じ意味です
//#define CCMID_InAirTemp 0
//#define CCMID_cnd 1
//#define CCMID_dummy 2


//CCM格納変数の宣言
//ここはこのままにして下さい
const int U_MAX_CCM = CCMID_dummy;
UECSCCM U_ccmList[U_MAX_CCM];

//CCM定義用の素材
const char ccmNameTemp[] PROGMEM= "Temperature";
const char ccmTypeTemp[] PROGMEM= "InAirTemp";
const char ccmUnitTemp[] PROGMEM= "C";

const char ccmNameCnd[] PROGMEM= "NodeCondition";
const char ccmTypeCnd[] PROGMEM= "cnd.aXX";
const char ccmUnitCnd[] PROGMEM= "";

//------------------------------------------------------
//UARDECS初期化用関数
//主にCCMの作成とMACアドレスの設定を行う
//------------------------------------------------------
void UserInit(){
//MAC address is printed on sticker of Ethernet Shield.
//You must assign unique MAC address to each nodes.
//MACアドレス設定、必ずEthernet Shieldに書かれた値を入力して下さい。
//全てのノードに異なるMACアドレスを設定する必要があります。
U_orgAttribute.mac[0] = 0x00;
U_orgAttribute.mac[1] = 0x00;
U_orgAttribute.mac[2] = 0x00;
U_orgAttribute.mac[3] = 0x00;
U_orgAttribute.mac[4] = 0x00;
U_orgAttribute.mac[5] = 0x00;

//Set ccm list
//CCMの作成
//UECSsetCCM(送/受の別, CCMID(固有の整数), CCM名(表示用), type, 単位,優先度(通常29), 小数桁数, 送受信頻度);
//true:送信 false:受信になります
UECSsetCCM(false, CCMID_InAirTemp, ccmNameTemp, ccmTypeTemp, ccmUnitTemp, 29, DECIMAL_DIGIT, A_10S_0);
UECSsetCCM(true,  CCMID_cnd      , ccmNameCnd , ccmTypeCnd , ccmUnitCnd , 29,		0, A_1S_0);

//以下のように記述することもできます
//This sentence is same as follows:
/*
UECSsetCCM(false, CCMID_InAirTemp,PSTR("Temperature"), PSTR("InAirTemp"), PSTR("C"), 29, DECIMAL_DIGIT, A_10S_0);
UECSsetCCM(true,  CCMID_cnd      ,PSTR("NodeCondition") ,  PSTR("cnd.aXX") , PSTR("") , 29,    0, A_1S_0);
*/
}

//---------------------------------------------------------
//Webページから入力が行われ各種値を取得後以下の関数が呼び出される。
//この関数呼び出し後にEEPROMへの値の保存とWebページの再描画が行われる
//---------------------------------------------------------
void OnWebFormRecieved(){
ChangeThermostat();
}


//---------------------------------------------------------
//毎秒１回呼び出される関数
//関数の終了後に自動的にCCMが送信される
//---------------------------------------------------------
void UserEverySecond(){
ChangeThermostat();
}

//---------------------------------------------------------
//１分に１回呼び出される関数
//---------------------------------------------------------
void UserEveryMinute(){
}

//---------------------------------------------------------
//メインループ
//システムのタイマカウント，httpサーバーの処理，
//UDP16520番ポートと16529番ポートの通信文をチェックした後，呼び出さされる関数。
//呼び出される頻度が高いため，重い処理を記述しないこと。
//---------------------------------------------------------
void UserEveryLoop(){
}

//---------------------------------------------------------
//setup()実行後に呼び出されるメインループ
//この関数内ではUECSloop()関数を呼び出さなくてはならない。
//UserEveryLoop()に似ているがネットワーク関係の処理を行う前に呼び出される。
//必要に応じて処理を記述してもかまわない。
//呼び出される頻度が高いため,重い処理を記述しないこと。
//---------------------------------------------------------
void loop(){
UECSloop();
}

//---------------------------------------------------------
//起動直後に１回呼び出される関数。
//様々な初期化処理を記述できる。
//この関数内ではUECSsetup()関数を呼び出さなくてはならない。
//必要に応じて処理を記述してもかまわない。
//---------------------------------------------------------
void setup(){
UECSsetup();
}

//---------------------------------------------------------
//サーモスタット動作を変化させる関数
//---------------------------------------------------------
void ChangeThermostat(){
    showValueTemp = U_ccmList[CCMID_InAirTemp].value;

//サーモスタット動作 
if(setONOFFAUTO==0)
    {U_ccmList[CCMID_cnd].value=0;}//Manual OFF
else if(setONOFFAUTO==1)
    {U_ccmList[CCMID_cnd].value=1;}//Manual ON
else if(setONOFFAUTO==2 && U_ccmList[CCMID_InAirTemp].validity && U_ccmList[CCMID_InAirTemp].value<setONTempFromWeb)
    {U_ccmList[CCMID_cnd].value=1;}//Auto ON
else
    {U_ccmList[CCMID_cnd].value=0;}//OFF

    showValueStatus = U_ccmList[CCMID_cnd].value;
}


/*
CCMへのアクセス方法
各CCMの状態はU_ccmList[CCMID]という構造体に格納されています
受信に設定したCCMの値は受信すると自動更新されます
送信に設定したCCMの値は自動送信されます

boolean U_ccmList[CCMID].sender;	//送受信の別
const char * U_ccmList[CCMID].name;	//CCM名(表示用)
const char * U_ccmList[CCMID].type;	//type(送受信用)
const char * U_ccmList[CCMID].unit;	//単位
unsigned char U_ccmList[CCMID].decimal;	//小数桁数
signed long U_ccmList[CCMID].value;	//CCMの値
signed char U_ccmList[CCMID].ccmLevel;	//送受信レベル(A_1S_0など、UECS仕様書参照)
signed short U_ccmList[CCMID].attribute[4];//受信時、受信したCCMのroom,region,order,priorityの順に格納
signed short U_ccmList[CCMID].baseAttribute[3];//ノードに設定したroom,region,order
boolean U_ccmList[CCMID].validity;	//受信時、CCMの値が有効かどうか(受信不能、タイムアウトするとfalse)
IPAddress U_ccmList[CCMID].address;	//受信時、相手のIPアドレス
boolean U_ccmList[CCMID].flagStimeRfirst;//送信:trueにするとCCMが送信される 受信:初めて受信するとtrueにセットされる

valueの小数桁数の設定について
送信側:
例えば小数桁数1の場合、valueに123を代入すると12.3として送信されます。
floatなどから変換する場合、10倍して整数にする処理が必要です。

受信側:
受信したCCMの小数桁数にかかわらず、valueはプログラム内で設定した小数桁数に丸められます。
例えば、小数桁数1の場合以下のように値が変換されてvalueに代入されます
123→1230
123.4→1234
123.45→1234
ただし、条件によっては桁数の変換時に誤差が発生することがあります。
*/


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
