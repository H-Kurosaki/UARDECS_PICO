//EEPROM Emulator
#ifndef EEPROM_h
#define EEPROM_h

struct EEPROMClass{
	uint8_t dummyrom[4096];
	bool startflag=false;
    uint8_t read( int idx ) 
			{
    			if(!startflag)
				{
					startflag=true;
					for(int i=0;i<4096;i++)
					{dummyrom[i]=255;}
				}
    	
    		return dummyrom[idx]; 
			}
    void write( int idx, uint8_t val )   { dummyrom[idx]=val; }
//    void update( int idx, uint8_t val )  { EERef( idx ).update( val ); }
    void begin(int dummy){}
    bool commit(){return true;}
    
};

static EEPROMClass EEPROM;
#endif
