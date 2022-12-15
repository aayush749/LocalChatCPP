// LocalChat.cpp : Defines the entry point for the application.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Networking/ServerSocket.h>

#include "LocalChat.h"

using namespace std;

int main()
{
	//WSADATA wsaData;
	//int iResult;

	//// Initialize Winsock
	//iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (iResult != 0) {
	//	printf("WSAStartup failed: %d\n", iResult);
	//	return 1;
	//}
	//else
	//	printf("Succesful!\n\n");

	//PCSTR PORT = "7777";
	//addrinfo* result = nullptr, * ptr = nullptr, hints;

	//ZeroMemory(&hints, sizeof(hints));
	//hints.ai_family = AF_INET;
	//hints.ai_socktype = SOCK_STREAM;
	//hints.ai_protocol = IPPROTO_TCP;
	//hints.ai_flags = AI_PASSIVE;

	//// Resolve the local address and port to be used by the server
	//iResult = getaddrinfo(NULL, PORT, &hints, &result);
	//if (iResult != 0) {
	//	printf("getaddrinfo failed: %d\n", iResult);
	//	WSACleanup();
	//	return 1;
	//}


	//SOCKET ListenSocket = INVALID_SOCKET;
	//ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	//if (ListenSocket == INVALID_SOCKET) {
	//	printf("Error at socket(): %ld\n", WSAGetLastError());
	//	freeaddrinfo(result);
	//	WSACleanup();
	//	return 1;
	//}

	//iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	//if (iResult == SOCKET_ERROR) {
	//	printf("bind failed with error: %d\n", WSAGetLastError());
	//	freeaddrinfo(result);
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//freeaddrinfo(result);

	//if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
	//	printf("Listen failed with error: %ld\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//SOCKET ClientSocket = INVALID_SOCKET;

	//// Accept a client socket
	//SOCKADDR_IN client_info = { 0 };
	//int size = sizeof(client_info);
	//ClientSocket = accept(ListenSocket, (SOCKADDR*) & client_info, &size);
	//if (ClientSocket == INVALID_SOCKET) {
	//	printf("accept failed: %d\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}
	//
	//char* ipAddr = inet_ntoa(client_info.sin_addr);
	//printf("Socket connected: %s\n", ipAddr);

	//const int DEFAULT_BUFLEN = 512;
	//char recvbuf[DEFAULT_BUFLEN];
	//int iSendResult;
	//int recvbuflen = DEFAULT_BUFLEN;

	//// Receive until the peer shuts down the connection
	//do {

	//	iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
	//	if (iResult > 0) {
	//		printf("Bytes received: %d\n", iResult);
	//		printf("%s\n\n", recvbuf);

	//		// Echo the buffer back to the sender
	//		iSendResult = send(ClientSocket, recvbuf, iResult, 0);
	//		if (iSendResult == SOCKET_ERROR) {
	//			printf("send failed: %d\n", WSAGetLastError());
	//			closesocket(ClientSocket);
	//			WSACleanup();
	//			return 1;
	//		}
	//		printf("Bytes sent: %d\n", iSendResult);
	//	}
	//	else if (iResult == 0)
	//		printf("Connection closing...\n");
	//	else {
	//		printf("recv failed: %d\n", WSAGetLastError());
	//		closesocket(ClientSocket);
	//		WSACleanup();
	//		return 1;
	//	}

	//} while (iResult > 0);

	ntwk::ServerSocket serverSock(AF_INET, 7777, "localhost");
	std::cout << serverSock << std::endl;

	ntwk::Socket client = serverSock.Accept();
	std::cout << "Client Details: " << client << std::endl;
	client.Close();
	return 0;
}
