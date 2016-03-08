#ifndef _GLOBAL_H_
#define _GLOBAL_H_
#include <inttypes.h>
#include <avr/eeprom.h>

#define ABTASTZEIT 100 // in 100us, entspricht 10ms
#define MAXEINGABE 25

#define A 22894
#define B 29
#define A2 24214
#define B2 -9
#define A3 23430
#define B3 6

#define  MAXVOLLLINKS		1100
#define  MAXVOLLRECHTS		500

#define  GERADEAUS		760
//#define  VOLLLINKS		940
//#define  VOLLRECHTS		560

#define  MAXRUEWAERTS	400
#define  STOP			700
#define  MAXVORWAERTS	1000

#define MAXTEXTH	25

#define   EEPReadByte(addr)         eeprom_read_byte((uint8_t *)addr)     
#define   EEPWriteByte(addr, val)   eeprom_write_byte((uint8_t *)addr, val)
#define   EEPUpdateByte(addr, val)  eeprom_update_byte((uint8_t *)addr, val)
#define   EEPWriteBlock(__src,__dst, anz) eeprom_write_block ((const void *)__src, (void *)__dst, anz)
#define   EEPReadBlock(__dst,__src, anz) eeprom_read_block ((void *)__dst, (const void *)__src, anz)
#define   EEPUpdateBlock(__src,__dst, anz)	eeprom_update_block ((const void *)__src, (void *)__dst, anz)
#define   EEPReadWord(addr)          eeprom_read_word((const uint16_t *)addr)     
#define   EEPWriteWord(addr, val)        eeprom_write_word((uint16_t *)addr,(uint16_t) val ) 
#define   EEPUpdateWord(addr, val)       eeprom_update_word((uint16_t *)addr,(uint16_t) val ) 


 
//Variablen zur Übergabe für Interrupts
extern unsigned int abtastzeit,abtastzeitMAX,hAabtastzeit;

//  Globale Variable
extern uint16_t abstandlinks,abstandrechts,abstandvorne,analogwertAkku;
extern uint16_t analogwertL,analogwertR,analogwertV;
//extern uint16_t analogwertEx;

extern uint16_t  OutL,OutR,OutV,OutS,OutFR;;
extern uint16_t  OutHL,OutHR,OutHV,OutHS,OutHFR;
/*
extern uint16_t  abstandI, analogwertI; // Sensor Strom
extern uint16_t  OutI, OutHI; 
extern uint16_t  abstandU, analogwertU; // Sensor Spannung
extern uint16_t  OutU, OutHU; 
*/
extern char eingabe[MAXEINGABE];
extern uint8_t j,logOnOff,logCmOnOff,history;
extern uint8_t	 logStart;
extern uint8_t	 hspStart;

extern int8_t  anzahl, befehl, mod;
extern 	int	kommand;		// 1 wenn neues Kommando eingegeben wurde
extern const	char kommandos[7][4];


extern const char welcome[];
extern const char freeRAMText[];
extern const char abtastzeitMAXText[];
extern const char kommandoText[];
extern const char info1kommandos[];
extern const char modTEXT[];

extern 	char texth[MAXTEXTH],tab[2] ;
extern 	char text3[];
extern 	char text4[];
extern 	char textF[];

extern 	uint16_t wert;
extern 	char zeichen;
extern	int16_t servoML;
extern	int16_t servoG;
extern	int16_t servoMR;
extern	int8_t maxFahr;	    //in %

//P-Regler, m = m1/m2
extern 	int16_t m1;		    //float Operation vermeiden!
extern 	int16_t m2;		    //Divisor Steigung
extern 	int16_t b;		    //Durchbruch durch die Y-Achse, wird nicht bwnötigt
extern 	int16_t e;		    //e=Regelabweichung in cm
extern 	int16_t y;			//y=Stellgröße / Winkel
extern 	int16_t sollwert;   //Sollwert 45 cm  

extern union conv
{
  int8_t Byte[2];  //2 Byte
   uint16_t Word;  //1 Word
}convert;

#endif