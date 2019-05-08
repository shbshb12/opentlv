#ifndef __YARLUNG_TLV__
#define __YARLUNG_TLV__

#define OTLV_FRAME_HEAD                         0xAA
#define OTLV_V1_FRAME_HEAD_HALF_FIXED           0xFE
#define OTLV_V2_FRAME_HEAD_JUST_CHECKSUM        0xFD

#define FRAME_PRO_VERSION                       0x02

#define OTLV_V1_FRAME_ALL_HEAD_LENGTH           18u
#define OTLV_V1_FRAME_ALL_TAIL_LENGTH           3u

#define OTLV_V1_FRAME_MIN_LENGTH                (OTLV_V1_FRAME_ALL_HEAD_LENGTH + OTLV_V1_FRAME_ALL_TAIL_LENGTH)




PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_header(OTLV_ENCODE_BUFFER_T *ebuff, unsigned int dtu_id, OTLV_DATE_T date, 
                            unsigned char project_code, unsigned char supplier_code, unsigned char cmd, unsigned short frame_length);
PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_header_quick(OTLV_ENCODE_BUFFER_T *ebuff, unsigned char cmd, unsigned short frame_length);
PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_body(OTLV_ENCODE_BUFFER_T *ebuff, unsigned char cmd, unsigned char * data, unsigned short data_len);
PUBLIC OTLV_ERR_E ytlv_v1_frame_encode_tail(OTLV_ENCODE_BUFFER_T *ebuff) ;
PUBLIC unsigned int ytlv_v1_frame_get_total_length(unsigned int data_len);

#endif /*__YARLUNG_TLV__*/
