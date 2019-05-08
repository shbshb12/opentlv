package com.yarlungsoft.parser.fzphouse;

import com.yarlungsoft.parser.exception.FrameFormatException;
import com.yarlungsoft.parser.util.CRC8;


public class PhouseFrameAccess {
	
	private static String PHOUSE_TOPIC_PREFIX = "dtu/up";
	
	
	public static void setPrefixTopic(String topic){
		if(topic != null && !topic.isEmpty()){
			PHOUSE_TOPIC_PREFIX = topic;
		}
	}
	
	private boolean isMatchedTopic(String topic){
		if(topic == null || topic.isEmpty())
			return false;
		
		if(topic.startsWith(PHOUSE_TOPIC_PREFIX) ){
			return true;
		}
		return false;
	}

	
	private void validate(String topic, byte[] payload) throws FrameFormatException{
		int length = 0;
		
		if(!isMatchedTopic(topic)){
			throw new FrameFormatException("Un-Matched Topic");
		}

		if(payload == null){
			throw new FrameFormatException("payload is null");
		}
		
		length = payload.length;
		if(length < FrameBean.FRAME_MIN_LEN){
			throw new FrameFormatException("Frame Length is invalid");
		}
		
		if( (payload[0] != (byte)FrameBean.FRAME_TAG_HEAD_1) || 
			(payload[1] != (byte)FrameBean.FRAME_TAG_HEAD_2) ||
			(payload[length - 2] != (byte)FrameBean.FRAME_TAG_TAIL_1) ||
			(payload[length - 1] != (byte)FrameBean.FRAME_TAG_TAIL_2)
		){
			throw new FrameFormatException("Frame Tag is invalid");
		}
		
		byte crc8 = CRC8.calcCrc8(payload, 0, length - 3);
		if(crc8 != payload[length - 3]){
			throw new FrameFormatException("Frame CRC8 is invalid");
		}		
		
		int frame_len = ((payload[3] & 0xff) << 8) | (payload[4] & 0xff);
		if(length != frame_len){
			throw new FrameFormatException("Frame Length is Unmatched with its actual length");
		}
		
		return;
	}
	
	
	public FrameBean parse(String topic, byte[] payload){
		byte cmd;
		
		try {
			validate(topic, payload);
		} catch (FrameFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		
		cmd = payload[11];
		
		switch((byte)cmd){
			case FrameBean.CMD_UP_TLV_DATA:				return new Cmd01FrameBean(payload);
			case FrameBean.CMD_UP_SYNC_TIMESTAMP:		return new Cmd02FrameBean(payload);
			case FrameBean.CMD_UP_UPLOAD_CARDINFO:		return new Cmd03FrameBean(payload);
			case FrameBean.CMD_UP_HEARTBEAT:			return new Cmd04FrameBean(payload);
			case FrameBean.CMD_UP_UPLOAD_CARDAUTH:		return new Cmd05FrameBean(payload);
		}
		
		return null;
	} 
	
}
