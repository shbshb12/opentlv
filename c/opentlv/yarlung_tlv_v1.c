#include "open_tlv.h"
#include "yarlung_tlv.h"
#include "string.h"
#include "assert.h"

/***********************************************************************************************
 *** 
 *** YARLUNG V1 FRAME STRUCTURE :
 ***
 ********************************************
 ***
 *** HEAD (18Bytes): 
 ***    2Byte: 0xAA 0xFE
 ***    1Byte: Protocol Version
 ***    2Byte: Prame Len- HighByte LowByte
 ***    1Byte: Project Code
 ***    1Byte: Supplier Code
 ***    4Byte: Dtu ID/IP (BIG Endian)
 ***    1Byte: Command Code
 ***    6Byte: year mon ay hour min sec
 ***
 *** Body (N Bytes)
 ***    TLV, TLV, TLV, ... 
 ***
 *** Tail (3 Bytes)
 ***    1Byte: CRC8 (from head(0xAA 0xFE) to last of Body: X^8 + X^5 + X^4 + 1)
 ***    2Byte: 0xDD 0xEE
 ***
 ***********************************************************************************************/
 
 

/********************************************************************************
 *****************Private Porting APIs ******************************************
 ********************************************************************************/
PRIVATE unsigned int porting_get_dtu_id(void){
    return  0;
}

PRIVATE unsigned char porting_get_project_code(void){
    return  0;
}

PRIVATE unsigned char porting_get_supplier_code(void){
    return  0;
}

PRIVATE OTLV_DATE_T porting_get_date(void){
    OTLV_DATE_T date;
    return  date;
}



/********************************************************************************
 *****************       PUBLIC  APIs  ******************************************
 ********************************************************************************/
PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_header(OTLV_ENCODE_BUFFER_T *ebuff, unsigned int dtu_id, OTLV_DATE_T date, 
                            unsigned char project_code, unsigned char supplier_code, unsigned char cmd, unsigned short frame_length) {
                                
    if((ebuff == (void*)0) || (ebuff->buff == (void*)0)) {
        return OERR_BUFFER_NULL_POINTER;
    }    
    
    //Can not clear the user buffer
    //ebuff->offset = 0;
	//memset(&ebuff->buff[0], 0 ,ebuff->max_len);

    if(ebuff->offset + frame_length > ebuff->max_len){
        return OERR_BUFFER_NOT_ENOUGH;
    }
    
	ebuff->buff[ebuff->offset+0] = OTLV_FRAME_HEAD;
	ebuff->buff[ebuff->offset+1] = OTLV_V1_FRAME_HEAD_HALF_FIXED;
	ebuff->buff[ebuff->offset+2] = FRAME_PRO_VERSION;
    
    ebuff->buff[ebuff->offset+3] = (frame_length & 0xff00) >> 8;
    ebuff->buff[ebuff->offset+4] = frame_length & 0xff;
    
    ebuff->buff[ebuff->offset+5] = project_code;    
    ebuff->buff[ebuff->offset+6] = supplier_code;
    
    ebuff->buff[ebuff->offset+7]  = (dtu_id >> 24 ) & 0xff ;
    ebuff->buff[ebuff->offset+8]  = (dtu_id >> 16 ) & 0xff ;
    ebuff->buff[ebuff->offset+9]  = (dtu_id >>  8 ) & 0xff ;
    ebuff->buff[ebuff->offset+10] = (dtu_id >>  0 ) & 0xff ;
    
    ebuff->buff[ebuff->offset+11] = cmd;

    ebuff->buff[ebuff->offset+12] = date.year;
    ebuff->buff[ebuff->offset+13] = date.mon;
    ebuff->buff[ebuff->offset+14] = date.day;
    ebuff->buff[ebuff->offset+15] = date.hour;
    ebuff->buff[ebuff->offset+16] = date.min;
    ebuff->buff[ebuff->offset+17] = date.sec;
    
    ebuff->offset = ebuff->offset + 18;
    
	return OERR_NONE;
}

PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_header_quick(OTLV_ENCODE_BUFFER_T *ebuff, unsigned char cmd, unsigned short frame_length) {
    unsigned int dtu_id = porting_get_dtu_id();    
    unsigned char project_code = porting_get_project_code();
    unsigned char supplier_code = porting_get_supplier_code();
    OTLV_DATE_T date = porting_get_date();
    
    return ytlv_v1_frame_encode_header(ebuff, dtu_id, date, project_code, supplier_code, cmd, frame_length);
}


PUBLIC unsigned int ytlv_v1_frame_get_total_length(unsigned int data_len){
    return data_len + OTLV_V1_FRAME_ALL_HEAD_LENGTH + OTLV_V1_FRAME_ALL_TAIL_LENGTH;    
}



PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_body(OTLV_ENCODE_BUFFER_T *ebuff, unsigned char cmd, unsigned char * data, unsigned short data_len){
    
    OTLV_ERR_E ret ;
    unsigned char offset = 0;
    unsigned short frame_len  = ytlv_v1_frame_get_total_length(data_len);
	
    if((ebuff == (void*)0) || (ebuff->buff == (void*)0)) {
        return OERR_BUFFER_NULL_POINTER;
    }
    
    ret = ytlv_v1_frame_encode_header_quick(ebuff, cmd, frame_len);
    if(ret != OERR_NONE) {
        return ret;
    }
    
    if(ebuff->offset + data_len > ebuff->max_len){
        return OERR_BUFFER_NOT_ENOUGH;
    }
    
    offset = ebuff->offset;
    
	memcpy(&ebuff->buff[ebuff->offset], &data[0], data_len);
    
    ebuff->offset += data_len;
    
    ebuff->buff[ebuff->offset] = otlv_util_crc8(&ebuff->buff[offset], ebuff->offset);
    ebuff->offset++;
    
    ebuff->buff[ebuff->offset++] = 0xDD;
    ebuff->buff[ebuff->offset++] = 0xEE;
	
	return OERR_NONE;
}

PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_tail(OTLV_ENCODE_BUFFER_T *ebuff) {
    if((ebuff == (void*)0) || (ebuff->buff == (void*)0)) {
        return OERR_BUFFER_NULL_POINTER;
    }
    
    if(ebuff->offset + OTLV_V1_FRAME_ALL_TAIL_LENGTH > ebuff->max_len){
        return OERR_BUFFER_NOT_ENOUGH;
    }
    
    ebuff->buff[ebuff->offset] = otlv_util_crc8(&ebuff->buff[ebuff->offset], ebuff->offset);
    ebuff->offset++;
    
    ebuff->buff[ebuff->offset++] = 0xDD;
    ebuff->buff[ebuff->offset++] = 0xEE;
	
	return OERR_NONE;
}
