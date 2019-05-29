/*
 * DiverseCloudLIB_TLV.c
 *
 *  Created on: 2019年5月28日
 *      Author: shb
 *     version: 1.1
 */

#include "diversecloudlib_tlv.h"
#include "time.h"
#include "string.h"
/*TLV帧属性_规范版本号*****************/
#define TLV_PROP_SPECVERSION		(2)//默认为2

/*v2 define begin****************************************************************/
#if(TLV_PROP_SPECVERSION == 2)	
/*TLV帧属性_特征帧定义*************/
#define TLV_PROP_HEAD1			(0XAA)//帧头1
#define TLV_PROP_HEAD2_TLV10	(0XFE)//帧头2 TLV v1.0版本
//#define TLV_PROP_HEAD2_JSON	(0XFD)//帧头2 TLV JSON版本(目前不支持)
#define TLV_PROP_TAIL1			(0XDD)//帧尾1
#define TLV_PROP_TAIL2			(0XEE)//帧尾2

/*TLV帧属性_行业定义***************/
#define TLV_PROP_CLASSID_DEFAULT		(0x00)//行业-默认值
//#define TLV_PROP_CLASSID_ELECTRIC	(0x10)//电力
//#define TLV_PROP_CLASSID_WATER		(0x20)//水务-通用
//#define TLV_PROP_CLASSID_WATER_PUMP	(0x21)//水务-泵房
//#define TLV_PROP_CLASSID_WATER_WM	(0x22)//水务-水表
//#define TLV_PROP_CLASSID_CITY		(0x30)//智慧城市
//#define TLV_PROP_CLASSID_ENV			(0x40)//环境检测
//#define TLV_PROP_CLASSID_FACTORY		(0x50)//智慧工厂

/*TLV帧属性_行业定义***************/
#define TLV_PROP_ADDRESSTYPEID_YL		(0)//标识地址类型：雅鲁标准
#define TLV_PROP_ADDRESSTYPEID_CUSTOMER	(100)//标识地址类型：客户自定义
/*TLV帧属性_控制码定义*************/
#define TLV_PROP_CMD_DEFAULT			(0)//控制码-默认值
//#define TLV_PROP_CMD_1				(1)//控制码1
//#define TLV_PROP_CMD_2				(2)//控制码2
//#define TLV_PROP_CMD_3				(3)//控制码3
#endif
/*v2 define end****************************************************************/

/****************声明****************/
static uint8_t tlv_prop_crc8(uint8_t *buff, uint32_t len);


/*************************************
*消息初始化
*prop		属性结构体指针
*return		错误码，参考TLV_ERRORCODE定义
*************************************/
TLV_ERRORCODE tlv_msg_init(TLV_PROP_DEF *prop)
{
	if (prop->AddressType == TLV_PROP_ADDRESSTYPE_YL)
	{
		if (prop->BuffSize < 25)
			return TLV_ISFCBUFFSIZE;//缓存不足以封装全帧
	}
	else if (prop->AddressType == TLV_PROP_ADDRESSTYPE_CUSTOMER)
	{
		if (prop->BuffSize < (22 + prop->CustomerAddress.Len))
			return TLV_ISFCBUFFSIZE;//缓存不足以封装全帧
	}
	else
		return TLV_ERRORADDRESSTYPE;

	prop->Length = 0;

	*(prop->Buff + prop->Length++) = TLV_PROP_HEAD1;
	*(prop->Buff + prop->Length++) = TLV_PROP_HEAD2_TLV10;
	*(prop->Buff + prop->Length++) = TLV_PROP_SPECVERSION;
	*(prop->Buff + prop->Length++) = 0x00;
	*(prop->Buff + prop->Length++) = 0x00;
	*(prop->Buff + prop->Length++) = TLV_PROP_CLASSID_DEFAULT;
	if (prop->AddressType == TLV_PROP_ADDRESSTYPE_YL)
	{
		*(prop->Buff + prop->Length++) = TLV_PROP_ADDRESSTYPEID_YL;
		
		*(prop->Buff + prop->Length++) = prop->YLAddress.Addr4;
		*(prop->Buff + prop->Length++) = prop->YLAddress.Addr3;
		*(prop->Buff + prop->Length++) = prop->YLAddress.Addr2;
		*(prop->Buff + prop->Length++) = prop->YLAddress.Addr1;
	}
	else if (prop->AddressType == TLV_PROP_ADDRESSTYPE_CUSTOMER)
	{
		*(prop->Buff + prop->Length++) = TLV_PROP_ADDRESSTYPEID_CUSTOMER;

		*(prop->Buff + prop->Length++) = prop->CustomerAddress.Len;
		memcpy((prop->Buff + prop->Length), prop->CustomerAddress.Addr, prop->CustomerAddress.Len);
		prop->Length += prop->CustomerAddress.Len;
	}
	*(prop->Buff + prop->Length++) = TLV_PROP_CMD_DEFAULT;
	*(prop->Buff + prop->Length++) = prop->DateTime.DateY;
	*(prop->Buff + prop->Length++) = prop->DateTime.DateM;
	*(prop->Buff + prop->Length++) = prop->DateTime.DateD;
	*(prop->Buff + prop->Length++) = prop->DateTime.TimeH;
	*(prop->Buff + prop->Length++) = prop->DateTime.TimeM;
	*(prop->Buff + prop->Length++) = prop->DateTime.TimeS; 
	return TLV_SUCCESS;
}
/*************************************
 *添加消息数据
 *prop		属性结构体指针
 *msg		消息结构体指针
 *return	错误码	
*************************************/
TLV_ERRORCODE tlv_msg_Add(TLV_PROP_DEF *prop, TLV_MSG_DEF *msg)
{
	sint64_t  msgvallong;
	sint32_t msgval;
	uint8_t decimal, type;	
	static const uint32_t ampfactor[7] = { 1,10,100,1000,10000,100000,1000000 };
	type = msg->Type & 0xF0;
	decimal = msg->Type & 0X07;
	switch (type)
	{
	case TLV_MSGPROP_TYPE_BOOL:
	case TLV_MSGPROP_TYPE_UCHAR:
	case TLV_MSGPROP_TYPE_SCHAR:
		if (prop->BuffSize < prop->Length + 8)
			return TLV_ISFCBUFFSIZE;
		break;
	case TLV_MSGPROP_TYPE_USHORT:
	case TLV_MSGPROP_TYPE_SSHORT:
		if (prop->BuffSize < prop->Length + 9)
			return TLV_ISFCBUFFSIZE;
		break;
	case TLV_MSGPROP_TYPE_SINT:
	case TLV_MSGPROP_TYPE_UINT:
		if (prop->BuffSize < prop->Length + 11)
			return TLV_ISFCBUFFSIZE;
		break;
	case TLV_MSGPROP_TYPE_SLONG:
	case TLV_MSGPROP_TYPE_ULONG:
		if (prop->BuffSize < prop->Length + 15)
			return TLV_ISFCBUFFSIZE;
		break;
	default:
		return TLV_ERRORMSGTYPE;
	}

	//根据decimal小数位确定放大倍数
	if (decimal != 0)
	{
		if ((type == TLV_MSGPROP_TYPE_SLONG) | (type == TLV_MSGPROP_TYPE_ULONG))
			msgvallong = (long long)(msg->Value*ampfactor[decimal]);
		else
			msgval = (sint32_t)(msg->Value*ampfactor[decimal]);
	}
	else if (decimal <= 6)
	{
		if ((type == TLV_MSGPROP_TYPE_SLONG) | (type == TLV_MSGPROP_TYPE_ULONG))
			msgvallong = (long long)msg->Value;
		else
			msgval = (sint32_t)msg->Value;
	}	
	else
		return TLV_ERRORMSGTYPE;//禁止小数点后位数大于7
	
	
	*(prop->Buff + prop->Length++) = msg->Port;
	*(prop->Buff + prop->Length++) = msg->Channel;
	*(prop->Buff + prop->Length++) = msg->Tag;
	*(prop->Buff + prop->Length++) = msg->Type;

	switch (type)
	{
	case TLV_MSGPROP_TYPE_BOOL:
		if (msgval != 0)
			*(prop->Buff + prop->Length++) = 1;
		else
			*(prop->Buff + prop->Length++) = 0;
		break;
	case TLV_MSGPROP_TYPE_UCHAR:
		if ((msgval < 0) | (msgval > 255u))
			return TLV_VALOUTOFRANGE;
		*(prop->Buff + prop->Length++) = (uint8_t)msgval;
		break;
	case TLV_MSGPROP_TYPE_SCHAR:
		if ((msgval < -127) | (msgval > 127))
			return TLV_VALOUTOFRANGE;
		*(prop->Buff + prop->Length++) = (uint8_t)msgval;
		break;
	case TLV_MSGPROP_TYPE_USHORT:
		if ((msgval < 0) | (msgval > 65535u))
			return TLV_VALOUTOFRANGE;
		if (prop->BO != TLV_PROP_BYTEORDER_BE)
		{
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = msgval & 0xff;
		}
		else
		{
			*(prop->Buff + prop->Length++) = msgval & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
		}
		break;
	case TLV_MSGPROP_TYPE_SSHORT:
		if ((msgval < -32767) | (msgval > 32767))
			return TLV_VALOUTOFRANGE;
		if (prop->BO != TLV_PROP_BYTEORDER_BE)
		{
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = msgval & 0xff;
		}
		else
		{
			*(prop->Buff + prop->Length++) = msgval & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
		}
		break;
	case TLV_MSGPROP_TYPE_SINT:
		if ((msgval < -2147483647) | (msgval > 2147483647))
			return TLV_VALOUTOFRANGE;
		if (prop->BO != TLV_PROP_BYTEORDER_BE)
		{
			*(prop->Buff + prop->Length++) = (msgval >> 24) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 16) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = msgval & 0xff;
		}
		else
		{
			*(prop->Buff + prop->Length++) = msgval & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 16) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 24) & 0xff;
		}
		break;
	case TLV_MSGPROP_TYPE_UINT:
		if ((msgval < 0) | (msgval > 4294967294u))
			return TLV_VALOUTOFRANGE;
		
		if (prop->BO != TLV_PROP_BYTEORDER_BE)
		{
			*(prop->Buff + prop->Length++) = (msgval >> 24) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 16) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = msgval & 0xff;
		}
		else
		{
			*(prop->Buff + prop->Length++) = msgval & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 16) & 0xff;
			*(prop->Buff + prop->Length++) = (msgval >> 24) & 0xff;
		}
		break;
	case TLV_MSGPROP_TYPE_SLONG:
	case TLV_MSGPROP_TYPE_ULONG:
		if (prop->BO != TLV_PROP_BYTEORDER_BE)
		{
			*(prop->Buff + prop->Length++) = (msgvallong >> 56) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 48) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 40) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 32) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 24) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 16) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = msgvallong & 0xff;
		}
		else
		{
			*(prop->Buff + prop->Length++) = msgvallong & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 8) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 16) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 24) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 32) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 40) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 48) & 0xff;
			*(prop->Buff + prop->Length++) = (msgvallong >> 56) & 0xff;
		}
		break;
	default:
		return TLV_ERRORMSGTYPE;
	}
	return TLV_SUCCESS;
}
/*************************************
*完成消息
*prop		属性结构体指针
*return		错误码，参考TLV_ERRORCODE定义
*************************************/
TLV_ERRORCODE tlv_msg_finish(TLV_PROP_DEF *prop)
{
	*(prop->Buff + 3) = ((prop->Length + 3) >> 8) & 0xff;
	*(prop->Buff + 4) = (prop->Length + 3) & 0xff;
	*(prop->Buff + prop->Length++) = tlv_prop_crc8(prop->Buff, prop->Length);
	*(prop->Buff + prop->Length++) = TLV_PROP_TAIL1;
	*(prop->Buff + prop->Length++) = TLV_PROP_TAIL2;
	return TLV_SUCCESS;
}

/*************************************
*TLV帧雅鲁标识地址设置
*devAddr4	设备标识地址4
*devAddr3	设备标识地址3
*devAddr2	设备标识地址2
*devAddr1	设备标识地址1
*return		TLV帧雅鲁格式化标识地址
*************************************/
TLV_PROP_YLADDRESS_DEF tlv_prop_yladdress_set(uint8_t addr4, uint8_t addr3, uint8_t addr2, uint8_t addr1)
{
	TLV_PROP_YLADDRESS_DEF address;
	address.Addr4 = addr4;
	address.Addr3 = addr3;
	address.Addr2 = addr2;
	address.Addr1 = addr1;
	return address;
}
/*************************************
*TLV帧非雅鲁自定义标识地址设置
*addr		标识地址指针
*len		标识地址长度
*return		TLV帧非雅鲁格式化标识地址
*************************************/
TLV_PROP_CUSTOMERADDRESS_DEF tlv_prop_customeraddress_set(uint8_t *addr, uint8_t len)
{
	TLV_PROP_CUSTOMERADDRESS_DEF address;
	address.Len = len;
	address.Addr = addr;
	return address;
}
/*************************************
*TLV帧数据上报时间（UTC+8）设置
*dateY		年
*dateM		月
*dateD		日
*timeH		时
*timeM		分
*timeS		秒
*return		TLV帧数据上报格式化时间
*************************************/
TLV_PROP_DATETIME_DEF tlv_prop_datetime_set(uint8_t dateY, uint8_t dateM, uint8_t dateD, uint8_t timeH, uint8_t timeM, uint8_t timeS)
{
	TLV_PROP_DATETIME_DEF dateTime;
	dateTime.DateY = dateY;
	dateTime.DateM = dateM;
	dateTime.DateD = dateD;
	dateTime.TimeH = timeH;
	dateTime.TimeM = timeM;
	dateTime.TimeS = timeS;
	return dateTime;
}
/*************************************
*TLV帧数据上报时间（UTC+8）设置-时间戳（秒）,不支持ms时间戳转换
*timestamp	long long型 时间戳（秒）
*return		TLV帧数据上报格式化时间
*************************************/
TLV_PROP_DATETIME_DEF tlv_prop_datetime_set_timestamps(const sint64_t timestampS)
{
	TLV_PROP_DATETIME_DEF dateTime;
	struct tm *lt;
	time_t tss = timestampS;
	lt = localtime(&tss);

	dateTime.DateY = lt->tm_year - 100;
	dateTime.DateM = lt->tm_mon + 1;
	dateTime.DateD = lt->tm_mday;
	dateTime.TimeH = lt->tm_hour;
	dateTime.TimeM = lt->tm_min;
	dateTime.TimeS = lt->tm_sec;

	return dateTime;
}
/*************************************
*TLV帧数据上报时间（UTC+8）设置，使用服务器接收时间作为上报时间
*return		TLV帧数据上报格式化时间
*************************************/
TLV_PROP_DATETIME_DEF tlv_prop_datetime_set_serverdatetime(void)
{
	TLV_PROP_DATETIME_DEF dateTime;
	memset(&dateTime, 0, sizeof(dateTime));
	/*dateTime.DateY = 0;
	dateTime.DateM = 0;
	dateTime.DateD = 0;
	dateTime.TimeH = 0;
	dateTime.TimeM = 0;
	dateTime.TimeS = 0;*/
	return dateTime;
}
/*************************************
*平台大小端检测
*return	BYTEORDER_BE	大端格式
		BYTEORDER_LE	小端格式
*************************************/
TLV_PROP_BYTEORDER_DEF tlv_prop_byteorder_check(void)
{
	union BOTMP
	{
		char c;
		int i;
	};
	union BOTMP botmp;
	botmp.i = 1;

	return botmp.c;
}

/***********CRC8校验表*************/
static const uint8_t CRC8_TAB[] = {
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

/*************************************
*CRC8数据校验函数
*通过查表的方式实现X^8 + X^5 + X^4 + 1
*pcrcbuff		输入数组
*crclen			数组长度
*return			CRC结果
*************************************/
static uint8_t tlv_prop_crc8(uint8_t *buff, uint32_t len)
{
	uint32_t i;
	uint8_t crc8 = 0;
	for (i = 0; i<len; i++)
	{
		crc8 = CRC8_TAB[buff[i] ^ crc8];
	}
	return(crc8);//返回校验值
}
