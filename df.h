#ifndef _DF_H_
#define _DF_H_
#define MWERTE	10				// 10 Werte = 100ms
#define HISTORYDEPTH 30	   //max 30, sonst Fehler

/************************************************************************
Title:    CrazyCar Funktionen
Author:   Schuelergruppe GHO1/RBZ1/WHG1
File:     df.h, V1.31 1 $
Software: AVR-GCC 
Hardware: any AVR ,tested on ATmega32
Usage:    
************************************************************************/
extern uint16_t datenSatzZaehler;
extern uint16_t historyAverageL,historyAverageR,historyAverageV;
extern uint16_t historyAverageS,historyAverageFR;
extern union conv  hAL, hAR, hAV, hAS, hAFR;

/*
extern uint16_t historyAverageI; // Sensor Strom
extern union conv hAI;
extern uint8_t  historyDepthI[HISTORYDEPTH][2]EEMEM;
extern uint16_t historyAverageU; // Sensor Spannung
extern union conv hAU;
extern uint8_t  historyDepthU[HISTORYDEPTH][2]EEMEM;
*/

extern int8_t historyDepthPointer;

extern uint8_t historyDepthL[HISTORYDEPTH][2] EEMEM,historyDepthR[HISTORYDEPTH][2] EEMEM,historyDepthV[HISTORYDEPTH][2] EEMEM;
extern uint8_t historyDepthS[HISTORYDEPTH][2] EEMEM,historyDepthFR[HISTORYDEPTH][2] EEMEM;
extern uint8_t historyAzMAX[2]EEMEM;
//extern uint16_t historyDepthEx3[HISTORYDEPTH];


// Funktionsprototypen
void initMikroController(void);
void warte_ms(int zeit);
void warte_sekunden(int zeit);

void ledPB1(int8_t an);
void ledPB2(int8_t an);
void ledPC2(int8_t an);
void ledPC3(int8_t an);
int8_t s1(void);
int8_t s2(void);
int8_t s3(void);
int8_t s4(void);
void datenspeichern(void);
void terminal_aktiv(void);
void ledPB1_flash(uint8_t flash_time);
void ledPB2_flash(uint8_t flash_time);
void ledPC2_flash(uint8_t flash_time);
void ledPC3_flash(uint8_t flash_time, uint8_t invert);
#endif

