package com.yarlungsoft.parser.util;

import java.math.BigDecimal;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * 
 * @author t.x <br>
 *         2018/03/28</br>
 * 
 */
public class Utility {
		
	// these byte datas are big endiean.
	public static int byte2ToInt(byte val1, byte val2) {
		return ((val1 & 0xff) << 8) | (val2 & 0xff);
	}
	
	public static byte byteToSignedChar(byte value){		
		return (byte) (value);			
	}
	
	public static short byteToUnsignedChar(byte value){		
		return (short) ((value  & 0xff));
	}
	
	public static short byte2ToSignedShort(byte val1, byte val2){		
		return (short) (((val1 & 0xff) << 8 ) | ((val2  & 0xff)));			
	}
	
	public static int byte2ToUnsignedShort(byte val1, byte val2){		
		return ((val1 & 0xff) << 8) | ((val2  & 0xff));
		}
	
	public static int byte4ToSignedInt(byte val1, byte val2, byte val3, byte val4) {
		return ((val1 & 0xff) << 24) | ((val2 & 0xff) << 16)
				| ((val3 & 0xff) << 8) | (val4 & 0xff);
	}
	
	public static long byte4ToUnsignedInt(byte val1, byte val2, byte val3, byte val4) {
		return ((val1 & 0xffL) << 24) | ((val2 & 0xff) << 16)
				| ((val3 & 0xff) << 8) | (val4 & 0xff);
	}

	public static long byte8ToLong(byte val1, byte val2, byte val3, byte val4,
			byte val5, byte val6, byte val7, byte val8) {
		return ((val1 & (long) 0xff) << 56)
				| ((val2 & (long) 0xff) << 48) | ((val3 & (long) 0xff) << 40)
				| ((val4 & (long) 0xff) << 32) | ((val5 & (long) 0xff) << 24)
				| ((val6 & (long) 0xff) << 16) | ((val7 & (long) 0xff) << 8) | (val8 & (long) 0xff);
	}
	
	// big endiean
	public static String formatIp(byte val1, byte val2, byte val3, byte val4) {
		return "" + (val1 & 0xff) + "." + (val2 & 0xff) + "." + (val3 & 0xff)
				+ "." + (val4 & 0xff);
	}

	public static String formatIp(int ip) {
		return "" + ((ip >> 24) & 0xff) + "." + ((ip >> 16) & 0xff) + "."
				+ ((ip >> 8) & 0xff) + "." + ((ip >> 0) & 0xff);
	}

	public static boolean checkAddrIsIp(String addr) {
		if (addr.length() < 7 || addr.length() > 15 || "".equals(addr)) {
			return false;
		}
		String rexp = "([1-9]|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])(\\.(\\d|[1-9]\\d|1\\d{2}|2[0-4]\\d|25[0-5])){3}";
		Pattern pat = Pattern.compile(rexp);
		Matcher mat = pat.matcher(addr);
		return mat.find();
	}

	public static String formatIp(String ip) {
		String[] ips = ip.split("\\.");
		String res = "";
		for (String i : ips) {
			int m = Integer.parseInt(i);
			String n = Integer.toHexString(m);
			if (n.length() == 1) {
				n = "0" + n;
			}
			res += n;
		}
		return res.toUpperCase();
	}

	// because the year is not complete.
	public static String formatBcdDate(byte[] data) {
		return "20" + parseBytesToHexStr(data);
	}
	
	public static String formateHexDate(byte[] data){
		if(data.length != 6){
			return null;
		}
		String date = "20";
		for(int i = 0; i< 6; i++){
			String d = String.valueOf(data[i]&0xFF);
			if(d.length() == 1){
				date += "0"+d;
			}else if(d.length() == 3){
				date += "00";
			}else{
				date += d;
			}
		}
		return date;
	}

	public static String parseBytesToHexStr(byte[] buf) {
		if (null == buf) {
			return null;
		}
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < buf.length; i++) {
			String hex = Integer.toHexString(buf[i] & 0xFF);
			if (hex.length() == 1) {
				hex = '0' + hex;
			}
			sb.append(hex.toUpperCase());
		}
		return sb.toString();
	}

	public static byte[] hexStringToBytes(String hexString) {
		if (hexString == null || hexString.equals("")) {
			return null;
		}
		hexString = hexString.toUpperCase();
		int length = hexString.length() / 2;
		char[] hexChars = hexString.toCharArray();
		byte[] d = new byte[length];
		for (int i = 0; i < length; i++) {
			int pos = i * 2;
			d[i] = (byte) (charToByte(hexChars[pos]) << 4 | charToByte(hexChars[pos + 1]));
		}
		return d;
	}

	private static byte charToByte(char c) {
		return (byte) "0123456789ABCDEF".indexOf(c);
	}

	public static String byteToHexStr(byte val) {
		String hex = Integer.toHexString(val & 0xFF);
		if (hex.length() == 1) {
			hex = '0' + hex;
		}
		return hex.toUpperCase();
	}

	/**
	 * @author TX <br>
	 *         2018/01/24<br>
	 * @param payload
	 * @return true if CRC is right.
	 */
	public static boolean checkCRC8(byte[] payload) {
		int len = payload.length;
		// byte[] temp = new byte[len - 4];//no head.
		// System.arraycopy(payload, 2, temp, 0, len - 4);
		byte[] temp = new byte[len - 3];// contain head.
		System.arraycopy(payload, 0, temp, 0, len - 3);
		int crc8 = payload[len - 3] & 0xFF;
		int calc = CRC8.calcCrc8(temp) & 0xFF;
		return crc8 == calc;
	}

	public static String getNowTimeString() {
		Date date = new Date(System.currentTimeMillis());
		SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		String dateStr = format.format(date);
		return dateStr;
	}

	/**
	 * String date change to time stamp.
	 * 
	 * @param time
	 * @return
	 */
	public static long getFormatDate(String time) {
		try {
			SimpleDateFormat formatter = new SimpleDateFormat("yyyyMMddHHmmss");
			return formatter.parse(time).getTime();
		} catch (ParseException e) {
			e.printStackTrace();
		}
		return -1;
	}

	public static int byteHigh4Bit(byte by){
		return (by & 0xf0) >> 4;
	}
	
	public static int byteLow3Bit(byte by){
		int lo4 = by & 0x0f;
		return (lo4<<1 &0xf)>> 1;
	}
	
	public static String byte2BitStr(byte by) {
		StringBuffer sb = new StringBuffer();
		sb.append((by >> 7) & 0x1).append((by >> 6) & 0x1)
				.append((by >> 5) & 0x1).append((by >> 4) & 0x1)
				.append((by >> 3) & 0x1).append((by >> 2) & 0x1)
				.append((by >> 1) & 0x1).append((by >> 0) & 0x1);
		return sb.toString();
	}

	public static byte bitStr2Byte(String bit) {
		int re, len;
		if (null == bit) {
			return 0;
		}
		len = bit.length();
		if (len != 4 && len != 8) {
			return 0;
		}
		if (len == 8) {// 8 bit
			if (bit.charAt(0) == '0') {// positive number;
				re = Integer.parseInt(bit, 2);
			} else {// negative number;
				re = Integer.parseInt(bit, 2) - 256;
			}
		} else {// 4 bit
			re = Integer.parseInt(bit, 2);
		}
		return (byte) re;
	}
	
	/**
	 * 
	 * @param value
	 * @param decimal eg:two decimal digits is 0.01
	 * @return
	 */
	public static String handleDecimal(int value, double decimal){
		if(decimal == 1d){
			return String.valueOf(value);
		}
		BigDecimal a = new BigDecimal(value);
		BigDecimal b = BigDecimal.valueOf(decimal);
		return String.valueOf(a.multiply(b).doubleValue());
	}
	
	public static String handleDecimal(long value, double decimal){
		if(decimal == 1d){
			return String.valueOf(value);
		}
		BigDecimal a = new BigDecimal(value);
		BigDecimal b = BigDecimal.valueOf(decimal);
		return String.valueOf(a.multiply(b).doubleValue());
	}
}
