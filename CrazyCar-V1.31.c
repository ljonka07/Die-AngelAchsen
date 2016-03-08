/*Werner Jessen + Dieter Rathmann */
/*Version: CrazyCar-V1.31 */
/*Prüfung der AkkuSpannung, neue USB-Ausgabe, ledSchalterTest() */
/* nicht mehr benötigte Programmteile entfernt */
/* Historie wird in EERAM geschrieben, neue Kommandos */
/* Mako's fuer Outputs, FR+Servo */

#include <avr/io.h>

#include "global.h"
#include "pwm.h"
#include "adc.h"
#include "uart.h"
#include "zyklus.h"
#include "df.h"
#include "EinAusgabeUSB.h"
#include "myFunktions.h"
#include "cc-lib.h"


int main (void)
{
	initMikroController();     //Initialisierung des Mikrocontrollers
	// while(1) { testServo(); }//<== Initialisierung  Servo, 1X <==========
	
	initServo(856,732,624);  //<== Volllinks, Gerade, Vollrechts eintragen, Servo, 1X <==========
	initFahr(50);					// klein anfangen, bis Fahrzeug sicher faehrt
	
	servo(0);						//Ansteuerung des Servos (GERADEAUS)
	fahr(0);      					//Ansteuerung des Fahrtenreglers (STOP)
	
	//Schalter auswerten und Einstellung merken 
	if ( s1()==0 )
		if ( s2()==0 )	mod = 0;		//Terminalbetrieb
		else				mod = 1;		//fahren1
	else
		if ( s2()==0 )	mod = 2;		//fahren2
		else				mod = 3;		//fahren3
			
	if( s3() == 1 ){	// Datenspeichern und Fahrzeug anhalten nach x Sekunden
		ledPB2(1);	//LED PB2 an			
		historyDepthPointer=0; 		//neue Datenaufzeichung
		hspStart=1;			// beginne Speicherung bein naechsten Block		
	}
			
	if ( s4() ) {
		ledPB1(1);  //LED an
		akkuSpannungPruefen(800); // Schalter S4 an, Grenzwert übergeben
	}


//Endlosschleife mit freier Zykluszeit   
	while(1) {			
			
	//Sensordaten einlesen
		analogwertL=adc(0);			// lese Sensor links
		analogwertR=adc(1);			// lese Sensor rechts
		analogwertV=adc(2);			// lese Sensor vorne
	//	analogwertI=adc(3);			// Sensor Strom	
	//	analogwertU=adc(4);			// Sensor Spannung
		analogwertAkku=adc(7);		// lese AkkuSpannung
		
//hier erfolgt die Umrechnung der digitalen Abstandswerte
// in den Abstandswert in cm (abstandlinks,abstandrechts,abstandvorne)
		abstandlinks  = linearisierungAD_links(analogwertL, 70); //<== Eigene Funktion schreiben
		abstandrechts = linearisierungAD_rechts(analogwertR, 70); //analogwert, cosinusAlpha*100
		abstandvorne  = linearisierungAD_vorne(analogwertV, 100);	
		

//konstante zeitliche Abarbeitung des Steuerzyklusses
//Abtastzeit wird mit Timerinterrupt erhöht, im Hintergrund
//ABTASTZEIT ist in gobal.h definiert, siehe zyklus.c
		if (abtastzeit>=ABTASTZEIT)  
		{
			abtastzeit=0;             		//abtastzeit auf 0 zurücksetzen
			
			datenSatzZaehler++;				// Anzahl der digitalisierten Analogwerte, pro Runde +1
			datenspeichern();				// Mittelwertbildung und speichern nach Anforderung zur Analyse
			
			//ledSchalterTest();			//<== Eigene Funktion schreiben
			//Schalterauswertung: s1() und s2()
			
			switch(mod){
				case	0:  	ledPC3(0); ledPC2(0); //nur Terminal Ein- u. Ausgabe
								break;
				case	1:  	ledPC3(0); ledPC2(1); //Fahrzeug faehrt via Software
								fahren1();
								break;  
				case	2:  	ledPC3(1); ledPC2(0); //Fahrzeug faehrt via Software
								fahren2();   
								break;
				case	3:  	ledPC3(1); ledPC2(1); //Fahrzeug faehrt via Software
								fahren3();   
								break;
								
				case	9:  	ledPC2_flash(100); ledPC3_flash(100,0);	 //Fahrzeug Stopp	
								fahr(0);    
								break;
								
				default: 	ledPC2_flash(30); ledPC3_flash(30,1); // nichts definiert
								break; 
			}
			terminal_aktiv();
			
			//Wenn s3 gesetzt ist oder Kommando hs, 
			//dann fahre solange bis der Datenspeicher voll ist.
			//Dient zur Auswertung der Strecke (Optimierung)		
			if (historyDepthPointer >= HISTORYDEPTH){ //Datenspeicher voll?
				servo(0);		  	// Lenkung: Gerade
				fahr(0);      	  	// Fahrtenregler: Stop
				mod = 0;			//default
			}	
			//Starte neu, wenn die Logzeit > 650 Sekunden
			if (datenSatzZaehler >= 65000) datenSatzZaehler = 0; 
		}//endzyklus	
	}//endfor
	return (0);
}//endmain
