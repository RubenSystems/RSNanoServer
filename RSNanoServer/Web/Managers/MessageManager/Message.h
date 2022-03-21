//
//  Message.h
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 01/04/2021.
//

#ifndef Message_h
#define Message_h

#include "Opcode.h"
#include <memory>
#include <string>

class Message {
	
	public:
		Message(Opcode opcode, const std::string data, bool masked = false) : opcode(opcode), data(data), masked(masked) {}
	
		Message() = default; 
	
		Opcode opcode;
		std::string data;
		bool masked;
	
		int payloadSize() { return ( int )this->data.size(); }
		
};

struct MessageHeader {
	unsigned short opcode : 4;
	unsigned short rsv1 : 1;
	unsigned short rsv2 : 1;
	unsigned short rsv3 : 1;
	unsigned short fin : 1;
	unsigned short payload_len: 7;
	unsigned short mask : 1;
};

#endif /* Message_h */
