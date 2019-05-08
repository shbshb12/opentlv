package com.yarlungsoft.parser.fzphouse;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

import com.yarlungsoft.parser.exception.FrameFormatException;
import com.yarlungsoft.parser.util.Utility;

public class Cmd01FrameBean extends FrameBean {

	private ArrayList<Object> 		mBody;
	private HashMap<String, Object> mItem;
	private String 					mIptag;
	
	public Cmd01FrameBean(byte[] payload) {
		super(payload);
		parseBody();
	}
	

	@Override
	public void parseBody() {
		mBody  = new ArrayList<Object>();		
		mIptag = Utility.formatIp(Utility.formatIp(this.mDtuip));
		
		try {
			decodeTlv(this.mBodyData, 0);
		} catch (FrameFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		
		mKv.put("body", mBody);
	}
	
	private void decodeTlv(byte[] data, int offset) throws FrameFormatException{
		/*
		 * TLV Format : ADR(1) : channel(1) : Tag(1) : Type(1) : Value(1:4)		 
		 */
		String address;
		String channel;
		String data_tag;		
		String device_tag;
		String full_tag;
		
		byte type;		
		int vtype;
		int vlen;
		int precision;
		boolean isBatch = false;
		
		if(offset < data.length - 4){
			address 	= Utility.byteToHexStr(data[offset]);
			channel 	= Utility.byteToHexStr(data[offset + 1]);
			data_tag 	= Utility.byteToHexStr(data[offset + 2]);
			
			type 		= data[offset + 3];			
			vtype 		= getValueType(type);
			vlen 		= getValueLength(type);
			precision 	= getValuePrecision(type);
			isBatch 	= isBatchbitActived(type);
			if(isBatch){
				//do not handle now!
				throw new FrameFormatException("Not support batch TLV now...");
			}
			
			String value = calcValue(vtype, precision, Arrays.copyOfRange(data, offset + 4, offset + 4 + vlen));			
			
			device_tag 	= mIptag + address + channel;
			full_tag 	= device_tag + data_tag;
			
			mItem  = new HashMap<String, Object>();
			
			mItem.put("address", address);
			mItem.put("channel", channel);
			
			mItem.put("dataTag", 	data_tag);
			mItem.put("deviceTag", 	device_tag);
			mItem.put("fullTag", 	full_tag);
			mItem.put("value", 		value);
			
			mBody.add(mItem);
			
			decodeTlv(data, offset + 4 + vlen);
		}		
	}
	
	
	private String calcValue(int type, int precision, byte[] value){
		switch(type){
		case 0:			//bool
			return (value[0] > 0) ? "1" : "0";
			
		case 1:		//signed char
			return String.valueOf(new BigDecimal(Utility.byteToSignedChar(value[0])).movePointLeft(precision).doubleValue());
			
		case 2:		//unsigned char
			return String.valueOf(new BigDecimal(Utility.byteToUnsignedChar(value[0])).movePointLeft(precision).doubleValue());
			
		case 3:		//signed short
			return String.valueOf(new BigDecimal(Utility.byte2ToSignedShort(value[0], value[1])).movePointLeft(precision).doubleValue());
			
		case 4:		//unsigned short
			return String.valueOf(new BigDecimal(Utility.byte2ToUnsignedShort(value[0], value[1])).movePointLeft(precision).doubleValue());			
			
		case 5:		//signed int
			return String.valueOf(new BigDecimal(Utility.byte4ToSignedInt(value[0], value[1],value[2], value[3])).movePointLeft(precision).doubleValue());			
			
		case 6:		//unsigned int
			return String.valueOf(new BigDecimal(Utility.byte4ToUnsignedInt(value[0], value[1],value[2], value[3])).movePointLeft(precision).doubleValue());
			
		case 7:		//signed long
			return String.valueOf(new BigDecimal(Utility.byte8ToLong(value[0], value[1],value[2], value[3],
					value[4], value[5],value[6], value[7] )).movePointLeft(precision).doubleValue());
						
		case 8:		//unsigned long
			//TODO ,bug, overflow
			return String.valueOf(new BigDecimal(Utility.byte8ToLong(value[0], value[1],value[2], value[3],
					value[4], value[5],value[6], value[7] )).movePointLeft(precision).doubleValue());
			
		case 9:		//float
			return String.valueOf(Float.intBitsToFloat(Utility.byte4ToSignedInt(value[0], value[1],value[2], value[3])));  
			
		case 10:		//double
			return String.valueOf(Double.longBitsToDouble(Utility.byte8ToLong(value[0], value[1],value[2], value[3],
					value[4], value[5],value[6], value[7])));
		}
		
		return "";
	}
	
	
	private int getValueType(byte type){
		return (type & 0xf0) >> 4;
	}
	
	private int getValueLength(byte type) throws FrameFormatException{
		int vlue = (type & 0xf0) >> 4;
		switch(vlue){
		case 0:		return 1;
		case 1:		return 1;
		case 2:		return 1;
		case 3:		return 2;
		case 4:		return 2;
		case 5:		return 4;
		case 6:		return 4;
		case 7:		return 8;
		case 8:		return 8;
		case 9:		return 4;
		case 10:	return 8;
		}
		
		throw new FrameFormatException("Unsupport TLV Value type: " + vlue);
	}
	
	private int getValuePrecision(byte type){
		return type & 0x07;
	}
	
	private boolean isBatchbitActived(byte type){
		return (type & 0x08) != 0x00;
	}
	
}
