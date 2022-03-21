//
//  MessageManager.hpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 15/03/2021.
//

#ifndef MessageManager_hpp
#define MessageManager_hpp

#include <stdio.h>
#include <string>

#include "Message.h"

class MessageManager {
	
	public:

		Message parseFrame (const std::string & data);
		
		std::string generateFrame (const Message & message);
			
	private:
	
		unsigned char randomChar();
};

#endif /* MessageManager_hpp */
