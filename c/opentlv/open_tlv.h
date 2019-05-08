#ifndef __OPEN_TLV__
#define __OPEN_TLV__

/***********************************************************
 *************  Common Defines *****************************
 ***********************************************************/
#ifdef PUBLIC
#undef PUBLIC
#endif
#define PUBLIC

#ifdef PRIVATE
#undef PRIVATE
#endif
#define PRIVATE static


/***********************************************************
 *************  TLV Related Defines ************************
 ***********************************************************/


//CANNOT CHANGE THIS
#define OTLV_SUPPORT_MAX_DECIMAL_BITS   7u
#define OTLV_LENGTH_FIELD_LEN           1u

typedef unsigned char obool;

typedef enum{
    OERR_NONE                   = 0x00,
    OERR_INVALID_VALUE_TYPE     = 0x01,
    OERR_INVALID_VALUE_DECIMAL  = 0x02,
    OERR_BUFFER_NOT_ENOUGH      = 0x03,
    OERR_BUFFER_NULL_POINTER    = 0x04,
} OTLV_ERR_E;

typedef enum{
    OVT_BOOL            = 0x00,
    OVT_SIGNED_CHAR     = 0x01,
    OVT_UNSIGNED_CHAR   = 0x02,
    OVT_SIGNED_SHORT    = 0x03,
    OVT_UNSIGNED_SHORT  = 0x04,
    OVT_SIGNED_INT      = 0x05,
    OVT_UNSIGNED_INT    = 0x06,
    OVT_SIGNED_LONG     = 0x07,
    OVT_UNSIGNED_LONG   = 0x08,
    OVT_FLOAT           = 0x09,
    OVT_DOUBLE          = 0x0A,
    OVT_DATE            = 0x0B,     //OTLV_DATE_T
    OVT_BYTEARRAY       = 0x0C,     //    
} OTLV_VALUE_TYPE_E;

typedef enum{
    OVD_DECIMAL_0       = 0,
    OVD_DECIMAL_1       = 1,
    OVD_DECIMAL_2       = 2,
    OVD_DECIMAL_3       = 3,
    OVD_DECIMAL_4       = 4,
    OVD_DECIMAL_5       = 5,
    OVD_DECIMAL_6       = 6,
    OVD_DECIMAL_7       = 7
} OTLV_VALUE_DECIMAL_E;

typedef struct{
    unsigned char year;     //20xx - 2000
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
}OTLV_DATE_T;


typedef struct{
    unsigned char len;
    unsigned char * data;
}OTLV_BYTEARRAY_T;


typedef struct{
    unsigned int    offset;
    unsigned int    max_len;
    unsigned char*  buff;
}OTLV_ENCODE_BUFFER_T;


/************************************************************************************************************************************************
 ****************************                           PUBLIC METHOD AREA                      *************************************************
 ************************************************************************************************************************************************/
PUBLIC unsigned char otlv_util_crc8(unsigned char *p, unsigned int len);
    
PUBLIC void otlv_encode_init(OTLV_ENCODE_BUFFER_T* pencode, unsigned char* pbuff, unsigned int buff_len);
PUBLIC OTLV_ERR_E otlv_encode(unsigned char * tag, unsigned char tag_len, unsigned char * value, unsigned char value_len, 
                              OTLV_VALUE_TYPE_E value_type, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_bool(unsigned char * tag, unsigned char tag_len, obool b, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_signed_char(unsigned char * tag, unsigned char tag_len, signed char value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_unsigned_char(unsigned char * tag, unsigned char tag_len, unsigned char value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_signed_short(unsigned char * tag, unsigned char tag_len, signed short value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_unsigned_short(unsigned char * tag, unsigned char tag_len, unsigned short value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff) ;
PUBLIC OTLV_ERR_E otlv_encode_signed_int(unsigned char * tag, unsigned char tag_len, signed int value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_unsigned_int(unsigned char * tag, unsigned char tag_len, unsigned int value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_signed_long(unsigned char * tag, unsigned char tag_len, signed long value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_unsigned_long(unsigned char * tag, unsigned char tag_len, unsigned long value, OTLV_VALUE_DECIMAL_E decimal, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_float(unsigned char * tag, unsigned char tag_len, float value, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_double(unsigned char * tag, unsigned char tag_len, double value, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_date(unsigned char * tag, unsigned char tag_len, OTLV_DATE_T value, OTLV_ENCODE_BUFFER_T * ebuff);
PUBLIC OTLV_ERR_E otlv_encode_bytearray(unsigned char * tag, unsigned char tag_len, unsigned char * value,  unsigned char value_len, OTLV_ENCODE_BUFFER_T * ebuff);

#endif /*__OPEN_TLV__*/
