//
//  Client.h
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 02/04/2021.
//

#ifndef Client_h
#define Client_h

#include <string>

class Client {
	public:
		Client() : socketNo (0), ip ("0.0.0.0"), port (0) {};
	
		Client(int socketNo, std::string ip, int port) : socketNo (socketNo), ip (ip), port (port) {}
		
		///Socket descriptor
		int socketNo;
	
		std::string ip;
	
		int port;
		
		
};

#endif /* Client_h */
