#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main() {
					/***************************
					|							|
					|	1. Initialize winsock   |
					|							|
					****************************/
	WSADATA wsData;
	WORD version = MAKEWORD(2, 2);

	int wsOk = WSAStartup(version, &wsData);

	if (wsOk != 0) {
		std::cerr << "Cannot initialize winsock! Quitting" << std::endl;
		return;
	}

					/***************************
					|							|
					|	2. Create a socket	    |
					|							|
					****************************/

	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET) {
		std::cerr << "Can't create a socket! Quitting" << std::endl;
		return;
	}

			/***************************************************
			|													|
			|	3. Bind the IP address and port to a socket     |
			|													|
			****************************************************/

	sockaddr_in hint;
	hint.sin_family			  = AF_INET;
	hint.sin_port			  = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;			// equivalent to inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

		/***********************************************************
		|															|
		|	4. Tell Winsock the socket is for listening				|
		|															|
		************************************************************/

	listen(listening, SOMAXCONN);


					/*******************************
					|								|
					|	5. Wait for a connection    |
					|								|
					********************************/

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Could not receive information regarding client socket!" << std::endl;
		return;
	}

	char host[NI_MAXHOST];			// Client's remote name
	char service[NI_MAXSERV];		// Service (i.e. port) the client is connected on

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	// DNS lookup on the client's information
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " connected on port " << service << std::endl;
	} 
	else {
		// getting the IP address (which is on 4 bytes) as a string of char 
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " <<
			ntohs(client.sin_port) << std::endl;
	}

					/*******************************
					|								|
					|	6. Close listening socket   |
					|								|
					********************************/

	closesocket(listening);


		/***********************************************************
		|															|
		|	7. While loop: accept and echo message back to client   |
		|															|
		************************************************************/

	// read until you can't read more bytes
	char buf[4096];

	while (true) {
		ZeroMemory(buf, 4096);

		/***************************************
		|										|
		|	7.a) Wait for client to send data	|
		|										|
		****************************************/ 

		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "Error in recv()" << std::endl;
			break;
		}

		if (bytesReceived == 0) {
			std::cout << "Client disconnected" << std::endl;
			break;
		}


		/***************************************
		|										|
		|	7.b) Echo message back to client	|
		|										|
		****************************************/

		// when we receive a msg, we don't get the final 0 => bytesReceived + 1
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

					/***************************
					|							|
					|	8. Close the socket		|
					|							|
					****************************/
	
	closesocket(clientSocket);


					/***************************
					|							|
					|	9. Cleanup winsock	    |
					|							|
					****************************/

	WSACleanup();
}