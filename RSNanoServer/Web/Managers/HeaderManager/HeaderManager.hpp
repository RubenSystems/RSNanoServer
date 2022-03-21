//
//  HeaderManager.hpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 06/03/2021.
//

#ifndef HeaderManager_hpp
#define HeaderManager_hpp

#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <string>

class HeaderManager {
	public:
		static const int headerBufferSize = 1024;
	
		std::string generateWebSocketHeader(const std::string & content);
	
		std::string generateHTTPHeader(const std::string & content);
	
		

	private:
		std::unordered_map<std::string, std::string> parseHeader(const std::string & content);
	
		std::string generateHandshake(const std::string & key);
	
		std::vector<std::string> split(const std::string & text, const char & breaker);
	
		std::string websocketGUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
		//dudheader : HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!
	
};


#endif /* HeaderManager_hpp */
