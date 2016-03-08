#include "global.h"
#include <stdint.h>
#include <avr/eeprom.h>



//Variablen zur Übergabe für Interrupts

unsigned int abtastzeit=0,abtastzeitMAX=0,hAabtastzeit=0; 

//  Globale Variable
uint16_t abstandlinks,abstandrechts,abstandvorne;
uint16_t analogwertL,analogwertR,analogwertV,analogwertAkku;

uint16_t  OutL=0,OutR=0,OutV=0, OutS=0,OutFR=0;
uint16_t  OutHL=0,OutHR=0,OutHV=0, OutHS=0,OutHFR=0;
/*
uint16_t  abstandI, analogwertI; // Sensor Strom
uint16_t  OutI=0, OutHI=0; 
uint16_t  abstandU, analogwertU; // Sensor Spannung
uint16_t  OutU=0, OutHU=0; 
*/

char eingabe[MAXEINGABE];		//15 siehe global.h
uint8_t j=0,logOnOff = 0,logCmOnOff = 0, history = 0;
uint8_t	 logStart=0;
uint8_t	 hspStart=0;

//Gobale Variable für Funktion eingabeUSB()
int8_t  anzahl = 7, befehl, mod=0;
int	kommand = 0;		// 1 wenn neues Kommando eingegeben wurde

const char welcome[]="CC V1.31 ";
const char freeRAMText[]="freeRAM=";
const char abtastzeitMAXText[]="AzMax=";
const char kommandoText[]="Kommando";
const char info1kommandos[]="s:\ts,f,log,cm,ha,hs,mm\r";
const char kommandos[7][4]={"s","f","log","cm","ha","hs","mm"};
const char modTEXT[]="Mod=";

char texth[MAXTEXTH], tab[2] ="\t\0" ;
char text3[] = "\tan" ; // log cm history
char text4[] = "\taus" ;	// log cm history

char textF[] = "ungültig" ;


uint16_t wert;
char zeichen;
int16_t fahr;
int16_t servoML;
int16_t servoG;
int16_t servoMR;
int8_t maxFahr;	//in %

//P-Regler, m = m1/m2
int16_t m1=67;			 //float Operation vermeiden!
int16_t m2=100;		 //Divisor Steigung
//int16_t b=0;			 //Durchbruch durch die Y-Achse, wird nicht benötigt
int16_t e;				 //e=Regelabweichung in cm
int16_t y;				 //y=Stellgröße / Winkel
int16_t sollwert = 35;//Sollwert 45 cm  

union conv convert;
 