//
//  Opcode.h
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 01/04/2021.
//

#ifndef Opcode_h
#define Opcode_h

enum Opcode {
	textFrame = 129,
	closeFrame = 136,
	pingFrame = 137,
	pongFrame = 138
};

#endif /* Opcode_h */

//	ERROR_FRAME=0xFF00,
//	INCOMPLETE_FRAME=0xFE00,
//
//	OPENING_FRAME=0x3300,
//	CLOSING_FRAME=0x3400,
//
//	INCOMPLETE_TEXT_FRAME=0x01,
//	INCOMPLETE_BINARY_FRAME=0x02,
//
//	TEXT_FRAME=0x81,
//	BINARY_FRAME=0x82,
//
//	PING_FRAME=0x19,
//	PONG_FRAME=0x1A
