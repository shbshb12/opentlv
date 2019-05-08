package com.yarlungsoft.parser.fzphouse;

import java.util.Arrays;
import java.util.HashMap;

public class Cmd05FrameBean extends FrameBean {
	private HashMap<String, Object> mBody;
	private Integer bAuthRes ;
	private String  card ;
	
	public Cmd05FrameBean(byte[] payload) {
		super(payload);
		parseBody();
	}
	

	@Override
	public void parseBody() {
		mBody    = new HashMap<String, Object>();

		bAuthRes = this.mBodyData[0] > 0 ? 0x01 : 0x00;
		card 	 = new String(Arrays.copyOfRange(mBodyData, 2, 12)).trim();
		
		mBody.put("card", 		card);
		mBody.put("auth_ret", 	bAuthRes);
		
		mKv.put("body", mBody);
	}
	
}
