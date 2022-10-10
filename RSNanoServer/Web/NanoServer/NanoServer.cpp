//
//  NanoServer.cpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 04/03/2021.
//

#include "NanoServer.hpp"

#include <string>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>


#include <iostream>


NanoServer::NanoServer(const int port, const int maxClients, const int maxPending) :
	port (port), maxClients(maxClients), maxPending(maxPending){
	this->headerManager = std::shared_ptr<HeaderManager>(new HeaderManager);
	this->messageManager = std::shared_ptr<MessageManager>(new MessageManager);
	this->clientManager = std::shared_ptr<ClientManager> (new ClientManager(this->maxClients));
}


void NanoServer::start() {
	int master_socket, addrlen, activity, i, sd;
	int max_sd;
	struct sockaddr_in address;

	fd_set readfds;

	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	this->configureSocket(master_socket, true);

	
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( this->port );

	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(master_socket, this->maxPending) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
			 
	addrlen = sizeof(address);
	std::cout << "[LOG] - server online\n";
			 
	while( true ) {
		FD_ZERO(&readfds);
	 
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			 
		for ( i = 0 ; i < this->maxClients ; i++) {
			//socket descriptor
			auto client = this->clientManager->operator[](i);
			if (client == nullptr) { continue; } else { sd = client->socketNo; }
			
			if(sd > 0) {
				FD_SET(sd , &readfds);
			}
				 
			if(sd > max_sd) {
				max_sd = sd;
			}
		}
	 
		if (((activity = select(max_sd + 1, &readfds, NULL, NULL, NULL)) < 0) && (errno != EINTR)) {
			perror("select");
			exit(EXIT_FAILURE);
		}
			 
		if (FD_ISSET(master_socket, &readfds)) { 
			int new_socket;
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
				perror("accept");
				exit(EXIT_FAILURE);
			}
			this->configureSocket(new_socket);
			
			std::string handshake = this->recieve(new_socket).first;
 			std::string message = this->headerManager->generateWebSocketHeader(handshake);
			
			
			::send(new_socket, message.c_str(), message.size(), 0);
				 
			
			this->clientManager->addClient(std::make_shared<Client> (new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port)));

		} else { 
			
			for (i = 0; i < this->maxClients; i++) {
				std::shared_ptr<Client> client;
				if ((client = this->clientManager->operator[](i)) == nullptr){ break; } else {sd = client->socketNo;}
					 
				if (FD_ISSET(sd, &readfds)) {
					auto valread = this->recieve(sd);
					
					//Check if it was for closing , and also read the incoming message
					if (valread.second <= 0) {
						this->closeHandler(*client);
						close( sd );
					} else {
						this->recieveHandler(*client, this->messageManager->parseFrame(valread.first));
					}
				}
			}
		}
	}
}

//USER ACTIONS
void NanoServer::onRecieve(const std::function<void (Client, Message)> & handler) {
	this->recieveHandler = handler;
}

void NanoServer::onClose(const std::function<void (Client)> & handler) {
	this->closeHandler = handler;
}

void NanoServer::send(const Client & toClient, const Message & withMessage) {
	this->send(toClient.socketNo, withMessage);
}

void NanoServer::close(const Client & client) {
	this->close(client.socketNo);
}

//Internal
std::pair<std::string, int> NanoServer::recieve(int socketNo) {
	char buffer [1025];
	auto status = ::recv(socketNo, buffer, 1025, NULL);
	buffer[status + 1] = '\0';
	std::string bufString = std::string(buffer);

	return std::make_pair(bufString, status);
}

void NanoServer::send(int socketNo, const Message & message) {
	auto response = this->messageManager->generateFrame(message);
	::send(socketNo, response.c_str(), response.size(), MSG_NOSIGNAL);
}


void NanoServer::configureSocket(int socketNo, bool master) {
	int opt = 1;
	if( master ) {
		if (setsockopt(socketNo, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
			perror("setsockopt");
			exit(EXIT_FAILURE);
		}
	} else {
		if ( setsockopt(socketNo, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt)) < 0 ) {
			std::cout << "[WARN] - keeping client alive failed\n";
		} else if (setsockopt(socketNo, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt)) < 0) {
			std::cout << "[WARN] - tcp did not work\n";
		}
	}
}

void NanoServer::close(int socketNo) {
//	getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
//	printf("Host disconnected, ip %s, port %d \n", inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
	
	::close(socketNo);
	for (int i = 0; i < this->maxClients; i ++) {
		std::shared_ptr<Client> client;
		if ((client = this->clientManager->operator[](i) ) != nullptr && client->socketNo == socketNo) {
			this->clientManager->removeClient(i);
			break;
		}
	}
}
