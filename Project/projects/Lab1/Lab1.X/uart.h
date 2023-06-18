
/* 
 * File:   uart.h
 * Author: jdgreenl
 *
 * Created on January 27, 2023, 10:53 AM
 */
#define UART_H
#ifdef UART_H
#include "lib121.h"
/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/
typedef struct c_buff {
    int head;
    int tail;
    unsigned char buffer[buff_size];
    int size;
    int full;
    int empty;
}c_buff;
/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/
/**
 * @Function Uart_Init(unsigned long baudrate)
 * @param baudrate
 * @return none
 * @brief  Initializes UART1 to baudrate N81 and creates circ buffers
 * @author instructor ece121 W2022 */
void Uart_Init(unsigned long baudRate);
/**
 * @Function int PutChar(char ch)
 * @param ch - the character to be sent out the serial port
 * @return True if successful, else False if the buffer is full or busy.
 * @brief  adds char to the end of the TX circular buffer
 * @author instrutor ECE121 W2022 */
int PutChar(char ch);

/**
 * @Function Uart_tx(char ch)
 * @param char ch
 * @return int
 * @brief 
 * @author <Your Name>*/
int Uart_TX(char ch);
/**
 * @Function unsigned char GetChar(void)
 * @param None.
 * @return NULL for error or a Char in the argument.
 * @brief  dequeues a character from the RX buffer,
 * @author instructor, ECE121 W2022 */
unsigned char GetChar(void);


/**
 * @Function Uart_RX(void)
 * @param void
 * @return unsigned char
 * @brief 
 * @author <Your Name>*/
unsigned char Uart_RX(void);


/**
 * @Function c_buff_init(struct c_buff circ);
 * @param circular buffer
 * @return void
 * @brief Inits Circular buffer */
void c_buff_init(c_buff circ);

void rxtx_buff_init(void);

int c_buff_add(c_buff circ, char c);
int TX_buff_add(char c);
int RX_buff_add(char c);

unsigned char c_buff_get(c_buff circ);
unsigned char TX_buff_get(void);
unsigned char RX_buff_get(void);
//unsigned char *

void NOP_delay_5ms(void);
#endif // UART_H

