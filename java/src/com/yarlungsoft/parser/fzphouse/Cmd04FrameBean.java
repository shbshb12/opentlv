package com.yarlungsoft.parser.fzphouse;

import java.util.HashMap;

public class Cmd04FrameBean extends FrameBean {
	private HashMap<String, Object> mBody;
	
	public Cmd04FrameBean(byte[] payload) {
		super(payload);
		parseBody();
	}
	

	@Override
	public void parseBody() {
		mBody  = new HashMap<String, Object>();
		mKv.put("body", mBody);
	}
	
}
