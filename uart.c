/*************************************************************************
Title:    Interrupt USART library with receive/transmit circular buffers
Author:   
File:     $Id: uart.c,v 1.0 2006/05/23 19:59:12 $
Software: AVR-GCC 3.3 
Hardware: any AVR with built-in USART, 
          tested on ATmega32 at 8Mhz

DESCRIPTION:
    An interrupt is generated when the USART has finished transmitting or
    receiving a byte. The interrupt handling routines use circular buffers
    for buffering received and transmitted data.
    
    The USART_RX_BUFFER_SIZE and USART_TX_BUFFER_SIZE variables define
    the buffer size in bytes. Note that these variables must be a 
    power of 2.
    
USAGE:
    Refere to the header file uart.h for a description of the routines. 
    See also example test_uart.c.

NOTES:
    Based on Atmel Application Note AVR306
                    
*************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "uart.h"


/*
 *  constants and macros
 */

/* size of RX/TX buffers */
#define USART_RX_BUFFER_MASK ( USART_RX_BUFFER_SIZE - 1)
#define USART_TX_BUFFER_MASK ( USART_TX_BUFFER_SIZE - 1)

#if ( USART_RX_BUFFER_SIZE & USART_RX_BUFFER_MASK )
#error RX buffer size is not a power of 2
#endif
#if ( USART_TX_BUFFER_SIZE & USART_TX_BUFFER_MASK )
#error TX buffer size is not a power of 2
#endif

  /* ATmega with one USART */

#define USART_STATUS   UCSRA
#define USART_CONTROL  UCSRB
#define USART_DATA     UDR
#define USART_UDRIE    UDRIE

/*
 *  module global variables
 */
static volatile unsigned char USART_TxBuf[USART_TX_BUFFER_SIZE];
static volatile unsigned char USART_RxBuf[USART_RX_BUFFER_SIZE];
static volatile unsigned char USART_TxHead;
static volatile unsigned char USART_TxTail;
static volatile unsigned char USART_RxHead;
static volatile unsigned char USART_RxTail;
static volatile unsigned char USART_LastRxError;


ISR(USART_RXC_vect)
/*************************************************************************
Function: USART Receive Complete interrupt
Purpose:  called when the USART has received a character
**************************************************************************/
{
    unsigned char tmphead;
    unsigned char data;
    unsigned char usr;
    unsigned char lastRxError;
 
 
    /* read USART status register and USART data register */ 
    usr  = USART_STATUS;
    data = USART_DATA;
    
    /* */
    lastRxError = (usr & ((1<<FE)|(1<<DOR))) ;
        
    /* calculate buffer index */ 
    tmphead = ( USART_RxHead + 1) & USART_RX_BUFFER_MASK;
    
    if ( tmphead == USART_RxTail ) {
        /* error: receive buffer overflow */
        lastRxError = USART_BUFFER_OVERFLOW >> 8;
    }else{
        /* store new index */
        USART_RxHead = tmphead;
        /* store received data in buffer */
        USART_RxBuf[tmphead] = data;
    }
    USART_LastRxError = lastRxError;   
}


ISR(USART_UDRE_vect)
/*************************************************************************
Function: USART Data Register Empty interrupt
Purpose:  called when the USART is ready to transmit the next byte
**************************************************************************/
{
    unsigned char tmptail;

    
    if ( USART_TxHead != USART_TxTail) {
        /* calculate and store new buffer index */
        tmptail = (USART_TxTail + 1) & USART_TX_BUFFER_MASK;
        USART_TxTail = tmptail;
        /* get one byte from buffer and write it to USART */
        USART_DATA = USART_TxBuf[tmptail];  /* start transmission */
    }else{
        /* tx buffer empty, disable UDRE interrupt */
        USART_CONTROL &= ~(1<<USART_UDRIE);
    }
}


/*************************************************************************
Function: uart_init()
Purpose:  initialize USART and set baudrate
Input:    baudrate using macro USART_BAUD_SELECT()
Returns:  none
**************************************************************************/
void uart_init(unsigned int baudrate)
{
    USART_TxHead = 0;
    USART_TxTail = 0;
    USART_RxHead = 0;
    USART_RxTail = 0;
    
    /* Set baud rate */
    if ( baudrate & 0x8000 )
    {
    	 USART_STATUS = (1<<U2X);  //Enable 2x speed 
    	 baudrate &= ~0x8000;
    }
/*
    UBRRH = (unsigned char)(baudrate>>8);
    UBRRL = (unsigned char) baudrate;
*/   
     UBRRH = 00 ;
    UBRRL = 51 ;
   /* Enable USART receiver and transmitter and receive complete interrupt */
    USART_CONTROL = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
    
    /* Set frame format: asynchronous, 7-- 8je--  data, odd no-je parity, 1 stop bit */
    
   // UCSRC = (1<<URSEL)|(1<<UPM1)|(1<<UPM0)|(1<<UCSZ1)|(1<<UCSZ0);
         UCSRC = (1<<URSEL)|(1<<UCSZ1)|(1<<UCSZ0);
}/* uart_init */


/*************************************************************************
Function: uart_getc()
Purpose:  return byte from ringbuffer  
Returns:  lower byte:  received byte from ringbuffer
          higher byte: last receive error
**************************************************************************/
unsigned int uart_getc(void)
{    
    unsigned char tmptail;
    unsigned char data;


    if ( USART_RxHead == USART_RxTail ) {
        return USART_NO_DATA;   /* no data available */
    }
    
    /* calculate /store buffer index */
    tmptail = (USART_RxTail + 1) & USART_RX_BUFFER_MASK;
    USART_RxTail = tmptail; 
    
    /* get data from receive buffer */
    data = USART_RxBuf[tmptail];
    
    return (USART_LastRxError << 8) + data;

}/* uart_getc */


/*************************************************************************
Function: uart_putc()
Purpose:  write byte to ringbuffer for transmitting via USART
Input:    byte to be transmitted
Returns:  none          
**************************************************************************/
void uart_putc(unsigned char data)
{
    unsigned char tmphead;

    
    tmphead  = (USART_TxHead + 1) & USART_TX_BUFFER_MASK;
    
    while ( tmphead == USART_TxTail ){
        ;/* wait for free space in buffer */
    }
    
    USART_TxBuf[tmphead] = data;
    USART_TxHead = tmphead;

    /* enable UDRE interrupt */
    USART_CONTROL    |= (1<<USART_UDRIE);

}/* uart_putc */


/*************************************************************************
Function: uart_puts()
Purpose:  transmit string to USART
Input:    string to be transmitted
Returns:  none          
**************************************************************************/
void uart_puts(const char *s )
{
    while (*s) 
      uart_putc(*s++);

}/* uart_puts */


/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to USART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts_p(const char *progmem_s )
{
    register char c;
    
    while ( (c = pgm_read_byte(progmem_s++)) ) 
      uart_putc(c);

}/* uart_puts_p */


