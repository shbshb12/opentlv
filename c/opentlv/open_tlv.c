#include "open_tlv.h"
#include "string.h"
#include "assert.h"

/* CRC8 table*/
//∂‡œÓ ΩX^8 + X^5 + X^4 + 1
PRIVATE const unsigned char CRC8_TAB[]=	{
			0x00,0x5e,0xbc,0xe2,0x61,0x3f,0xdd,0x83,//0~7
			0xc2,0x9c,0x7e,0x20,0xa3,0xfd,0x1f,0x41,//8~15
			0x9d,0xc3,0x21,0x7f,0xfc,0xa2,0x40,0x1e,//16_23
			0x5f,0x01,0xe3,0xbd,0x3e,0x60,0x82,0xdc,//24~31
			0x23,0x7d,0x9f,0xc1,0x42,0x1c,0xfe,0xa0,//32~39
			0xe1,0xbf,0x5d,0x03,0x80,0xde,0x3c,0x62,//40~47
			0xbe,0xe0,0x02,0x5c,0xdf,0x81,0x63,0x3d,//48~55
			0x7c,0x22,0xc0,0x9e,0x1d,0x43,0xa1,0xff,//56~63
			0x46,0x18,0xfa,0xa4,0x27,0x79,0x9b,0xc5,//64~71
			0x84,0xda,0x38,0x66,0xe5,0xbb,0x59,0x07,//72~79
			0xdb,0x85,0x67,0x39,0xba,0xe4,0x06,0x58,//80~87
			0x19,0x47,0xa5,0xfb,0x78,0x26,0xc4,0x9a,//88~95
			0x65,0x3b,0xd9,0x87,0x04,0x5a,0xb8,0xe6,//96~103
			0xa7,0xf9,0x1b,0x45,0xc6,0x98,0x7a,0x24,//104~111
			0xf8,0xa6,0x44,0x1a,0x99,0xc7,0x25,0x7b,//112~119
			0x3a,0x64,0x86,0xd8,0x5b,0x05,0xe7,0xb9,//120~127
			0x8c,0xd2,0x30,0x6e,0xed,0xb3,0x51,0x0f,//128~135
			0x4e,0x10,0xf2,0xac,0x2f,0x71,0x93,0xcd,//136~143
			0x11,0x4f,0xad,0xf3,0x70,0x2e,0xcc,0x92,//144~151
			0xd3,0x8d,0x6f,0x31,0xb2,0xec,0x0e,0x50,//152~159
			0xaf,0xf1,0x13,0x4d,0xce,0x90,0x72,0x2c,//160~167
			0x6d,0x33,0xd1,0x8f,0x0c,0x52,0xb0,0xee,//168~175
			0x32,0x6c,0x8e,0xd0,0x53,0x0d,0xef,0xb1,//176~183
			0xf0,0xae,0x4c,0x12,0x91,0xcf,0x2d,0x73,//184~191
			0xca,0x94,0x76,0x28,0xab,0xf5,0x17,0x49,//192~199
			0x08,0x56,0xb4,0xea,0x69,0x37,0xd5,0x8b,//200~207
			0x57,0x09,0xeb,0xb5,0x36,0x68,0x8a,0xd4,//208~215
			0x95,0xcb,0x29,0x77,0xf4,0xaa,0x48,0x16,//216~223
			0xe9,0xb7,0x55,0x0b,0x88,0xd6,0x34,0x6a,//224~231
			0x2b,0x75,0x97,0xc9,0x4a,0x14,0xf6,0xa8,//232~239
			0x74,0x2a,0xc8,0x96,0x15,0x4b,0xa9,0xf7,//240~247
			0xb6,0xe8,0x0a,0x54,0xd7,0x89,0x6b,0x35,//248~255
			};

            
PRIVATE void memcpy_reverse(unsigned char * ptr, unsigned char len){
    unsigned char left = 0;  
    unsigned char right = len - 1;  
    unsigned char *p = ptr;  
    unsigned char tmp;
    while (left < right) {  
        tmp = *(p + left);  
        *(p + left) = *(p + right);  
        *(p + right) = tmp;  
          
        left ++;  
        right --;  
    }
}

PUBLIC unsigned char otlv_util_crc8(unsigned char *p, unsigned int len)
{
	unsigned int i;
	unsigned char CRC8=0;

	for(i = 0; i < len; i++)
	   CRC8 = CRC8_TAB[p[i]^CRC8];
    
	return(CRC8);
}

PUBLIC void otlv_encode_init(OTLV_ENCODE_BUFFER_T* pencode, unsigned char* pbuff, unsigned int buff_len){    
    assert(pencode != (void*)0);
    
    memset(pencode, 0, sizeof(OTLV_ENCODE_BUFFER_T));    
    memset(&pbuff[0], 0 ,buff_len);
    
    pencode->buff = pbuff;
    pencode->max_len = buff_len;
}


PUBLIC OTLV_ERR_E otlv_encode(unsigned char * tag, unsigned char tag_len, unsigned char * value, unsigned char value_len, 
                              OTLV_VALUE_TYPE_E value_type, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff)
{
    
    unsigned int length = 0;
    unsigned char l_field = 0;
    
    if((ebuff == (void*)0) || (ebuff->buff == (void*)0)) {
        return OERR_BUFFER_NULL_POINTER;
    }
    
    if(OVT_BYTEARRAY == value_type) {
        length = tag_len + value_len + OTLV_LENGTH_FIELD_LEN + 1;
    } else {
        length = tag_len + value_len + OTLV_LENGTH_FIELD_LEN;
    }
    
    if(ebuff->offset + length > ebuff->max_len) {
        return OERR_BUFFER_NOT_ENOUGH;
    }
    
    if(value_type > OVT_BYTEARRAY) {
        return OERR_INVALID_VALUE_TYPE;
    }
    
    if(decimal > OVD_DECIMAL_7) {
        return OERR_INVALID_VALUE_DECIMAL;
    }

    l_field = (value_type << 4) | decimal;
    
    memcpy((void*)&ebuff->buff[ebuff->offset], (void*)tag, tag_len);
    ebuff->offset += tag_len;
    
    memcpy(&ebuff[ebuff->offset], &l_field, 1);
    ebuff->offset += 1;
    
    if(OVT_BYTEARRAY == value_type) {
        memcpy(&ebuff[ebuff->offset], &value_len, 1);
        ebuff->offset += 1;
    }
    
    memcpy(&ebuff[ebuff->offset], value, value_len);
    ebuff->offset += value_len;
    
    return OERR_NONE;
}

PUBLIC OTLV_ERR_E otlv_encode_bool(unsigned char * tag, unsigned char tag_len, obool b, OTLV_ENCODE_BUFFER_T * ebuff) {
    return otlv_encode(tag, tag_len, (unsigned char *)&b, 1, OVT_BOOL, OVD_DECIMAL_0, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_signed_char(unsigned char * tag, unsigned char tag_len, signed char value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    return otlv_encode(tag, tag_len, (unsigned char *)&value, 1, OVT_SIGNED_CHAR, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_unsigned_char(unsigned char * tag, unsigned char tag_len, unsigned char value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    return otlv_encode(tag, tag_len, (unsigned char *)&value, 1, OVT_UNSIGNED_CHAR, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_signed_short(unsigned char * tag, unsigned char tag_len, signed short value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {    
    unsigned char len = sizeof(signed short);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_SIGNED_SHORT, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_unsigned_short(unsigned char * tag, unsigned char tag_len, unsigned short value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(unsigned short);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_UNSIGNED_SHORT, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_signed_int(unsigned char * tag, unsigned char tag_len, signed int value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(signed int);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_SIGNED_INT, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_unsigned_int(unsigned char * tag, unsigned char tag_len, unsigned int value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(unsigned int);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_UNSIGNED_INT, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_signed_long(unsigned char * tag, unsigned char tag_len, signed long value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(signed long);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_SIGNED_LONG, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_unsigned_long(unsigned char * tag, unsigned char tag_len, unsigned long value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(unsigned long);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_UNSIGNED_LONG, decimal, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_float(unsigned char * tag, unsigned char tag_len, float value, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(float);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_FLOAT, OVD_DECIMAL_0, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_double(unsigned char * tag, unsigned char tag_len, double value, OTLV_ENCODE_BUFFER_T * ebuff) {
    unsigned char len = sizeof(double);
    memcpy_reverse((unsigned char*)&value, len);
    return otlv_encode(tag, tag_len, (unsigned char *)&value, len, OVT_DOUBLE, OVD_DECIMAL_0, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_date(unsigned char * tag, unsigned char tag_len, OTLV_DATE_T value, OTLV_ENCODE_BUFFER_T * ebuff) {    
    return otlv_encode(tag, tag_len, (unsigned char *)&value, sizeof(OTLV_DATE_T), OVT_DATE, OVD_DECIMAL_0, ebuff);
}

PUBLIC OTLV_ERR_E otlv_encode_bytearray(unsigned char * tag, unsigned char tag_len, unsigned char * value,  unsigned char value_len, OTLV_ENCODE_BUFFER_T * ebuff) {    
    return otlv_encode(tag, tag_len, (unsigned char *)&value, value_len, OVT_BYTEARRAY, OVD_DECIMAL_0, ebuff);
}
