package com.yarlungsoft.parser.fzphouse;

import java.util.Calendar;
import java.util.HashMap;

import com.google.gson.Gson;

public abstract class FrameBean implements Parser{
	public  final static byte	CMD_UP_TLV_DATA			= 0x01;
	public  final static byte	CMD_UP_SYNC_TIMESTAMP	= 0x02;
	public  final static byte	CMD_UP_UPLOAD_CARDINFO	= 0x03;
	public  final static byte	CMD_UP_HEARTBEAT		= 0x04;
	public  final static byte	CMD_UP_UPLOAD_CARDAUTH	= 0x05;
	
	
	
	protected final static byte FRAME_HEAD_LEN	= 18;
	protected final static byte FRAME_TAIL_LEN	= 3;	
	protected final static byte FRAME_MIN_LEN	= FRAME_HEAD_LEN + FRAME_TAIL_LEN;
	
	protected final static byte FRAME_TAG_HEAD_1	= (byte)0xAA;
	protected final static byte FRAME_TAG_HEAD_2	= (byte)0xFE;
	protected final static byte FRAME_TAG_TAIL_1	= (byte)0xDD;
	protected final static byte FRAME_TAG_TAIL_2	= (byte)0xEE;	
	protected final static byte FRAME_VERSION		= (byte)0x02;
	
	protected int	mDtuip;				//dtu ip 
	protected int	mFrameLen;			//frame length, from head to tail
	protected byte	mProjectCode;		//project code
	protected byte	mSupplyCode;		//supplier code
	protected byte	mCmd;				//Cmd code
	protected byte	mCrc8;				//CRC8 check
	
	protected Calendar mUploadDataTime;
	
	protected byte	mFrameData[];		//Frame data, head and tail inclued	
	protected byte	mBodyData[];		//Frame body data
	
	
	protected HashMap<String, Object> mKv;
	
	public FrameBean(byte[] payload){
		mKv = new HashMap<String, Object>();
		
		mFrameData = new byte[payload.length];
		for(int i =0; i< payload.length; i++){
			mFrameData[i] = payload[i];
		}
		
		parseFixed();
	}
	
	protected void parseFixed(){
		mFrameLen 		= ((mFrameData[3] & 0xff) << 8) | (mFrameData[4] & 0xff);
		mProjectCode 	= mFrameData[5];
		mSupplyCode  	= mFrameData[6];
		mDtuip			= ((mFrameData[7] & 0xff) << 24) | 
					  	  ((mFrameData[8] & 0xff) << 16) | 
					  	  ((mFrameData[9] & 0xff) <<  8) | 
					  	  ((mFrameData[10]& 0xff) <<  0);		
		mCmd		 	= mFrameData[11];
		
		int year	= mFrameData[12] + 2000;
		int mon 	= mFrameData[13] - 1;
		int day 	= mFrameData[14];
		int hour 	= mFrameData[15];
		int min 	= mFrameData[16];
		int sec 	= mFrameData[17];
		
		mUploadDataTime = Calendar.getInstance();
		mUploadDataTime.set(year, mon, day, hour, min, sec);
		
		int body_len = mFrameLen - FRAME_MIN_LEN;
		mBodyData = new byte[body_len];
		System.arraycopy(mFrameData, FRAME_HEAD_LEN, mBodyData, 0, body_len);	
		
		mKv.put("ip", 		mDtuip);
		mKv.put("ipstr", 	formatIpstr(mDtuip));
		mKv.put("project", 	mProjectCode);
		mKv.put("supplier", mSupplyCode);
		mKv.put("cmd", 		mCmd);
		mKv.put("readtime_ms", mUploadDataTime.getTimeInMillis());			//设备读数据时间
		mKv.put("readtime_str", mUploadDataTime.getTime().toString());
		
		mKv.put("recvtime_ms", Calendar.getInstance().getTimeInMillis());	//网络端接收时间
	}
	
	public String toJson(){
		return new Gson().toJson(mKv);
	}
	
	private String formatIpstr(int ip){
		return "" + ((ip >> 24) & 0xff) + "." + ((ip >> 16) & 0xff) + "." + ((ip >> 8) & 0xff) + "." + ((ip >> 0) & 0xff); 
	}
}
 