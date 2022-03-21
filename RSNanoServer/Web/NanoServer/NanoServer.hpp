//
//  NanoServer.hpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 04/03/2021.
//

#ifndef NanoServer_hpp
#define NanoServer_hpp

#include <stdio.h>
#include <memory>

#include "../Managers/HeaderManager/HeaderManager.hpp"
#include "../Managers/MessageManager/MessageManager.hpp"
#include "../Managers/ClientManager/ClientManager.hpp"


class NanoServer {
	public:
		
		NanoServer(const int port, const int maxClients, const int maxPending);
		
		void start();
	
		void onRecieve(const std::function<void (Client, Message)> & handler);
	
		void onClose(const std::function<void (Client)> & handler);
	
		void send(const Client & toClient, const Message & withMessage);
	
		void close(const Client & client);
	
	private:
		
		//Server config
		const int port;
		const int maxClients;
		const int maxPending;

		void configureSocket(int socketNo, bool master = false);
	
		//Actions
		std::pair<std::string, int> recieve(int socketNo);
		void send(int socketNo, const Message & message);
		void close(int socketNo);
		
		//User Actions
		std::function<void (Client, Message)> recieveHandler;
		std::function<void (Client)> closeHandler;
		
	
		//Managers
		std::shared_ptr<HeaderManager> headerManager;
		std::shared_ptr<MessageManager> messageManager;
		std::shared_ptr<ClientManager> clientManager;

};
#endif /* NanoServer_hpp */
