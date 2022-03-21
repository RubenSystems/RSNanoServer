//
//  HeaderManager.cpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 06/03/2021.
//

#include "HeaderManager.hpp"

#include <iostream>
#include <sstream>

#include "Crypto/SHA1.h"
#include "Crypto/Base64.hpp"



std::string HeaderManager::generateWebSocketHeader(const std::string & content) {
	auto parsedResponse = this->parseHeader(content);
	if (parsedResponse.find("Sec-WebSocket-Key") == parsedResponse.end()) {
		throw std::runtime_error("[ERROR] - not valid websocket");
	}
	
	
	
	std::string header = "HTTP/1.1 101 Switching Protocols\r\n";
	header += "Upgrade: websocket\r\n";
	header += "Connection: Upgrade\r\n";
	header += "Sec-WebSocket-Accept: " + this->generateHandshake(parsedResponse["Sec-WebSocket-Key"]);
	header += "\r\n\r\n";
	
	return header;
	
}

std::string generateHTTPHeader(const std::string & content) {
	return "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: "+std::to_string(content.size())+"\n\n"+content+"\r\n";
}

std::string HeaderManager::generateHandshake(const std::string & key) {
	SHA1 hasher;
	Base64 encoder;
	
//	std::cout << "====toencrypt====-" << key << "-=====\n";
	hasher.update(key + this->websocketGUID);
	
	auto hashed = hasher.final();
	
	char bytes[hashed.length()/2];
	std::stringstream converter;
	for(int i = 0; i < hashed.length(); i+=2) {
		converter << std::hex << hashed.substr(i,2);
		int byte;
		converter >> byte;
		bytes[i/2] = byte & 0xFF;
		converter.str(std::string());
		converter.clear();
	}
	
	return encoder.encode(bytes);
}

std::unordered_map<std::string, std::string> HeaderManager::parseHeader(const std::string & content) {
	std::unordered_map<std::string, std::string> values;
	for(auto item : this->split(content, '\n')) {
		auto keyValueSplit = this->split(item, ':');
		if (keyValueSplit.size() == 2) {
			
			values[keyValueSplit[0]] = keyValueSplit[1].substr(1, keyValueSplit[1].size() - 2 );
		}
	}
	return values;
}

std::vector<std::string> HeaderManager::split(const std::string & text, const char & breaker) {
	std::string tmp;
	std::vector<std::string> stk;
	std::stringstream ss(text);
	while(getline(ss, tmp, breaker)) {
		stk.push_back(tmp);
	}
	return stk;
}
