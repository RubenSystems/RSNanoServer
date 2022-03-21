//
//  ClientManager.hpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 03/04/2021.
//

#ifndef ClientManager_hpp
#define ClientManager_hpp

#include <stdio.h>
#include <memory>

#include "Client.h"


class ClientManager {
	
	
	public:
		ClientManager(int maxClients);
	
		~ClientManager();
	
		std::shared_ptr<Client> operator[](int index);
	
		void addClient(std::shared_ptr<Client> client);
		
		void removeClient(int index);
	
	private:
		const int maxClients;
	
		int currentClientSize = 0;
	
		std::shared_ptr<Client> * clients;
	
		
	
};

#endif /* ClientManager_hpp */
