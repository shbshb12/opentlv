package com.yarlungsoft.parser.fzphouse;

import java.util.HashMap;

public class Cmd02FrameBean extends FrameBean {

	private HashMap<String, Object> mBody;
	
	public Cmd02FrameBean(byte[] payload) {
		super(payload);
		parseBody();
	}
	

	@Override
	public void parseBody() {
		mBody  = new HashMap<String, Object>();
		mKv.put("body", mBody);
	}
	
}
