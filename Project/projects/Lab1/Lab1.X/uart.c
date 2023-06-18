


#include <sys/attribs.h> 
#include "uart.h"
#include <BOARD.h>
#include <xc.h>
#include <math.h>
#include "lib121.h"
#include <stdlib.h>
#include <stdio.h>
#include "Protocol2.h"
#include <string.h>
#include "MessageIDs.h"
#include <GenericTypeDefs.h>

//#define mainman 1
#define NOPS 5250
c_buff RX_buff;
c_buff TX_buff;

void NOP_delay_5ms() {
    int i;
    for (i = 0; i < NOPS; i++) {
        asm(" nop ");
    }
}

/**
 * @Function c_buff_init(struct c_buff circ);
 * @param circular buffer
 * @return void
 * @brief Inits Circular buffer */
void c_buff_init(c_buff circ) {

    circ.size = buff_size;
    circ.full = 0;
    circ.empty = 1;
    circ.head = 0;
    circ.tail = 0;
}

void rxtx_buff_init(void) {
    RX_buff.size = buff_size;
    RX_buff.full = 0;
    RX_buff.empty = 1;
    RX_buff.head = 0;
    RX_buff.tail = 0;


    TX_buff.size = buff_size;
    TX_buff.full = 0;
    TX_buff.empty = 1;
    TX_buff.head = 0;
    TX_buff.tail = 0;
}

int c_buff_add(c_buff circ, char c) {
    if (circ.full == 1) {
        return 0;
    }

    circ.buffer[circ.head] = c;
    circ.head = (circ.head + 1) % buff_size;
    if (circ.empty) {
        circ.empty = 0;
    }
    if (circ.head == circ.tail) {
        circ.full = 1;
    }
    return 1;
}

int TX_buff_add(char c) {
    if (TX_buff.full == 1) {
        return 0;
    }

    TX_buff.buffer[TX_buff.head] = c;
    TX_buff.head = (TX_buff.head + 1) % buff_size;
    if (TX_buff.empty) {
        TX_buff.empty = 0;
    }
    if (TX_buff.head == TX_buff.tail) {
        TX_buff.full = 1;
    }

    return 1;
}

int RX_buff_add(char c) {
    if (RX_buff.full == 1) {
        return 0;
    }

    RX_buff.buffer[RX_buff.head] = c;
    RX_buff.head = (RX_buff.head + 1) % buff_size;
    if (RX_buff.empty) {
        RX_buff.empty = 0;
    }
    if (RX_buff.head == RX_buff.tail) {
        RX_buff.full = 1;
    }
    return 1;
}

unsigned char c_buff_get(c_buff circ) {
    if (circ.empty) {
        return '\0';
    }
    unsigned char popped;
    popped = circ.buffer[circ.tail];
    circ.tail = (circ.tail + 1) % buff_size;
    if (circ.tail == circ.head) {
        circ.empty = 1;
    }
    if (circ.full) {
        circ.full = 0;
    }
    return popped;
}

unsigned char TX_buff_get(void) {
    if (TX_buff.empty) {
        return '\0';
    }
    unsigned char popped;
    popped = TX_buff.buffer[TX_buff.tail];
    TX_buff.tail = (TX_buff.tail + 1) % buff_size;
    if (TX_buff.tail == TX_buff.head) {
        TX_buff.empty = 1;
    }
    if (TX_buff.full) {
        TX_buff.full = 0;
    }
    return popped;
}

unsigned char RX_buff_get(void) {
    if (RX_buff.empty) {
        return '\0';
    }
    unsigned char popped;
    popped = RX_buff.buffer[RX_buff.tail];
    RX_buff.tail = (RX_buff.tail + 1) % buff_size;
    if (RX_buff.tail == RX_buff.head) {
        RX_buff.empty = 1;
    }
    if (RX_buff.full) {
        RX_buff.full = 0;
    }
    return popped;
}

/**
 * @Function Uart_Init(unsigned long baudRate)
 * @param baudRate
 * @return None.
 * @brief  Intitializes the UART SFRs
 * functions
 */
void Uart_Init(unsigned long baudRate) {
    U1BRG = baudRate;
    U1MODE = 0x8000; // Turns on Uart for 8 bit
    U1STA = 0x1410; //; // Turns on RX and TX    0x1400 0x5440

    IEC0bits.U1RXIE = 1; //enables interupts
    IEC0bits.U1TXIE = 1;
    IPC6bits.U1IP = 6;

    //INTCONbits.MVEC = 1;
    __builtin_enable_interrupts();

}

/**
 * @Function int PutChar(char ch)
 * @param ch - the character to be sent out the serial port
 * @return True if successful, else False if the buffer is full or busy.
 * @brief  adds char to the end of the TX circular buffer
 * @author instrutor ECE121 W2022 */
int PutChar(char ch) {
    while (!U1STAbits.TRMT);
    if (U1STAbits.UTXBF == 0) {
        int transmit;
        transmit = TX_buff_add(ch);
        IFS0bits.U1TXIF = 1;
        return transmit;
    }

    return 0;
}

/**
 * @Function unsigned char GetChar(void)
 * @param None.
 * @return NULL for error or a Char in the argument.
 * @brief  dequeues a character from the RX buffer,
 * @author instructor, ECE121 W2022 */
unsigned char GetChar(void) {
    unsigned char recieved;
    recieved = RX_buff_get();
    return recieved;

}

int Uart_TX(char ch) {
    if (U1STAbits.UTXBF) {
        return 0;
    } else {
        U1TXREG = ch;
        return 1;
    }
}

unsigned char Uart_RX(void) {
    unsigned char clear, recieved;
    if (U1STAbits.PERR || U1STAbits.FERR || U1STAbits.OERR) { //detects then clear errors
        clear = U1RXREG;
        U1STAbits.OERR = 0;
        return '\0';
    }

    if (U1STAbits.URXDA) { //checks if recieve buffer has data
        recieved = U1RXREG;
        return recieved;
    }
    return '\0';
}


/*******************************************************************************
 * PRIVATE FUNCTIONS to add to your Uart.c file                                    
 *
 ******************************************************************************/

/**
 * @Function _mon_putc(char c)
 * @param c - char to be sent
 * @return None.
 * @brief  overwrites weakly defined extern to use circular buffer
 *         instead of Microchip 
 * functions
 */
void _mon_putc(char c) {
    //"monitor put character" iostream to Monitor
    //stdio calls this function
    PutChar(c);
}

/**
 * @Function _mon_puts(const char* s)
 * @param s - pointer to the string to be sent
 * @return None.
 * @brief  overwrites weakly defined extern to use circular buffer instead of 
Microchip 
 *         functions
 */
void _mon_puts(const char* s) {
    //monitor put string
    //stdio calls this function
    int l;
    while (*s != '\0') {
        PutChar(*s);
        s++;
    }
    PutChar('\n');
}

/****************************************************************************
 * Function: IntUart1Handler
 * Parameters: None.
 * Returns: None.
 * The PIC32 architecture calls a single interrupt vector for both the 
 * TX and RX state machines. Each IRQ is persistent and can only be cleared
 * after "removing the condition that caused it".  
 ****************************************************************************/
void __ISR(_UART1_VECTOR, IPL6AUTO) IntUart1Handler(void) {
    //[your interrupt handler code goes here]
    //int p = IFS0bits.U1RXIF;
    //int q = IFS0bits.U1TXIF;
    if (IFS0bits.U1RXIF) {
        unsigned char clear, recieved;
        if (U1STAbits.PERR || U1STAbits.FERR || U1STAbits.OERR) { //detects then clear errors
            clear = U1RXREG;
            U1STAbits.OERR = 0;
        } else if (U1STAbits.URXDA) { //checks if recieve buffer has data
            recieved = U1RXREG;
            RX_buff_add(recieved);
        }
        IFS0bits.U1RXIF = 0;
    }

    if (IFS0bits.U1TXIF) {
        U1TXREG = TX_buff_get();
        IFS0bits.U1TXIF = 0;
    }
    return;
}



#ifdef mainman

int main(void) {
    BOARD_Init();

    char x;
    unsigned char c;
    unsigned long bd;
    //c_buff_init(RX_buff);
    //c_buff_init(TX_buff);
    rxtx_buff_init();
    bd = 21;
    Uart_Init(bd);
    x = '\0';
    // based off prof
    printf("\r\nConfiguration UART reference:");
    printf("\r\nU1MODE: %Xh, %dd", U1MODE, U1MODE);
    printf("\r\nU1STA: %Xh, %dd", U1STA, U1STA);
    printf("\r\nU1BRG: %dd", U1BRG);
    printf("\r\n***************************");
    int i;
    for (i = 0; i < 10000; i++) {
        asm("Nop");
    }
    printf("\n\r ****** RX Buffer Wrap Test ****** \n\r");
    while (1) {

        c = GetChar();
        if (c != '\0') {
            printf("%c", c);
        }
        //printf("hi");

        /*
        if (U1STAbits.URXDA == 1) {
            
            x = Uart_RX();
            Uart_TX(x);
        }
         */

        for (i = 0; i < 1000; i++) {
            asm("Nop");
        }

    }
    BOARD_End();

}



#endif


