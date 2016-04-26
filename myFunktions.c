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
int16_t ar = 0;
int16_t av = 0;
int16_t al = 0;
int16_t back = 0;
int16_t serv = 0;


// servo > 0 => rechts, servo < 0 => links

// <== Eigene Funktion und Bedingungen formulieren / schreiben
void fahren1(void){
	if (abstandvorne > 50) 
	{ 
		back = 0;
	}
	ar = abstandrechts;
	av = abstandvorne;
	al = abstandlinks;
	if (abstandvorne > 130) 
	{
		fahr(25);
		servo(pReglerServoRechts(abstandrechts, abstandlinks, abstandvorne));
	}
	else if ((abstandvorne < 130) && (back == 0) && (abstandvorne > 30)) 
	{ 
		fahr(20); 
		servo(pReglerServoRechts(abstandrechts));
	}
	else if ((abstandvorne < 30) || (back == 1))
	{ 
		back = 1;
		fahr(-20);
		if (ar > al)
		{
			servo(-10);
		}
		else
		{
			servo(10);
		}
	}
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


uint16_t pReglerServoRechts(uint16_t distanceR, uint16_t distanceL, uint16_t distanceV)
{
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
	if (distanceV > 100) {
		if (distanceL > 100)
		{
			serv = (distanceR*m1)/m2 + b;
		} 
		else if (distanceR > 100)
		{
			serv =  (distanceL*m1)/m2 + b;
		}
	} 
	else 
	{
		if (distanceL > distanceR) 
		{
			serv = -10;
		} 
		else 
		{
			serv = 10;
		}
	}
	return serv
}


void akkuSpannungPruefen(int schwellwert)
{
// Prüfe die AkkuSpannung nur, wenn Schalter 4 an ist und nur nach einem Reset!!
// Alle LEDs blinken, wenn Akku-Spannung < Schwellwert !!
	analogwertAkku=adc(7);
	if (analogwertAkku < schwellwert)
	{
	    ledPB1(1);
        ledPB2(1);
        ledPC2(1);
        ledPC3(1);
        warte_ms(blinkT/2);
        ledPB1(0);
        ledPB2(0);
        ledPC2(0);
        ledPC3(0);
        warte_ms(blinkT/2);
	}
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


