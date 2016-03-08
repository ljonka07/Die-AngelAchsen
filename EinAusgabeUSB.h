#ifndef _EinAusgabeUSB_H_
#define _EinAusgabeUSB_H_


/************************************************************************
Title:    Crazy Car Funktionen
Author:   Dieter Rathmann
File:     myFunktions.h, V1.31 1 $
Software: AVR-GCC 
Hardware: any AVR ,tested on ATmega32
Usage:    
************************************************************************/

#define	AUSGABEFAKTOR	2				// wenn 1 u. MwWerte=10 dann alle 500ms

// Funktionsprototypen
void ausgabeUSB2(void);
void ausgabehistory(void);
void eingabeUSB(void);

int pruefeWerteBereich(int unten, int oben, int wert);
int8_t getServo(int16_t wertOCR1A);
int8_t getFahr(int16_t wertOCR1B);


#endif

