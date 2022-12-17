#include <Networking/ServerSocket.h>

#include "LocalChat.h"

#include <array>
#include <Message/TextMessage.h>

int main()
{
	TextMessage mesg(L"Hello World", L"*end*");
	std::wcout << mesg << std::endl;
}
