#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"
#include "adc.h"
#include "global.h"
#include "zyklus.h"
#include "myFunktions.h"
#include "EinAusgabeUSB.h"
#include "df.h"
#include "cc-lib.h"



int16_t histerese=0;


// servo > 0 => rechts, servo < 0 => links

// <== Eigene Funktion und Bedingungen formulieren / schreiben
void fahren1(void){
	if (abstandvorne > 130)
	{
		fahr(25);
	}
	else
	{
		fahr(20);
	}
	servo(pReglerServoRechts(abstandrechts));
}

void fahren2(void){

}
void fahren3(void){

}


uint16_t linearisierungAD_links(uint16_t analogwert, uint8_t cosAlpha){
	if (analogwert < 100) {analogwert = 100;};
	int16_t cm = ((A / analogwert) + B)*cosAlpha;
	cm /= 100;
	return cm;
}

int16_t linearisierungAD_rechts(uint16_t analogwert, uint8_t cosAlpha){
	if (analogwert < 100) {analogwert = 100;};
	int16_t cm = ((A2 / analogwert) + B2)*cosAlpha;
	cm /= 100;
	return cm;
}

uint16_t linearisierungAD_vorne(uint16_t analogwert, uint8_t cosAlpha){
	if (analogwert < 100) {analogwert = 100;};
	int16_t cm = ((A3 / analogwert) + B3)*cosAlpha;
	cm /= 100;
	return cm;
}

uint16_t linearisierungAD(uint16_t analogwert, uint8_t cosAlpha){
	if (analogwert < 100) {analogwert = 100;};
	int16_t cm = ((A3 / analogwert) + B3)*cosAlpha;
	cm /= 100;
	return cm;
}

uint16_t pReglerServoRechts(uint16_t distance){
	//ausrichten an der rechten Wand mit P-Regler
	//Funktion y(e) = me + b z

	//==>  Variablen sind in der global.h u. global.c definiert!  <====
	//int16_t m1=67;			//float Operation vermeiden
	//int16_t m2=100;			//Divisor Steigung
	//mit m=m1/m2=0,67, bei +- 15cm vom Sollwert
	
	//int16_t b=0;				//Durchbruch durch die Y-Achse
	//int16_t e;				//e=Regelabweichung in cm
	//int16_t y;				//y=Stellgroeße / Winkel
	//int16_t sollwert = 35;//Sollwert 45 cm  
		
	//bestimmen der Regelabweichung
	//z.B. Sollwert gerade (35cm),	20cm volllinks, 50cm vollrechts
	
	return (m1 * distance)/m2 - b;

}


void akkuSpannungPruefen(int schwellwert){
//Prüfe die AkkuSpannung nur, wenn Schalter 4 an ist und nur nach einem Rest!!
// Alle LEDs blinken, wenn Akku-Spannung < schwellwert !!

	analogwertAkku=adc(7);
}


void ledSchalterTest(void){
	if (s1() == 1)
	{
		for (int i=1;i <= 10; i++) {
			ledPB1(1);
			ledPB2(1);
			ledPC2(1);
			ledPC3(1);
			warte_ms(100);
			ledPB1(0);
			ledPB2(0);
			ledPC2(0);
			ledPC3(0);
			warte_ms(100);  
		};
	}
	else
	{
		ledPB1(0);
		ledPB2(0);
		ledPC2(0);
		ledPC3(0);
	};
}


