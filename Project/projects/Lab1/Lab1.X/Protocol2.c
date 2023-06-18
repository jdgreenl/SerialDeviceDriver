#include <sys/attribs.h> 
#include "uart.h"
#include <BOARD.h>
#include <xc.h>
#include <math.h>
#include "lib121.h"
#include <stdlib.h>
#include "Protocol2.h"
#include <MessageIDs.h>
#include <string.h>
#include <stdio.h>


#define PROTOCOL_TESTHARNESS

extern c_buff RX_buff;


rx_buff RXP_buff;  // Packet Buffer
rxpADT Packsack;   // Packet pointer
unsigned char PayLoad[MAXPAYLOADLENGTH];  // test variable
int curr_state;    // Global state variable |NEEDED BuildRxPacket() 


// RX state machine/Packet builder
uint8_t BuildRxPacket(rxpADT rxPacket, unsigned char reset) {
    if (reset == 'r') {  // unused in this edition/Potential reseter
        //memset(PayLoad, 0, sizeof(PayLoad)); // Attempt to wipe memory/Useless for now 
        curr_state = 0; // 0 is base state for system
    }
    if (RX_buff.empty == 1) { // extern variable//Checks RX_buff from uart.c
        return '\0';
    }
    uint8_t complete = '1';
    uint8_t incomplete = '0';
    unsigned char got;
    got = GetChar();  // GetChar() from uart.c // removes uint_8 from the rx buffer
    unsigned char checksum;
    int c;
    
    unsigned char test[2];
    unsigned char len;
    len = 0x02;
    test[0] = got;

    switch (curr_state) {
        case 0:
            if (got == HEAD) {
                //     Protocol_SendPacket(len, ID_DEBUG, test);
                curr_state = 1;
            }
            break;
        case 1:
            rxPacket->len = (uint8_t) got;
            rxPacket->L = (int) got; //casts got into int
            rxPacket->L--;
            curr_state = 2;
            //  Protocol_SendPacket(len, ID_DEBUG, test);
            break;

        case 2:
            // Protocol_SendPacket(len, ID_DEBUG, test);
            rxPacket->ID = got;
            rxPacket->C = 0;
            curr_state = 3;
            if(rxPacket->L == 0){
                curr_state =4;
            }
            break;
        case 3:
            //Protocol_SendPacket(len, ID_DEBUG, test);
            rxPacket->payLoad[rxPacket->C] = got;
            rxPacket->C++;
            if (rxPacket->L == rxPacket->C) {
                curr_state = 4;
            }
            break;
        case 4:
            // Protocol_SendPacket(len, ID_DEBUG, test);
            if (got == TAIL) {
                curr_state = 5;
            }
            break;
        case 5:
            // Protocol_SendPacket(len, ID_DEBUG, test);
            c = Protocol_ChecksumChecker(rxPacket->L, rxPacket->payLoad, got, rxPacket->L);
            if (c == 1) {
                curr_state = 6;
            } else {
                curr_state = 0;
            }
            break;
        case 6:
            //Protocol_SendPacket(len, ID_DEBUG, test);
            if (got == ENDR) {
                curr_state = 7;
            }
            break;
        case 7:
            //Protocol_SendPacket(len, ID_DEBUG, test);
            if (got == ENDN) {
                curr_state = 0;
                return got;
            }
            curr_state = 0;
            break;

    }
    return got;
}

/**
 * @Function char Protocol_CalcIterativeChecksum(unsigned char charIn, unsigned char curChecksum)
 * @param charIn, new char to add to the checksum
 * @param curChecksum, current checksum, most likely the last return of this function, can use 0 to reset
 * @return the new checksum value
 * @brief Returns the BSD checksum of the char stream given the curChecksum and the new char
 * @author mdunne */
unsigned char Protocol_CalcIterativeChecksum(unsigned char charIn, unsigned char curChecksum) {

    curChecksum = (curChecksum >> 1) + (curChecksum << 7);
    curChecksum += charIn;
    return curChecksum;

}

int Protocol_ChecksumChecker(unsigned char ID, unsigned char *Payload, unsigned char checksum, int len) {
    unsigned char curChecksum = 0;
    curChecksum = Protocol_CalcIterativeChecksum(ID, curChecksum);
    int C = 0;
    unsigned char curr_pay;
    while (C < len) {
        curr_pay = *(unsigned char*) Payload;
        curChecksum = Protocol_CalcIterativeChecksum(curr_pay, curChecksum);
        C++;
        if (C != len) {
            Payload = (unsigned char*) Payload + 1;
        }
    }

    if (checksum == curChecksum) {
        return 1;
    } else {
        return 0;
    }


}

int Protocol_SendPacket(unsigned char len, unsigned char ID, void *Payload) {

    unsigned char Checksum;
    unsigned char curr_pay;
    Checksum = 0;
    int L = (int) len;
    L--;
    int C = 0;
    if (L > MAXPAYLOADLENGTH) {
        return 0;
    }

    PutChar(HEAD);
    PutChar(len);
    PutChar(ID);
    Checksum = Protocol_CalcIterativeChecksum(ID, Checksum);
    while (C < L) {
        curr_pay = *(unsigned char*) Payload;
        Checksum = Protocol_CalcIterativeChecksum(curr_pay, Checksum);
        PutChar(curr_pay);
        C++;
        if (C != L) {
            Payload = (unsigned char*) Payload + 1;
        }
    }
    PutChar(TAIL);
    PutChar(Checksum);
    // printf("\r\nchecksum: %X\r\n",Checksum);
    PutChar(ENDR);
    PutChar(ENDN);
    return 1;


}

void flushPacketBuffer() {

    RXP_buff.empty = 1;
    RXP_buff.full = 0;
    RXP_buff.head = 0;
    RXP_buff.tail = 0;
    RXP_buff.size = 0;


}

int Protocol_GetInPacket(uint8_t *type, uint8_t *len, unsigned char *msg) {
    if (RXP_buff.empty == 1) {
        return 0;
    }
    type = &RXP_buff.buffer[RXP_buff.tail]->ID;
    len = &RXP_buff.buffer[RXP_buff.tail]->len;
    msg = &RXP_buff.buffer[RXP_buff.tail]->payLoad[0];
    return 1;
}

unsigned char Protocol_ReadNextPacketID(void) {
    if (RXP_buff.empty == 1) {
        return 0;
    }
    unsigned char id;
    id = RXP_buff.buffer[RXP_buff.tail]->ID;
    return id;
}

void rxp_buff_init(void) {
    RXP_buff.size = buff_size;
    RXP_buff.full = 0;
    RXP_buff.empty = 1;
    RXP_buff.head = 0;
    RXP_buff.tail = 0;

}

int RXP_buff_add(rxpADT packet) {
    if (RXP_buff.full == 1) {
        return 0;
    }

    RXP_buff.buffer[RXP_buff.head] = packet;
    RXP_buff.head = (RXP_buff.head + 1) % buff_size;
    if (RXP_buff.empty) {
        RXP_buff.empty = 0;
    }
    if (RXP_buff.head == RXP_buff.tail) {
        RXP_buff.full = 1;
    }
    return 1;
}

rxpADT RXP_buff_get(void) {
    if (RXP_buff.empty) {
        return '\0';
    }
    rxpADT popped;
    popped = RXP_buff.buffer[RXP_buff.tail];
    RXP_buff.tail = (RXP_buff.tail + 1) % buff_size;
    if (RXP_buff.tail == RXP_buff.head) {
        RXP_buff.empty = 1;
    }
    if (RXP_buff.full) {
        RXP_buff.full = 0;
    }
    return popped;
}

int Protocol_Init(unsigned long baudrate) {
    Uart_Init(baudrate);
    rxtx_buff_init();
    rxp_buff_init();
    curr_state = 0;

    return 1;

}

uint8_t Protocol_QueuePacket() {
    uint8_t f;
    uint8_t LEDs;
    unsigned char send_LEDs[1];
    unsigned char send_pong[4];
    unsigned char pong_array[4];
    unsigned  char Len;
    rxpADT p;
    unsigned int x;
    int i;
    int j;
    p = RXP_buff_get();
    
    if(p != '\0'){
        if(p->ID == ID_LEDS_GET){
            LEDs = LEDS_GET();
            Len = 2;
            send_LEDs[0] = (unsigned char)LEDs;
            Protocol_SendPacket(Len,ID_LEDS_STATE,send_LEDs);
        } else if (p->ID == ID_LEDS_SET){
            send_LEDs[0] = p->payLoad[0];
            LEDS_SET(p->payLoad[0]);
            Len = 2;
            //send_LEDs[0] = LEDs;
            //Protocol_SendPacket(Len,ID_LEDS_STATE,send_LEDs);
            
        } else if (p->ID == ID_PING){
            Len = 5;
            send_pong[3] = p->payLoad[3];
            send_pong[2] = p->payLoad[2];
            send_pong[1] = p->payLoad[1];
            send_pong[0] = p->payLoad[0];
            //Protocol_SendPacket(Len, ID_PONG, send_pong);
            for(i = 0; i < sizeof(send_pong); i++){
                x = (x<<8) | send_pong[i];
            }
            x = x >> 1;
            for(j = sizeof(x)-1; j >= 0; j--){
                pong_array[j] = x & 0xff;
                x >>= 8;
            }
            //Protocol_SendPacket(Len, ID_PONG, pong_array);
            send_pong[3] = pong_array[3];
            send_pong[2] = pong_array[2];
            send_pong[1] = pong_array[1];
            send_pong[0] = pong_array[0];
            Protocol_SendPacket(Len, ID_PONG, send_pong);
        }
        
    }

    return '\0';

}



#ifdef PROTOCOL_TESTHARNESS

int main(void) {
    BOARD_Init();
    int i;
    unsigned char c;
    unsigned long bd;
    rxtx_buff_init();
    bd = 21;
    Protocol_Init(bd);
    LEDS_INIT();
    //LEDS_SET(0xff);

    unsigned char Len = 0x0A;
    unsigned char Len_2 = 0x02;
    unsigned char L;
    char *str = "hello";
    char new[] = {0x00, 0x25, 0x7D, 0x96};
    unsigned char test[10] = "workplease";
    unsigned char opening[60];
    sprintf(opening, "\r\nProtocol2 Code Compilation Time:%s, %s\r\n", __DATE__, __TIME__);
    unsigned char gotten[20];

    struct rxpT Packet;

    rxpADT Ppdoo;
    Ppdoo = &Packet;

    unsigned char got;

    uint8_t f;
    uint8_t r = '\0';


    L = strlen(opening);
    //printf("%s,  %d", opening, L);
    Protocol_SendPacket(L, ID_DEBUG, opening);

    //unsigned char t[2] = {0x80};
    //Protocol_SendPacket(Len_2, 0x80, t);

    while (1) {
        //Protocol_SendPacket(L, ID_DEBUG, opening);

        f = BuildRxPacket(Ppdoo, r);
        gotten[0] = f;
        if (f != '\0') {
            sprintf(gotten, "\r\n0x%02X\r\n", f);
            L = strlen(gotten);
            //Protocol_SendPacket(L, ID_DEBUG, gotten);
            if(f==ENDN){
                curr_state = 0;
                RXP_buff_add(Ppdoo);
            }
        }
        
        Protocol_QueuePacket();
        /*
        got = GetChar();
        
        if(got != 0){
            test[0] = got;
            Protocol_SendPacket(Len, ID_DEBUG, test);
        }
         
         
        for (i = 0; i < 1000; i++) {
            asm("Nop");
        }
*/
    }
    BOARD_End();

}


#endif

