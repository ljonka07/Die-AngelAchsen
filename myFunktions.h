#ifndef _MYFUNKIONS_H_
#define _MYFUNKIONS_H_


/************************************************************************
Title:    Crazy Car Funktionen
Author:   Schuelergruppe GHO/RBZ/WHG
File:     myFunktions.h, V1.31 2015/11/23 1 $
Software: AVR-GCC 
Hardware: any AVR ,tested on ATmega32
Usage:    
************************************************************************/


// Funktionsprototypen
void fahren1(void);
void fahren2(void);
void fahren3(void);

uint16_t  linearisierungAD(uint16_t analogwert, uint8_t cosAlpha);
void akkuSpannungPruefen(int schwellwert);
void ledSchalterTest(void);
void pReglerServoRechts(void);


#endif

