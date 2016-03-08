#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

#include "uart.h"
#include "global.h"
#include "zyklus.h"
#include "EinAusgabeUSB.h"
#include "myFunktions.h"
#include "df.h"
#include "adc.h"
#include "cc-lib.h"


#define  MAXLINKS		500
#define  MAXRECHTS	1100

void eingabeUSB(void){
//Diese Funktion liest Eingaben von der USB-Terminal-Schnittstelle ein und 
//wertet sie aus. Eine Eingabe muss mit einem Leerzeichen (Space) abschließen!
//Die eingegebenen Kommandows werden ausgewertet, z.B. "log " aktiviert 
//die Kontrollausgabe (logging) auf der USB-Terminal-Schnittstelle.

		//lese von USB solang Zeichen ein bis ein Leerzeichen kommt
		wert   = uart_getc();	//
		zeichen= (char)wert;

		if (wert != USART_NO_DATA){	//ein neues Zeichen?
		//	uart_putc(zeichen) ;			//Testausgabe
			eingabe[j] = zeichen ;
			j++;		
		}
		if (j >= MAXEINGABE) j=0;	//neue Zeichenkette einlesen		
			
		if (zeichen == ' ') {
			if (j != 1){
				eingabe[j-1] = '\0';		//Endekennung
				uart_putc('\r');
				uart_puts(kommandoText) ;
				uart_putc(' ');uart_putc(' ');
				uart_puts(eingabe);		
				//uart_putc('\r');	
				kommand = 1;	//neues Kommando 
			}
			j=0;	//neue Zeichenkette einlesen			
		}
			
		//Überprüfe das neue Kommando und führe es ggf. aus
		if (kommand == 1){ 
			//Prüfe Kommandows
			int i,zahl,n=0;
			befehl = -1;
			for (i=0; (i < anzahl) && (befehl == -1); i++){		
				for (n=0; (kommandos[i][n] == eingabe[n]) && (kommandos[i][n] != '\0'); n++);
				if (kommandos[i][n] == '\0') befehl = i;		
			}
			  //itoa(befehl,texth,10);  //Kontrollausgabe
			  //strcat(texth2,texth);
			  //uart_puts(texth2); uart_putc('\r');
				
			switch (befehl) {
				case 0:{	//Servo
					zahl=atoi(&eingabe[n]);
					servo(zahl);						//Ansteuerung des Servos
					uart_putc('\r');	
					break;  			
				}
				case 1: {	//Fahrtenregler
					zahl=atoi(&eingabe[n]); //SRAM sparen	
					fahr(zahl);
					uart_putc('\r'); 
					break;  			
				}
					
				case 2: // Loggen auf USB-Terminal on / off
					if (logOnOff == 1) {
						logOnOff = 0;
						uart_puts(text4); 
						uart_puts(tab); 
						uart_puts(freeRAMText); 
						itoa(freeRam(),texth,10);  			// Verbrach von SRAM prüfen
						uart_puts(texth);
						uart_puts(tab); 
						uart_puts(abtastzeitMAXText); 						
						itoa(abtastzeitMAX,texth,10);  
						uart_puts(texth);						
						uart_putc('\r');
						uart_puts(kommandoText);
						uart_puts(info1kommandos); uart_putc('\r');
					}
					else {
						logOnOff = 1;		//aktiviere Ausgabe
						logStart=0;			//sync auf ersten Datenblock
						uart_puts(text3); 
						uart_puts(tab); 
						uart_puts(abtastzeitMAXText); 						
						itoa(abtastzeitMAX,texth,10);  
						uart_puts(texth);																		
						uart_putc('\r');
					}
					break; 					
				case 3: // Logdaten auf USB-Terminal in cm on / off
					if (logCmOnOff == 1) {
						logCmOnOff = 0;
						uart_puts(text4); uart_putc('\r');
					}
					else {
						logCmOnOff = 1;
						uart_puts(text3); uart_putc('\r');
					}
					break; 
					
				case 4: 
					if (history == 1) {
						history = 0;
						uart_puts(text4); uart_putc('\r');
					}
					else {
						history = 1;
						uart_puts(text3); uart_putc('\r');
					}
					break;    
				case 5: 
					j=0;	//neue Zeichenkette einlesen
					uart_puts(text3); uart_putc('\r');	
					abtastzeitMAX=0;		//Zyklusmessung zurücksetzen
					historyDepthPointer=0; 		//neue Datenaufzeichung
					hspStart=1;			// beginne Speicherung bein naechsten Block
					break;    
				case 6: {	//Mod wechseln
					if((eingabe[n] > 47) && (eingabe[n] < 58)){ //dann ist es eine Ziffer
						mod = (int8_t) eingabe[n] - 48;
						ledPB2(0);	//LED PB2 aus
						strcpy(texth,tab); uart_puts(texth);
						uart_puts(modTEXT); uart_putc( (char) mod+48);
						uart_putc('\r'); 
					}
					else{
						uart_puts(kommandoText);
						uart_puts(textF); uart_putc('\r');
					}
					break;  			
				}	
					
				default: { //uart_puts(kommandoText);
					strcpy(texth,tab); uart_puts(texth);
					uart_puts(textF); uart_putc('\r'); break;}     // für den Fall einer falschen Zeile
			}
			kommand = 0;	// Kommando abgearbeitet
		}	
}

int pruefeWerteBereich(int unten, int oben, int wert){
//Rückgabe = 1 , wenn wert gößerbzw gleich unten und zuätzlich 
//kleine bzw. gleich oben ist
// 	
	if (( wert >= unten) && (wert <= oben)) return 1;
	else return 0;
}

void ausgabeUSB2(void){
	//Die Funktion gibt folgende Datensatz je 0,5Sekunden über die USB-Terminal-Schnittstelle aus:
	//Analogwerte der Sensoren rechts. links und vorne
	//die Werte für den Fahrtenregler und den Servo
	//ggf. den AKKU-Analogwerte
	//es können zusätzliche Werte ausgegeben erden
		
	// Es fehlt die Mittelwertbildung,		//??	
	// die Ausgabe der Senorwerte in cm
	// das festlegen von Schwellwerten der Akku-Spannung

	
	if (history == 1){
		ausgabehistory();
		history = 0;	//Ausgabe der Aufzeichung
		logOnOff = 0;	//Keine weiter Ausgaben
	}
	else {	

		// Ausgabe der Überschrift, in verschiedenen Zyklen	
		switch(datenSatzZaehler % (1000)){
			case 7: uart_puts("\rt in s\tlinks\tvorne\t");	break;
			case 9: uart_puts("rechts\t");	break;
			case 17: uart_puts("Servo\tFahr\t"); break;
			case 27: uart_puts("Akku\t"); break;
//			case 37: uart_puts("Strom\t");break;
//			case 47: uart_puts("Spannung\t");	break;
			case 57: uart_puts("Mod");	break;
			
			case 97: uart_putc('\r');	break;
			default:	break;
		}
//*		Berechnungen in verschiedenen Zyklen
		switch(datenSatzZaehler % MWERTE){
			case 0: ; break;
			case 1: OutL += OutHL;break;
			case 2: OutR += OutHR;break;
			case 3: OutV += OutHV;break;
			case 4: OutS += OutHS;break;
			case 5: OutFR += OutHFR;break;
//			case 6: OutI += OutHI;break; // Sensor Strom
//			case 7: OutU += OutHU;break; // Sensor Spannung
			case 8: break;
			default: break;
		}

//		*/
		if ( (!(datenSatzZaehler % (50*AUSGABEFAKTOR))) &&
					(logStart > 0) )  { //50 = 0,5Sekunden
			OutL /= 5;	// Mittelwertbildung
			OutR /= 5;		
			OutV /=  5;
			OutS  /=  5;
			OutFR /=  5;
//			OutI  /=  5; // Sensor Strom
//			OutU  /=  5; // Sensor Spannung	
				
			if(logCmOnOff == 1) {
				//Linearisierung der Analogwerte mittels einer Hyperbel 2. Ordnung
				//x0,7 Sensor schaut im Winkel von 45° auf die Wand
				OutL = linearisierungAD (OutL,70);
				OutR = linearisierungAD (OutR,70);
				OutV = linearisierungAD (OutV,100);
//					historyAverage1OutEx = linearisierungAD (historyAverage1OutEx,10);	
			}
	
			itoa(datenSatzZaehler/100,texth,10);	// wandle die Ausgabezeit (interger) in acsii um
			strcat(texth,tab);						// Text anhaengen
			uart_puts(texth);
			
			itoa(OutL,texth,10);  // Sensor Links
			strcat(texth,tab);	
			uart_puts(texth);
			
			itoa(OutV,texth,10);  // Sensor Vorne		
			strcat(texth,tab);	
			uart_puts(texth);
			
			itoa(OutR,texth,10);  // Sensor Rechts		 
			strcat(texth,tab);	
			uart_puts(texth);
			
			itoa(getServo((int16_t)OutS),texth,10);  			// Servo / Lenkung			 
			strcat(texth,tab);	
			uart_puts(texth);
			
			itoa(getFahr((int16_t)OutFR),texth,10);  			// Antrieb / Fahrtenregler			 
			strcat(texth,tab);
			uart_puts(texth);

			itoa(analogwertAkku,texth,10);  // Akku, analog			 
			strcat(texth,tab);
			uart_puts(texth);	
/*			
			itoa(OutI,texth,10);  // Sensor Strom		 
			strcat(texth,tab);
			uart_puts(texth);	
			itoa(OutU,texth,10);  // Sensor Spannung		 
			strcat(texth,tab);
			uart_puts(texth);	
*/	
//*
			itoa(mod,texth,10);   
			strcat(texth,tab);
			uart_puts(texth);	
//*/
							
			if ( (datenSatzZaehler % (50*AUSGABEFAKTOR)) == 0 ){
				uart_putc('\r');			
			}		
		}
		if ( !(datenSatzZaehler % 50) ) {
			//Mittelwerte für neue Berechung auf 0 zurücksetzen 
			OutL = 0;
			OutR = 0;		
			OutV = 0;
			OutS = 0;
			OutFR = 0;
//			OutI = 0; // Sensor Strom	
//			OutU = 0; // Sensor Spannung	
				
			logStart=1;
		}
	}	
}

void ausgabehistory(void){
	uart_putc('\r');
	uart_puts("Historyausgabe: ");
	uart_puts(tab); 
	uart_puts(abtastzeitMAXText); 		
	convert.Byte[0] = EEPReadByte(&historyAzMAX[0]);
	convert.Byte[1] = EEPReadByte(&historyAzMAX[1]);					
	itoa(convert.Word,texth,10);  
	uart_puts(texth);						
	uart_putc('\r');

	uart_puts("\rt in 0,1s\tlinks\tvorne\trechts\tServo\tFahrr\r");	
	//	uart_puts("\rt in 0,1s\tlinks\tvorne\trechts\tServo\tFahrr\tStrom\tSpannung\tAZ_\tAZ_MAX\r");;	
	uart_putc('\r');
	for(int i=0; i < historyDepthPointer; i++){
			
		itoa(i,texth,10);	  // wandle die Ausgabezeit (interger) in acsii um
		strcat(texth,tab);  // Text anhaengen
		uart_puts(texth);
		
		if (logCmOnOff == 0) {
			convert.Byte[0] = EEPReadByte(&historyDepthL[i][0]);
			convert.Byte[1] = EEPReadByte(&historyDepthL[i][1]);
			itoa(convert.Word,texth,10);  // Sensor Links, analog 
			strcat(texth,tab);	
			uart_puts(texth);

			convert.Byte[0] = EEPReadByte(&historyDepthV[i][0]);
			convert.Byte[1] = EEPReadByte(&historyDepthV[i][1]);
			itoa(convert.Word,texth,10);  // Sensor Vorne, analog		 
			strcat(texth,tab);	
			uart_puts(texth);

			convert.Byte[0] = EEPReadByte(&historyDepthR[i][0]);
			convert.Byte[1] = EEPReadByte(&historyDepthR[i][1]);
			itoa(convert.Word,texth,10);  // Sensor Rechts, analog			 
			strcat(texth,tab);	
			uart_puts(texth);
			
		}
		else{
			convert.Byte[0] = EEPReadByte(&historyDepthL[i][0]);
			convert.Byte[1] = EEPReadByte(&historyDepthL[i][1]);
			itoa(linearisierungAD(convert.Word,70),texth,10);  // Sensor Links, analog	 
			strcat(texth,tab);	
			uart_puts(texth);

			convert.Byte[0] = EEPReadByte(&historyDepthV[i][0]);
			convert.Byte[1] = EEPReadByte(&historyDepthV[i][1]);
			itoa(linearisierungAD(convert.Word,100),texth,10);  // Sensor Vorne, analog		 
			strcat(texth,tab);	
			uart_puts(texth);
		
			convert.Byte[0] = EEPReadByte(&historyDepthR[i][0]);
			convert.Byte[1] = EEPReadByte(&historyDepthR[i][1]);		
			itoa(linearisierungAD(convert.Word,70),texth,10);  // Sensor Rechts, analog
			strcat(texth,tab);	
			uart_puts(texth);

		}
		convert.Byte[0] = EEPReadByte(&historyDepthS[i][0]);
		convert.Byte[1] = EEPReadByte(&historyDepthS[i][1]);				
		itoa(getServo(convert.Word),texth,10);  			// Servo / Lenkung		 
		strcat(texth,tab);	
		uart_puts(texth);	

		convert.Byte[0] = EEPReadByte(&historyDepthFR[i][0]);
		convert.Byte[1] = EEPReadByte(&historyDepthFR[i][1]);			
		itoa(getFahr(convert.Word),texth,10);  			// Antrieb / Fahrtenregler		 
		strcat(texth,tab);	
		uart_puts(texth);		
			
/*		
		convert.Byte[0] = EEPReadByte(&historyDepthI[i][0]); // Sensor Strom	
		convert.Byte[1] = EEPReadByte(&historyDepthI[i][1]);			
		itoa(convert.Word,texth,10);  					 
		strcat(texth,tab);	
		uart_puts(texth);		
		
		convert.Byte[0] = EEPReadByte(&historyDepthU[i][0]); // Sensor Spannung	
		convert.Byte[1] = EEPReadByte(&historyDepthU[i][1]);			
		itoa(convert.Word,texth,10);  					 
		strcat(texth,tab);	
		uart_puts(texth);	
*/	
		uart_putc('\r');
	}
	uart_putc('\r');
	abtastzeit=0; //Auf Null setzen
	abtastzeitMAX=0;
}

int8_t getServo(int16_t wertOCR1A){

	//Umsetzung von -10 bis +10 auf Servoparameter
	if(wertOCR1A == servoG)	      return 0;       //gerade
	else if(wertOCR1A == servoMR)	return 10;			//max rechts
	else if(wertOCR1A == servoML)	return -10;		//max links
	else if(wertOCR1A > servoG) return ((wertOCR1A - servoG) / ((servoML - servoG) / -10)); //links
	else                        return ((servoG - wertOCR1A) / ((servoG - servoMR) / 10)); //rechts
}

int8_t getFahr(int16_t wertOCR1B){
	//Umsetzung von -100 bis +100 auf Fahrtenreglerparameter
	if(wertOCR1B == STOP)	      return 0;       //gerade
	else if(wertOCR1B == MAXVORWAERTS)	return 100;			//max rechts
	else if(wertOCR1B == MAXRUEWAERTS)	return -100;		//max links
	else return ((int16_t)(wertOCR1B - STOP ) / ((int16_t)(MAXVORWAERTS - STOP) / 100));
}

