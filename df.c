/*Werner Jessen + Dieter Rathmann */
/*Version: CrazyCar-V1.31 */
/* 03.11.2015 */

#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>

#include "global.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"
#include "zyklus.h"
#include "df.h"
#include "cc-lib.h"
#include <stdint.h>
#include <avr/eeprom.h>
#include "EinAusgabeUSB.h"
 
uint16_t datenSatzZaehler=1;
uint16_t historyAverageL=0,historyAverageR=0,historyAverageV=0;
uint16_t historyAverageS=0,historyAverageFR=0;
union conv hAL, hAR, hAV, hAS, hAFR;

/*
uint16_t historyAverageI=0; // Sensor Strom
union conv hAI;
uint8_t  historyDepthI[HISTORYDEPTH][2]EEMEM;
uint16_t historyAverageU=0; // Sensor Spannung
union conv hAU;
uint8_t  historyDepthU[HISTORYDEPTH][2]EEMEM;
*/

int8_t  historyDepthPointer=0;
uint8_t historyDepthL[HISTORYDEPTH][2]EEMEM,historyDepthR[HISTORYDEPTH][2]EEMEM,historyDepthV[HISTORYDEPTH][2]EEMEM;
uint8_t historyDepthS[HISTORYDEPTH][2]EEMEM,historyDepthFR[HISTORYDEPTH][2]EEMEM;
uint8_t historyAzMAX[2]EEMEM;

//uint16_t historyDepthEx3[HISTORYDEPTH];


void initMikroController(void){
    //Ausgänge für Zyklustest
	DDRC |= (1<<PC1) ;
	
	// PWM Auagaenge fuer Motor und Lenkservo initialisieren
	DDRD |= (1<<PD4)|(1<<PD5)|(1<<PD6);
	
	pwm_init() ; 		//PWM Timer initialisieren
	adc_init() ; 		//Analogwandler initialisieren
	uart_init(9600); 	//serielle Schnittstelle initialisieren
	init_Timer2();   	//Timer für zyklischen Umlauf initialisieren
	sei() ;				//Interrupts freigeben
	
	OCR1B = STOP ;		//Geschwindigkeit B(400=schnell rückwärts/700=stop/1000=schnell vorwärts)
	OCR1A = GERADEAUS;	//A Lenkung?(750=geradeaus/950=voll rechts/550=voll links)

	uart_putc('\r');	
	uart_puts(welcome) ;	// Ausgabe bei Initialisierung des uControllers
	uart_putc('\r');
	
	// LEDs
	DDRB |= (1 << DDB1) | (1 << DDB2); //Ausgänge LEDs
	DDRC |= (1<< DDC2) | (1<<DDC3);		//Ausgänge LEDs
	DDRC &=  ~((1 << DDC4) | (1 << DDC5) | (1 << DDC6) | (1 << DDC7)); //Eingaenge Switch
	PORTC |= ( (1<<PC4) | (1<<PC5) | (1<<PC6)| (1<<PC7));  /* Interne Pull-Up fuer beide einschalten */


}

void warte_sekunden(int zeit){
	// Funktion wartete die angegebene Zeit in Sekunden.   		
	int k = 0;
	do{
		for(int i = 1; i <= 100; i++){// warte 100 mal 10 ms
			_delay_ms(10);   
		}	
	}while( ++k < zeit );
}

void warte_ms(int zeit){
	// Funktion wartete die angegebene Zeit in Millisekunden.   		
	int k = 0;
	do{
		_delay_ms(1); 	// warte mindestens 1 ms
	}while( ++k < zeit );
}	

void ledPB1(int8_t an) { 
	if ( an == 0 )	PORTB = PORTB & (~(1<<PB1)); //LED PB1 aus	
	else 				PORTB = PORTB | (1<<PB1);  	//LED PB1 an
}
void ledPB2(int8_t an) { 
	if ( an == 0 )	PORTB &=  (~(1<<PB2));	//LED PB2 aus, kurze Version
	else 				PORTB |= (1<<PB2);		//LED PB2 an, kurze Version
}
void ledPC2(int8_t an) { 
	if ( an == 0 )	PORTC &=  (~(1<<PC2));	//LED PC2 aus
	else 				PORTC |= (1<<PC2);		//LED PC2 an
}
void ledPC3(int8_t an) { 
	if ( an == 0 )	PORTC &=  (~(1<<PC3));	//LED PC3 aus
	else 				PORTC |= (1<<PC3);		//LED PC3 an
}	

int8_t s1(void) { 
	return ((PINC & (0x1 << PINC7)) ^ (0x1 << PINC7)) >> PINC7; } //Schalter 1,PC7,PIN 29
int8_t s2(void) { 
	return ((PINC & (0x1 << PINC6)) ^ (0x1 << PINC6)) >> PINC6; } //Schalter 2,PC6,PIN 28
int8_t s3(void) { 
	return ((PINC & (0x1 << PINC5)) ^ (0x1 << PINC5)) >> PINC5; } //Schalter 3,PC5,PIN 27
int8_t s4(void) { 
	return ((PINC & (0x1 << PINC4)) ^ (0x1 << PINC4)) >> PINC4; } //Schalter 4,PC4,PIN 26
	

void datenspeichern(void){  //Nur digitale Werte!

	//Aufsummieren der Analogwerte		
	historyAverageL = historyAverageL + analogwertL;
	historyAverageR = historyAverageR + analogwertR;	
	historyAverageV = historyAverageV + analogwertV;
	historyAverageS  = historyAverageS  + OCR1A;
	historyAverageFR = historyAverageFR + OCR1B;	
	
//	historyAverageI = historyAverageI + analogwertI; // Sensor Strom
//	historyAverageU = historyAverageU + analogwertU; // Sensor Spannung	
	
	if ( !(datenSatzZaehler % MWERTE)){	
		OutHL = historyAverageL / MWERTE; // Mittelwerte bilden und 
		OutHR = historyAverageR / MWERTE; // 10ms X 	MWERTE merken
		OutHV = historyAverageV / MWERTE;
		OutHS  = historyAverageS / MWERTE;
		OutHFR = historyAverageFR / MWERTE;
		
		hAL.Word = OutHL; //Werte in union speichern
		hAR.Word = OutHR; //Vorbereitung fuer EEMEM schreiben
		hAV.Word = OutHV;
		hAS.Word = OutHS;
		hAFR.Word = OutHFR;
			
		// MittelwertSpeicher zurücksetzen		
		historyAverageL = 0;
		historyAverageR = 0;		
		historyAverageV = 0;		
		historyAverageS  = 0;
		historyAverageFR = 0;
		
/*		OutHI = historyAverageI / MWERTE; // Sensor Strom
		hAI.Word = OutHI;
		historyAverageI  = 0;
		OutHU = historyAverageU / MWERTE; // Sensor Spannung
		hAU.Word = OutHU;	
		historyAverageU  = 0;
*/

		if (hspStart == 1) hspStart++;
	}
	
// Mittelwerte in EEMEM Byteweise speichern, 
	if ( (historyDepthPointer < HISTORYDEPTH) && (hspStart > 1) ){
		
		switch (datenSatzZaehler % MWERTE){
			case 0:	EEPWriteByte(&historyDepthL[historyDepthPointer][0],hAL.Byte[0] ); 
					//	historyDepthEx3[historyDepthPointer] = 0;
						break;
			case 1:	EEPWriteByte(&historyDepthL[historyDepthPointer][1],hAL.Byte[1] ); break;
			case 2:	EEPWriteByte(&historyDepthR[historyDepthPointer][0],hAR.Byte[0] ); 
			//			EEPWriteByte(&historyDepthI[historyDepthPointer][0],hAI.Byte[0] ); // Sensor Strom
						break;
			case 3:	EEPWriteByte(&historyDepthR[historyDepthPointer][1],hAR.Byte[1] ); break;
			case 4:	EEPWriteByte(&historyDepthV[historyDepthPointer][0],hAV.Byte[0] );
			//			EEPWriteByte(&historyDepthI[historyDepthPointer][1],hAI.Byte[1] ); // Sensor Strom
						break;
			case 5:	EEPWriteByte(&historyDepthV[historyDepthPointer][1],hAV.Byte[1] ); break;
			case 6: 	EEPWriteByte(&historyDepthS[historyDepthPointer][0],hAS.Byte[0] ); 
			//			EEPWriteByte(&historyDepthU[historyDepthPointer][0],hAU.Byte[0] ); // Sensor Spannung
						break;
			case 7: 	EEPWriteByte(&historyDepthS[historyDepthPointer][1],hAS.Byte[1] ); break;
			case 8: 	EEPWriteByte(&historyDepthFR[historyDepthPointer][0],hAFR.Byte[0] );
			// Test			EEPWriteByte(&historyDepthFR[historyDepthPointer][1],hAFR.Byte[1] );
			//			EEPWriteByte(&historyDepthU[historyDepthPointer][1],hAU.Byte[1] ); // Sensor Spannung
						break;
			case 9: 	EEPWriteByte(&historyDepthFR[historyDepthPointer][1],hAFR.Byte[1] ); 
					//	historyDepthEx3[historyDepthPointer] = hAabtastzeit;
						historyDepthPointer++; 
						break;	
			default: break;
		}	
		if (historyDepthPointer == HISTORYDEPTH) {	//längste Zykluszeit speichern
			convert.Word = abtastzeitMAX;
			EEPWriteByte(&historyAzMAX[0],convert.Byte[0] ); 
			EEPWriteByte(&historyAzMAX[1],convert.Byte[1] ); 
		}		
	}	
}	
void terminal_aktiv(void){
	eingabeUSB();
	if ( ((logOnOff == 1) || (history == 1)) ){
		ausgabeUSB2(); //Datenausgabe via USB	
	}
	
	if( !(datenSatzZaehler % 100) ) hAabtastzeit = 0;	//Auf 0 setzen für neuen Zyklus	
	if (abtastzeit > hAabtastzeit) hAabtastzeit = abtastzeit; //Max pro MWERTE Zyklen
	if (abtastzeit > abtastzeitMAX) abtastzeitMAX = abtastzeit; //seit Start
}

void ledPB1_flash(uint8_t flash_time){
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		ledPB1(1);	//LED PB1 an
		else ledPB1(0);	//LED PB1 aus
}
void ledPB2_flash(uint8_t flash_time){
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		ledPB2(1);	//LED PB2 an
		else ledPB2(0);	//LED PB2 aus
}		
void ledPC2_flash(uint8_t flash_time){
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		ledPC2(1);	//LED PB2 an
		else ledPC2(0);	//LED PB2 aus
}		
void ledPC3_flash(uint8_t flash_time,uint8_t invert ){ //10ms x flash_time
	if ((datenSatzZaehler % flash_time) > (flash_time/2) ) // Blickfrequenz
		if (invert == 0) 	ledPC3(1);	//LED PB2 an
		else 					ledPC3(0);	//LED PB2 aus	
	else 
		if (invert == 0) 	ledPC3(0);	//LED PB2 aus
		else 					ledPC3(1);	//LED PB2 an
}	