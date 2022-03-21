//
//  ClientManager.cpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 03/04/2021.
//

#include "ClientManager.hpp"
#include <iostream>

ClientManager::ClientManager(int maxClients) : maxClients(maxClients){
	this->clients = new std::shared_ptr<Client> [maxClients];
}

ClientManager::~ClientManager() {
	delete [] this->clients;
	this->clients = nullptr;
}


void ClientManager::removeClient(int index) {
	this->clients[index].reset();
	this->clients[index] = nullptr;
	this->currentClientSize --;
}


std::shared_ptr<Client> ClientManager::operator[](int index) {
	return this->clients[index];
}

void ClientManager::addClient(std::shared_ptr<Client> client) {
	bool added = false;
	if ( this->currentClientSize + 1 == maxClients ) {
		throw std::runtime_error("[ERROR] - unable to add client - maximum concurrent connections reached");
	}
	for (auto i = 0; i < this->maxClients; i++) {
		if( this->clients[i] == nullptr ) {
			this->clients[i] = client;
			this->currentClientSize ++;
			added = true;
			break;
		}
	}
	if ( added == false ) {
		throw std::runtime_error("[ERROR] - unable to add client - maximum concurrent connections reached");
	}
}

