

TLVFrame = (function (global) {
	
	})(window);
	
	function validate(topic, payload){
		if(payload === 'undefined'){
			console.log(">>>>> invalid Frame Payload");
		}
		
		var len = payload.length;
		
		if(len < 21){
			console.log(">>>>> invalid Frame length");
		}
		
		if((payload[0] != 0xAA) || 
		   (payload[1] != 0xFE) ||
		   (payload[len-2] != 0xDD) ||
		   (payload[len-1] != 0xEE)
		){
			console.log(">>>>> invalid Frame head/Tail");
			return false;
		}
		
		var fr_len = ((payload[3] & 0xff) << 8) | (payload[4] & 0xff) ;
		
		if(fr_len != len){
			console.log(">>>>> Frame length un-matched..");
			return false;
		}
		
		//crc8
		
		console.log(">>>>> Frame check passed..");
		return true;
	};
	
	function decode(topic, payloadBytes){
		var res = [];
		
		if(validate(topic, payloadBytes) != true){
			return res;
		}
		
		if(payloadBytes[11] != 0x01){
			console.log(">>>>> Not RealTime Data, Not handled...");
			return res;
		}
		
		var fbody = new Array();
		var fbody_len = payloadBytes.length - 21;
		
		for(var i=0; i<fbody_len; i++){
			fbody.push(payloadBytes[18+i]);
		}
		
		//console.log(fbody);

		decode_tlv(fbody, 0, res);
		
		return res;
	}
	
	function decode_tlv(data, offset, res){
		
		
		if(offset < data.length - 4){
			var modbus_tag 		= Byte2Hex(data[offset]);
			var channel_tag 	= Byte2Hex(data[offset + 1]);
			var data_tag 		= Byte2Hex(data[offset + 2]);		
			var data_type 		= data[offset + 3];			
			
			var vtype 		= getValueType(data_type);
			var vlen 		= getValueLength(data_type);
			var precision 	= getValuePrecision(data_type);
			var isBatch 	= isBatchbitActived(data_type);
			
			var array = range(data, offset + 4, offset + 4 + vlen, 1);
			
			//console.log(array);
			
			var value = calcValue(vtype, precision, array);
			
			res.push({"modbus_tag":modbus_tag, "channel_tag":channel_tag, "data_tag":data_tag, "value": value});
			
			decode_tlv(data, offset + 4 + vlen, res);			
		}
		
		function range(array, start, stop, step)
		{
			if (typeof(stop) == 'undefined')
			{
				// one param defined
				stop = start;
				start = 0;
			}
			if (typeof(step) == 'undefined')
			{
				step = 1;
			}
			if ((step > 0 && start >= stop) || (step < 0 && start <= stop))
			{
				return [];
			}
			var result = [];
			for (var i = start; step > 0 ? i < stop : i > stop; i += step)
			{
				result.push(array[i]);
			}
			return result;
		}
		
		
		function calcValue( type,  precision, value){
			var func = new Utility;
			
			switch(type){
			case 0:			//bool
				return (value[0] > 0) ? 1 : 0;
				
			case 1:		//signed char
				return new Number(func.byteToSignedChar(value[0])*Math.pow(10, -1*precision)).toFixed(precision);
				
			case 2:		//unsigned char
				return new Number(func.byteToUnsignedChar(value[0])*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byteToUnsignedChar(value[0])).movePointLeft(precision).doubleValue());
				
			case 3:		//signed short
				return new Number(func.byte2ToSignedShort(value[0], value[1])*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byte2ToSignedShort(value[0], value[1])).movePointLeft(precision).doubleValue());
				
			case 4:		//unsigned short
				return new Number(func.byte2ToUnsignedShort(value[0], value[1])*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byte2ToUnsignedShort(value[0], value[1])).movePointLeft(precision).doubleValue());			
				
			case 5:		//signed int
				return new Number(func.byte4ToSignedInt(value[0], value[1],value[2], value[3])*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byte4ToSignedInt(value[0], value[1],value[2], value[3])).movePointLeft(precision).doubleValue());			
				
			case 6:		//unsigned int
				return new Number(func.byte4ToUnsignedInt(value[0], value[1],value[2], value[3])*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byte4ToUnsignedInt(value[0], value[1],value[2], value[3])).movePointLeft(precision).doubleValue());
				
			case 7:		//signed long
				return new Number(func.byte8ToLong(value[0], value[1],value[2], value[3], value[5], value[6],value[7], value[8] )*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byte8ToLong(value[0], value[1],value[2], value[3], value[5], value[6],value[7], value[8] )).movePointLeft(precision).doubleValue());
							
			case 8:		//unsigned long
				//TODO ,bug, overflow
				return new Number(func.byte8ToLong(value[0], value[1],value[2], value[3], value[5], value[6],value[7], value[8] )*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(new BigDecimal(Utility.byte8ToLong(value[0], value[1],value[2], value[3], value[5], value[6],value[7], value[8] )).movePointLeft(precision).doubleValue());
				
			case 9:		//float
				return new Number(func.byte4ToSignedInt(value[0], value[1],value[2], value[3])*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(Float.intBitsToFloat(Utility.byte4ToSignedInt(value[0], value[1],value[2], value[3])));  
				
			case 10:		//double
				return new Number(func.byte8ToLong(value[0], value[1],value[2], value[3], value[5], value[6],value[7], value[8] )*Math.pow(10, -1*precision)).toFixed(precision);
				//return String.valueOf(Double.longBitsToDouble(Utility.byte8ToLong(value[0], value[1],value[2], value[3],value[5], value[6],value[7], value[8])));
			}
			
			return "";
		}
		
		
		function getValueType(type){
			return (type & 0xf0) >> 4;
		}
	
		function getValueLength(type){
			var vlue = (type & 0xf0) >> 4;
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
		
			throw new Error("Unsupport TLV Value type: " + vlue);
		}
	
		function getValuePrecision(type){
			return type & 0x07;
		}
	
		function isBatchbitActived(type){
			return (type & 0x08) != 0x00;
		}
		
		
		//this func is just transfer byte array to hex string
		function Byte2Hex(B)
		{  
			var tmp = B.toString(16);    

			if(tmp.length == 1)    
			{    
				tmp = "0" + tmp;    
			}    

			return tmp.toUpperCase();
		}
		
		function Bytes2Hexstring(arr)
		{  
			var str = "";    
			for(var i=0; i<arr.length; i++)    
			{    
				var tmp = arr[i].toString(16);    
				if(tmp.length == 1)    
				{    
					tmp = "0" + tmp;    
				}    
				str += tmp;    
			}  
			return str,toUpperCase();    
		}
	}
	
	var Utility = function(){
			
	
		this.byteToSignedChar = function(value){		
			return (value);			
		}
	
		this.byteToUnsignedChar = function(value){		
			var ret = ((value  & 0xff) >>> 0);
			//console.log("byteToUnsignedChar:" + ret);
			return ret;
		}
	
		this.byte2ToSignedShort = function(val1, val2){		
			return ((((val1 & 0xff) << 8 ) | ((val2  & 0xff))) << 0);			
		}
	
		this.byte2ToUnsignedShort = function(val1, val2){		
			var ret = ((((val1 & 0xff) << 8) | (val2  & 0xff)) >>> 0);
			//console.log("byte2ToUnsignedShort:" + val1+ "," + val2 +"," + ret);
			return ret;
		}
	
		this.byte4ToSignedInt = function(val1, val2, val3, val4) {
			var ret = ((val1 & 0xff) << 24) | ((val2 & 0xff) << 16)
					| ((val3 & 0xff) << 8) | (val4 & 0xff);
			return (ret << 0);
		}
	
		this.byte4ToUnsignedInt = function(val1, val2, val3, val4) {
			var ret =((val1 & 0xff) << 24) | ((val2 & 0xff) << 16)
				| ((val3 & 0xff) << 8) | (val4 & 0xff);
				
			//console.log("byte4ToUnsignedInt:" + ret);
			//console.log("val1:" + val1);
			//console.log("val2:" + val2);
			//console.log("val3:" + val3);
			//console.log("val4:" + val4);
			return (ret >>> 0);
		}

		this.byte8ToLong = function(val1, val2, val3, val4, val5, val6, val7, val8) {
			return ((val1 &  0xffffffffffffffff) << 56)
				| ((val2 & 0xffffffffffffffff) << 48) | ((val3 & 0xffffffffffffffff) << 40)
				| ((val4 & 0xffffffffffffffff) << 32) | ((val5 & 0xffffffffffffffff) << 24)
				| ((val6 & 0xffffffffffffffff) << 16) | ((val7 & 0xffffffffffffffff) << 8) | (val8 & 0xffffffffffffffff);
		}
		
	}
	
	
