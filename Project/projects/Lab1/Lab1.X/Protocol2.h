/* 
 * File:   Protocol2.h
 * Author: leafg
 *
 * Created on February 9, 2023, 8:03 PM
 */

#ifndef PROTOCOL2_H
#define	PROTOCOL2_H


/*******************************************************************************
 * PUBLIC #DEFINES                                                            *
 ******************************************************************************/

#define PACKETBUFFERSIZE 16
#define MAXPAYLOADLENGTH 128 
#define DEBUG 0 
#define HEAD 0xCC
#define TAIL 0xB9
#define ENDR 0x0D
#define ENDN 0x0A
#define failed
/*******************************************************************************
 * PUBLIC DATATYPES
 ******************************************************************************/

typedef struct rxpT {
    int L;
    int C;
    uint8_t ID;
    uint8_t len;
    uint8_t checkSum;
    unsigned char payLoad[MAXPAYLOADLENGTH];
} *rxpADT;

typedef struct rx_buff {
    int head;
    int tail;
    rxpADT buffer[buff_size];
    int size;
    int full;
    int empty;
}rx_buff;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/
/**
 * @Function Protocol_Init(baudrate)
 * @param Legal Uart baudrate
 * @return SUCCESS (true) or ERROR (false)
 * @brief Initializes Uart1 for stream I/O to the lab PC via a USB 
 *        virtual comm port. Baudrate must be a legal value. 
 * @author instructor W2022 */
int Protocol_Init(unsigned long baudrate);

/**
 * @Function unsigned char Protocol_QueuePacket()
 * @param none
 * @return the buffer full flag: 1 if full
 * @brief Place in the main event loop (or in a timer) to continually check 
 *        for completed incoming packets and then queue them into 
 *        the RX circular buffer. The buffer's size is set by constant
 *        PACKETBUFFERSIZE.
 * @author instructor W2023 */
uint8_t Protocol_QueuePacket();

/**
 * @Function int Protocol_GetInPacket(uint8_t *type, uint8_t *len, uchar *msg)
 * @param *type, *len, *msg
 * @return SUCCESS (true) or WAITING (false)
 * @brief Reads the next packet from the packet Buffer 
 * @author instructor W2022 */
int Protocol_GetInPacket(uint8_t *type, uint8_t *len, unsigned char *msg);

/**
 * @Function int Protocol_SendDebugMessage(char *Message)
 * @param Message, Proper C string to send out
 * @return SUCCESS (true) or ERROR (false)
 * @brief Takes in a proper C-formatted string and sends it out using ID_DEBUG
 * @warning this takes an array, do <b>NOT</b> call sprintf as an argument.
 * @author mdunne */
int Protocol_SendDebugMessage(char *Message);

/**
 * @Function int Protocol_SendPacket(unsigned char len, void *Payload)
 * @param len, length of full <b>Payload</b> variable
 * @param Payload, pointer to data
 * @return SUCCESS (true) or ERROR (false)
 * @brief composes and sends a full packet
 * @author instructor W2022 */
int Protocol_SendPacket(unsigned char len, unsigned char ID, void *Payload);

/**
 @Function unsigned char Protocol_ReadNextID(void)
 * @param None
 * @return Reads the ID of the next available Packet
 * @brief Returns ID or 0 if no packets are available
 * @author instructor W2022 */
unsigned char Protocol_ReadNextPacketID(void);

/**
 * @Function flushPacketBuffer()
 * @param none
 * @return none
 * @brief flushes the rx packet circular buffer  
 * @author instructor W2022 */
void flushPacketBuffer();


void RXP_buff_init(void);
int RXP_buff_add(rxpADT rxPacket);
rxpADT RXP_buff_get(void);

unsigned int convertEndian(unsigned int *);
/*******************************************************************************
 * PRIVATE FUNCTIONS
 * Generally these functions would not be exposed but due to the learning nature 
 * of the class some are are noted to help you organize the code internal 
 * to the module. 
 ******************************************************************************/

/* BuildRxPacket() should implement a state machine to build an incoming
 * packet incrementally and return it completed in the called argument packet
 * structure (rxPacket is a pointer to a packet struct). The state machine should
 * progress through discrete states as each incoming byte is processed.
 * 
 * To help you get started, the following ADT is an example of a structure 
 * intended to contain a single rx packet. 
 * typedef struct rxpT {
 *    uint8_t ID;      
 *    uint8_t len;
 *    uint8_t checkSum; 
 *    unsigned char payLoad[MAXPAYLOADLENGTH];
 * }*rxpADT; 
 *   rxpADT rxPacket ...
 * Now consider how to create another structure for use as a circular buffer
 * containing a PACKETBUFFERSIZE number of these rxpT packet structures.
 ******************************************************************************/
uint8_t BuildRxPacket(rxpADT rxPacket, unsigned char reset);

/**
 * @Function char Protocol_CalcIterativeChecksum(unsigned char charIn, unsigned char curChecksum)
 * @param charIn, new char to add to the checksum
 * @param curChecksum, current checksum, most likely the last return of this function, can use 0 to reset
 * @return the new checksum value
 * @brief Returns the BSD checksum of the char stream given the curChecksum and the new char
 * @author mdunne */
unsigned char Protocol_CalcIterativeChecksum(unsigned char charIn, unsigned char curChecksum);


/**
 * @Function int Protocol_ChecksumChecker(unsigned char ID, unsigned char *payload, unsigned char checksum)
 * @param ID, used in checksum
 * @param payload, payload to be checked
 * @checksum, value to be checked
 * @return if the checksum matches
 * @brief Returns the BSD checksum of the char stream given the curChecksum and the new char
 * @author mdunne */
int Protocol_ChecksumChecker(unsigned char ID, unsigned char *Payload, unsigned char checksum, int len);

/**
 * This macro initializes all LEDs for use. It enables the proper pins as outputs and also turns all
 * LEDs off.
 */
#define LEDS_INIT() do {LATECLR = 0xFF; TRISECLR = 0xFF;} while (0)

/**
 * Provides a way to quickly get the status of all 8 LEDs into a uint8, where a bit is 1 if the LED
 * is on and 0 if it's not. The LEDs are ordered such that bit 7 is LED8 and bit 0 is LED0.
 */
#define LEDS_GET() (LATE & 0xFF)

/**
 * This macro sets the LEDs on according to which bits are high in the argument. Bit 0 corresponds
 * to LED0.
 * @param leds Set the LEDs to this value where 1 means on and 0 means off.
 */
#define LEDS_SET(leds) do { LATE = (leds); } while (0)
#endif	/* PROTOCOL_H */


