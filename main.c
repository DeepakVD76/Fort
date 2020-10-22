#include <stdio.h>
#include <stdlib.h>
#include "msg_pdu.h"

int main()
{
    uint8_t i;
    struct Msg_PDU pdu;

    pdu.dest_addr = get_my_addr();
    pdu.src_addr = 0x100;
    pdu.body_len = 0x02;
    pdu.body[0] = 0x0A;
    pdu.body[1] = 0x0B;
    
    uint8_t txBuf[pdu.body_len + 9] , rxBuf[pdu.body_len + 9];
    /* 9 => 2 Byte : Preamble 0 & 1
            2 Byte : Src_Adr
            2 Byte : Dest_Addr
            1 Byte : Body_len
            2 Byte : Checksum
          --------------------
            9 Bytes
    Here we can not use directly the sizeof(pdu) instead of "pdu.body_len + 9" as the structure is define with the
    body length = MAX_BODY_LEN == 255 bytes. Its possible that the Body may contain lesser than 255 bytes
     in the body and hence the above declaration.
    */
    char status_tx_msg , status_rx_msg ;    // the status code is negative and hence defined as char instead of uint8_t as unsigned char.

//    while(1)
    {

    // call the below code whenever Transmission of Data is Required.
        status_tx_msg = prepareMessage(&pdu, txBuf, sizeof(txBuf));
        
        for(i = 0 ; i < sizeof(txBuf) ; i++)
            rxBuf[i] = txBuf[i];
            
        if(status_tx_msg != 1)
            printf("TX : Unknown Error");
         else
        {
            printf("\nTX : pdu.dest_addr = %d" , pdu.dest_addr);
            printf("\nTX : pdu.src_addr = %d" , pdu.src_addr);
            printf("\nTX : pdu.body_len = %d" , pdu.body_len);
            printf("\nTX : pdu.body[0] = %d" , pdu.body[0]);
            printf("\nTX : pdu.body[1] = %d" , pdu.body[1]);
            
             for(i = 0 ; i < sizeof(txBuf) ; i++)
                    printf("\ntxBuf[%d] = %d " , i , txBuf[i] );
            printf("\n");
        }

    //  Call the below code when Data is Received in RX Buffer
        status_rx_msg = receiveMessage( rxBuf, sizeof(rxBuf), &pdu );
        if(status_rx_msg != 1)
        {
            switch(status_rx_msg)
            {
                case 0:                  printf("RX : Unknown Error");
                                            break;
                case ERR_NOMEM :         printf("RX : Error No Memory");
                                            break;
                case ERR_INVALID_CKSUM : printf("RX : Error Invalid Checksum");
                                            break;
                case ERR_WRONG_DST :     printf("RX : Error Wrong Destination Address");
                default:                    break;

            }
        }
        else
        {
             for(i = 0 ; i < sizeof(rxBuf) ; i++)
                    printf("\nrxBuf[%d] = %d " , i , rxBuf[i] );

            printf("\nRX : pdu.dest_addr = %d" , pdu.dest_addr);
            printf("\nRX : pdu.src_addr = %d" , pdu.src_addr);
            printf("\nRX : pdu.body_len = %d" , pdu.body_len);
            printf("\nRX : pdu.body[0] = %d" , pdu.body[0]);
            printf("\nRX : pdu.body[1] = %d" , pdu.body[1]);
            printf("\n");
        }
    }


    return 0;
}


