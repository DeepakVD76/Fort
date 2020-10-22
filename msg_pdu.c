#include <stdio.h>
#include <stdlib.h>
#include "msg_pdu.h"

#define Preamble_0 0xAA
#define Preamble_1 0xAA

/*
 * Prepares a PDU for transmssion out of the device's communications port.
 *
 * @param pdu[in] The in-memory representation of the data to send. This
 *                structure has buffer has been populated by the calling
 *                function.
 *
 * @param txBuf[in/out] A serialization buffer that will be passed to the
 *                      communications infrastructure for transmission. This
 *                      buffer should be populated with the full on-the-wire
 *                      representation of the PDU as shown in Table 1 based on
 *                      the data in <pdu>.
 *
 * @param bufSize[in] The size of the buffer pointed to by <txBuf>
 *
 * @return 1 on success, ERRNO for a known error, or 0 for an unknown error
 */
//TODO - Implement this function
int prepareMessage(Msg_PDU const * const pdu, uint8_t* const txBuf, size_t const bufSize)
{
    uint16_t chk_sum;

    uint16_t buffer_position = 0;       // Initialise the Tx Buffer Pointer
    uint8_t body_position = 0;          // Actual Data Body character Position


    if( (pdu->body_len == 0) || (pdu->body_len > MAX_BODY_LEN) )
        return 0;       // unknown error

    *(txBuf + buffer_position++)  =  Preamble_0 ;
    *(txBuf + buffer_position++)  =  Preamble_1 ;

    *(txBuf + buffer_position++)  =  (pdu->src_addr) >> 8 ;    //For Big Endian format the MSByte should get stored first
    *(txBuf + buffer_position++)  =  (uint8_t)(pdu->src_addr);  // type cast integer to unsigned char to hold the lower byte.

    *(txBuf + buffer_position++)  =  (pdu->dest_addr) >> 8 ;    //For Big Endian format the MSByte should get stored first
    *(txBuf + buffer_position++)  =  (uint8_t)(pdu->dest_addr);  // type cast integer to unsigned char to hold the lower byte.

    *(txBuf + buffer_position++)  =  pdu->body_len ;


    for(;buffer_position < bufSize - 2; buffer_position++)    // bufsize without check sum bytes i.e. 2 bytes less
    {
        *(txBuf + buffer_position)  =  *( (pdu->body) + body_position++) ;
    }

    chk_sum = calculate_checksum( txBuf, bufSize - 2);  // without checksum bytes

    *(txBuf + buffer_position++)  =  chk_sum >> 8 ;    //For Big Endian format the MSByte should get stored first
    *(txBuf + buffer_position++)  =  (uint8_t)(chk_sum);  // type cast integer to unsigned char to hold the lower byte.


    return 1;   // Success Exit
}


/*
 * Process a received buffer of data for usage by the rest of the application
 *
 * @param rxBuf[in] The raw buffer content as received by the communications
 *                  subsystem. <rxBuf> is gauranteed by the receiving logic
 *                  to start on a message boundary (i.e. <0xAA><0xAA>), and to
 *                  contain exactly one full message.
 *
 * @param bufSize[in] Size of <rxBuf>
 *
 * @param pdu[in/out] Pointer to an already-allocated buffer where the internal
 *                    representation should be stored.
 *
 * @return 1 on success, ERRNO for a known error, or 0 for an unknown error
 */
//TODO - Implement this function
int receiveMessage(uint8_t const * const rxBuf, size_t const bufSize, Msg_PDU * const pdu)
{
    uint16_t chk_sum , complete_integer;

    uint16_t buffer_position = 2;       // Initialise the Rx Buffer Pointer skipping the initial two preamble bytes
                                        // as it was already cheched while receiving the data.
    uint8_t body_position = 0;          // Actual Data Body character Position


    complete_integer =  *(rxBuf + buffer_position++);
    complete_integer <<= 8;
    complete_integer +=  *(rxBuf + buffer_position++);
    pdu->src_addr = complete_integer ;

    complete_integer =  *(rxBuf + buffer_position++);
    complete_integer <<= 8;
    complete_integer +=  *(rxBuf + buffer_position++);
    pdu->dest_addr = complete_integer ;

    pdu->body_len = *(rxBuf + buffer_position++);

    if( (pdu->body_len > 0) && (pdu->body_len == bufSize - 9) ) // without preamble , src_add , dest_add , body_len & checksum bytes
    {
        for(;buffer_position < bufSize - 2; buffer_position++)    // bufsize without check sum bytes i.e. 2 bytes less
        {
            *( (pdu->body) + body_position++) = *(rxBuf + buffer_position) ;
        }
    }
    else
    {
        if(pdu->body_len == 0)
            return 0;   // Unknown Error
        else if(pdu->body_len > bufSize - 9)
            return ERR_NOMEM;
    }

    chk_sum = *(rxBuf + buffer_position++);
    chk_sum <<= 8;
    chk_sum +=  *(rxBuf + buffer_position++);
    if( chk_sum != calculate_checksum( rxBuf, bufSize - 2))   // without checksum bytes
        return ERR_INVALID_CKSUM;

    if(pdu->dest_addr != get_my_addr() )
        return ERR_WRONG_DST;

    return 1;   // Success Exit
}

/*
One Correction Required in the Header File as follows,

Inline Functions are those functions whose definitions are small and be substituted at the place where its function call is happened
When compiled , GCC Performs inline substitution as the part of optimisation. So there is no function call present.
(In our case for calculate_checksum() and get_my_addr() functions)
Normally GCC's file scope is "not extern linkage". That means inline functions is never ever provided to the linker
which is causing the compiler to  generate the error "undefined reference to calculate_checksum" or "get_my_addr".
To resolve this problem use "static" before inline as i have corrected in the msg_pdu.h file.
Using "static" keyword forces the compiler to consider this inline function in the linker and hence its has corrected the error
and compiled successfully.

*/
