package com.yarlungsoft.parser.fzphouse;

import java.util.Arrays;
import java.util.HashMap;

public class Cmd03FrameBean extends FrameBean {
	private int mCardNum;
	private HashMap<String, Integer> mCards;
	private HashMap<String, Object> mBody;
	
	public Cmd03FrameBean(byte[] payload) {
		super(payload);		
		parseBody();
	}
	

	@Override
	public void parseBody() {
		mBody  = new HashMap<String, Object>();
		mCards = new HashMap<String, Integer>();
		
		mCardNum = (int) (this.mBodyData[0] & 0xff);
		if(mCardNum == 0){
			mBody.put("cards_count", 0);
			mBody.put("cards_list",  mCards);		
			mKv.put("body", mBody);			
			return;
		} else {
			if(this.mBodyData.length != mCardNum*12 + 1){
				mBody.put("cards_count", 0);
				mBody.put("cards_list",  mCards);		
				mKv.put("body", mBody);				
				return;
			} else {
				int status ;
				String card;
				
				for(int i=0; i < mCardNum; i++){
					status = this.mBodyData[1 + i*12];
					card = new String(Arrays.copyOfRange(mBodyData, 3+i*12, 3+i*12+10)).trim();
					mCards.put(card, status);									
				}
				
				mBody.put("cards_count", mCardNum);
				mBody.put("cards_list",  mCards);		
				mKv.put("body", mBody);
			}
		}
	}
	
}
