//
//  MessageManager.cpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 15/03/2021.
//

#include "MessageManager.hpp"

//#include "../HeaderManager/HeaderManager.hpp"

#include <sstream>
#include <iostream>


Message MessageManager::parseFrame(const std::string & data) {
	// output
	Message message;
	
//	request Decoded;
//	Decoded.type = "Unknown";
//	Decoded.exitcode = 0;
//	Decoded.payload = "";
//
//	// header format
	MessageHeader * header;
	header = ( struct MessageHeader * )data.substr(0, 2).c_str();
//
		
	
//	printf("Header: OPCODE %x MASKED? %d, LENGTH: %d\n", message.opcode, message.masked, message.payloadSize() );
	
	// check the op-code
	switch (header->opcode) {
		case 1:
			message.opcode = textFrame;
			break;
		case 8:
			message.opcode = closeFrame;
			break;
		case 9:
			message.opcode = pingFrame;
			break;
		case 10:
			message.opcode = closeFrame;
			break;
		default:
			throw std::runtime_error(std::string("[ERROR] - unable to handle opcode of type" + std::to_string(header->opcode)));
			break;
	}
	
	std::string payloadBody = data.substr(2); // remove 2 byte long header
	std::string payloadMask;
	
	int payload_offset = 0;

	if (header->payload_len == 126) {
		// 2 byte payload length
		payload_offset += 2;
	} else if (header->payload_len == 127) {
		// 8 byte payload length
		payload_offset += 8;
	}
	
//	delete header;
//	header = nullptr;
	
//	printf("Payload offset: %d \n", payload_offset);
	
	if ((data[1]&0x80)) {
		// extract the mask
		payloadMask = payloadBody.substr(payload_offset, 4);
		payload_offset += 4; // mask key is 4 bytes long
	}
	
//	printf("Payload offset: %d \n", payload_offset);
	
	payloadBody = payloadBody.substr(payload_offset);
//	printf("MASK: %s PAYLOAD: %s \n", payloadMask.c_str(), payloadBody.c_str());
	
	// unmask payloadBody
	int payloadBodyLength = (int)payloadBody.length();
	
	std::stringstream unm;
	for (int i = 0; i < payloadBodyLength; i++) {
		unsigned char d = payloadBody[i];
		unsigned char m = payloadMask[i % 4];
		unsigned char unmasked = d ^ m;
		
		unm << unmasked;
	}
	
//	printf("UNMASKED: %s \n", unm.str().c_str());

	message.data = unm.str();
	message.masked = true;
	
	
	return message;
}

std::string MessageManager::generateFrame (const Message & message) {
	std::stringstream sstr;
			
	// set header flags: fin, rsv, opcode
	unsigned char head = message.opcode;
	
	
	sstr << head;
	
	// set payload info
	size_t length = message.data.length();
	
	if (length > 65535) { // 9 bytes needed
		unsigned long len = length;
		
		unsigned char b1 = (message.masked ? 255 : 127);
		sstr << b1;
		
		for (int i = 7; i >= 0; i--) {
			unsigned char b = 0;
			
			for (int j = 0; j < 8; j++) {
				unsigned char shift = 0x01 << j;
				shift = shift << (8 * i);
				
				b += pow(2, j) * (len&shift);
			}
			
			if (i == 7 && b > 127) { // frame too big
				return "";
			}
			
			sstr << b;
		}
		
	} else if (length > 125) { // 3 bytes needed
		unsigned short len = length;
		
		unsigned char b1 = (message.masked ? 254 : 126);
		
		unsigned char b2 = 128 * (len&0x8000) + 64 * (len&0x4000) + 32 * (len&0x2000) + 16 * (len&0x1000);
		b2 += 8 * (len&0x800) + 4*(len&0x400) + 2*(len&0x200) + (len&0x100);
		
		unsigned char b3 = 128 * (len&0x80) + 64 * (len&0x40) + 32 * (len&0x20) + 16 * (len&0x10);
		b3 += 8 * (len&0x08) + 4*(len&0x04) + 2*(len&0x02) + (len&0x01);
		
		sstr << b1;
		sstr << b2;
		sstr << b3;
	} else { // 1 byte needed
		unsigned char b = (message.masked ? length + 128 : length);
		sstr << b;
	}
	
	unsigned char mask[4];
	
	if (message.masked) {
		// generate mask
		for (int i = 0; i < 4; i++) {
			mask[i] = this->randomChar();
			sstr << mask[i];
		}
	}
	
	for (int i = 0; i < length; i++) {
		unsigned char chr = message.data[i];
		unsigned char res = (message.masked ? (chr ^ mask[i % 4]) : chr);
		
		sstr << res;
	}
	
	return sstr.str();
}

unsigned char MessageManager::randomChar(){
   static const char alphanum[] =
	   "0123456789"
	   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	   "abcdefghijklmnopqrstuvwxyz";

   return alphanum[rand() % (sizeof(alphanum) - 1)];
}


/*
std::pair<std::optional<std::string>, WebSocketFrameType> MessageManager::parse(unsigned char inBuffer [HeaderManager::headerBufferSize], int inLength) {
	
	WebSocketFrameType frametype ;
	if(inLength < 3) {
		frametype = INCOMPLETE_FRAME;
		return { std::nullopt, frametype };
	}

	unsigned char msg_opcode = inBuffer[0] & 0x0F;
	unsigned char msg_fin = (inBuffer[0] >> 7) & 0x01;
	unsigned char msg_masked = (inBuffer[1] >> 7) & 0x01;

	// *** message decoding

	int payload_length = 0;
	int pos = 2;
	int length_field = inBuffer[1] & (~0x80);
	unsigned int mask = 0;

	//printf("IN:"); for(int i=0; i<20; i++) printf("%02x ",buffer[i]); printf("\n");

	if(length_field <= 125) {
		payload_length = length_field;
	} else if(length_field == 126) { //msglen is 16bit!
		//payload_length = in_buffer[2] + (in_buffer[3]<<8);
		payload_length = (
			(inBuffer[2] << 8) |
			(inBuffer[3])
		);
		pos += 2;
	} else if(length_field == 127) { //msglen is 64bit!
		payload_length = (
			(inBuffer[2] << 56) |
			(inBuffer[3] << 48) |
			(inBuffer[4] << 40) |
			(inBuffer[5] << 32) |
			(inBuffer[6] << 24) |
			(inBuffer[7] << 16) |
			(inBuffer[8] << 8) |
			(inBuffer[9])
		);
		pos += 8;
	}

	//printf("PAYLOAD_LEN: %08x\n", payload_length);
	if(inLength < payload_length+pos) {
//		frametype =  INCOMPLETE_FRAME;
	}

	if(msg_masked) {
		mask = *((unsigned int*)(inBuffer+pos));
		//printf("MASK: %08x\n", mask);
		pos += 4;

		// unmask data:
		unsigned char* c = inBuffer+pos;
		for(int i=0; i<payload_length; i++) {
			c[i] = c[i] ^ ((unsigned char*)(&mask))[i%4];
		}
	}

	//TODO: if output buffer is too small -- ERROR or resize(free and allocate bigger one) the buffer ?
	//if(payload_length > out_size) {}
	std::string toReturn (reinterpret_cast< char const* >( inBuffer + pos ));
//	std::cout << "TEXT : " << toReturn << "\n";

	if (msg_opcode == 0x0) {
		frametype = (msg_fin) ? TEXT_FRAME : INCOMPLETE_TEXT_FRAME;
		return {(msg_fin) ? std::optional<std::string>(toReturn) : std::nullopt, frametype};
	} else if (msg_opcode == 0x1) {
		frametype = (msg_fin) ? TEXT_FRAME : INCOMPLETE_TEXT_FRAME;
		return {(msg_fin) ? std::optional<std::string>(toReturn) : std::nullopt, frametype};
	} else if (msg_opcode == 0x2) {
		frametype = (msg_fin) ? BINARY_FRAME : INCOMPLETE_BINARY_FRAME;
		return {(msg_fin) ? std::optional<std::string>(toReturn) : std::nullopt, frametype};
	} else if (msg_opcode == 0x9) {
		frametype = PING_FRAME;
		return {std::nullopt, frametype};
	} else if (msg_opcode == 0xA) {
		frametype = PONG_FRAME;
		return {std::nullopt, frametype};
	} else {
		frametype = ERROR_FRAME;
		return {std::nullopt, frametype};
	}
}


int MessageManager::makeFrame(WebSocketFrameType frame_type, unsigned char* msg, int msg_length, unsigned char* buffer) {

	int pos = 0;
	int size = msg_length;
	buffer[pos++] = (unsigned char)frame_type; // text frame

	if(size <= 125) {
		buffer[pos++] = size;
	}else if(size <= 65535) {
		buffer[pos++] = 126; //16 bit length follows

		buffer[pos++] = (size >> 8) & 0xFF; // leftmost first
		buffer[pos++] = size & 0xFF;
	}else { // >2^16-1 (65535)
		buffer[pos++] = 127; //64 bit length follows

		// write 8 bytes length (significant first)

		// since msg_length is int it can be no longer than 4 bytes = 2^32-1
		// padd zeroes for the first 4 bytes
		for(int i=3; i>=0; i--) {
			buffer[pos++] = 0;
		}
		// write the actual 32bit msg_length in the next 4 bytes
		for(int i=3; i>=0; i--) {
			buffer[pos++] = ((size >> 8*i) & 0xFF);
		}
	}
	memcpy((void*)(buffer+pos), msg, size);
	return (size+pos);
}



std::string MessageManager::generate(const WebSocketFrameType & frameType, const std::string & message) {
	int pos = 0;
	unsigned long size = message.size();
	
	unsigned char buffer [HeaderManager::headerBufferSize];
	
	buffer[pos++] = (unsigned char)frameType; // text frame

	if(size <= 125) {
		buffer[pos++] = size;
	}
	else if(size <= 65535) {
		buffer[pos++] = 126; //16 bit length follows
		
		buffer[pos++] = (size >> 8) & 0xFF; // leftmost first
		buffer[pos++] = size & 0xFF;
	}
	else { // >2^16-1 (65535)
		buffer[pos++] = 127; //64 bit length follows
		
		// write 8 bytes length (significant first)
		
		// since msg_length is int it can be no longer than 4 bytes = 2^32-1
		// padd zeroes for the first 4 bytes
		for(int i=3; i>=0; i--) {
			buffer[pos++] = 0;
		}
		// write the actual 32bit msg_length in the next 4 bytes
		for(int i=3; i>=0; i--) {
			buffer[pos++] = ((size >> 8*i) & 0xFF);
		}
	}
//	memcpy((void*)(buffer+pos), msg, size);
	
	return (std::string)(reinterpret_cast< char const* >( buffer + pos ));
}


std::string MessageManager::parsedWebSocketFrameType(const WebSocketFrameType & type) {
	switch (type) {
		case ERROR_FRAME:
			return std::string ("ERROR_FRAME");
		case INCOMPLETE_FRAME:
			return std::string ("INCOMPLETE_FRAME");
		case OPENING_FRAME:
			return std::string ("OPENING_FRAME");
		case CLOSING_FRAME:
			return std::string ("CLOSING_FRAME");
		case INCOMPLETE_TEXT_FRAME:
			return std::string ("INCOMPLETE_TEXT_FRAME");
		case INCOMPLETE_BINARY_FRAME:
			return std::string ("INCOMPLETE_BINARY_FRAME");
		case TEXT_FRAME:
			return std::string ("TEXT_FRAME");
		case BINARY_FRAME:
			return std::string ("BINARY_FRAME");
		case PING_FRAME:
			return std::string ("PING_FRAME");
		case PONG_FRAME:
			return std::string ("PONG_FRAME");
	}
}
*/
