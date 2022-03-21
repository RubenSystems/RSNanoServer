//
//  main.cpp
//  RSNanoTube
//
//  Created by Ruben Ticehurst-James on 04/03/2021.
//

#include <iostream>


using namespace std;

#include "Web/NanoServer/NanoServer.hpp"

int main(int argc, const char * argv[]) {

	NanoServer x (8080, 30, 10);
	
	x.onRecieve([ &x ] ( Client client, Message message ) -> void {
		std::cout << message.data << "\n";
		
		x.send(client, Message(textFrame, "Well hello there client. IT truely is a pleasure to meat you!"));
		
		x.close(client);
	});
	
	x.onClose([ &x ] (Client client) -> void {
		std::cout << "[LOG] - client closed: " << client.ip << "\n";
	});
	
	x.start();
	
	
	return 0;
}

