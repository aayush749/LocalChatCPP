#include <Networking/Networking.h>

#include <cstdio>
#include <stdexcept>

WSADATA wsaData;

void InitWSA()
{
    // Initialize Winsock

    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        char MSG[512] = "WSAStartup failed: %d\n";
        snprintf(MSG, sizeof(MSG), MSG, iResult);
        throw std::runtime_error(MSG);
    }
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        // Initialize once for each new process.
        // Return FALSE to fail DLL load. 
        InitWSA();
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.            
        break;

    case DLL_PROCESS_DETACH:
        // Perform any necessary cleanup.
        WSACleanup();
        break;
    }
    return TRUE;
}