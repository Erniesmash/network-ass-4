/******************************************************************************/
/*!
\file		Main.cpp
\author 	
\par    	
\date   	
\brief		

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#include "main.h"

// ---------------------------------------------------------------------------
// Globals
float	  g_dt;
double  g_appTime;


/******************************************************************************/
/*!
	Starting point of the application
*/
/******************************************************************************/
int WINAPI WinMain(_In_ HINSTANCE instanceH, _In_opt_ HINSTANCE prevInstanceH, _In_ LPSTR command_line, _In_ int show)
{
	UNREFERENCED_PARAMETER(prevInstanceH);
	UNREFERENCED_PARAMETER(command_line);

	//// Enable run-time memory check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	#endif

	// Initialize the system
	AESysInit (instanceH, show, 800, 600, 1, 60, false, NULL);

	if (!freopen("CONIN$", "r", stdin)) {
		return 1;
	}

	if (!freopen("CONOUT$", "w", stdout)) {
		return 1;
	}

	if (!freopen("CONOUT$", "w", stderr)) {
		return 1;
	}

	int ret{ WinsockServerSetup() };
	if (ret) {
		return ret;
	}

	// Changing the window title
	AESysSetWindowTitle("Asteroids Server");

	//set background color
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

	GameStateMgrInit(GS_ASTEROIDS);

	while(gGameStateCurr != GS_QUIT)
	{
		// reset the system modules
		AESysReset();

		// If not restarting, load the gamestate
		if(gGameStateCurr != GS_RESTART)
		{
			GameStateMgrUpdate();
			GameStateLoad();
		}
		else
			gGameStateNext = gGameStateCurr = gGameStatePrev;

		// Initialize the gamestate
		GameStateInit();

		while(gGameStateCurr == gGameStateNext)
		{
			AESysFrameStart();

			AEInputUpdate();

			GameStateUpdate();

			GameStateDraw();
			
			AESysFrameEnd();

			// check if forcing the application to quit
			if ((AESysDoesWindowExist() == false) || AEInputCheckTriggered(AEVK_ESCAPE))
				gGameStateNext = GS_QUIT;

			g_dt = (f32)AEFrameRateControllerGetFrameTime();
			g_appTime += g_dt;
		}
		
		GameStateFree();

		if(gGameStateNext != GS_RESTART)
			GameStateUnload();

		gGameStatePrev = gGameStateCurr;
		gGameStateCurr = gGameStateNext;
	}

	// free the system
	AESysExit();
}

int WinsockServerSetup() {
	HWND hwndConsole = GetConsoleWindow();
	SetForegroundWindow(hwndConsole);

	std::string portString{};
	std::cout << "Server Port Number: ";
	std::cin >> portString;
	std::cout << std::endl;

	// Start Winsock
	WSADATA wsaData{};
	int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errorCode != NO_ERROR) {
		std::cerr << "WSAStartup() failed." << std::endl;
		return errorCode;
	}

	// Get Address Info
	addrinfo hints{};
	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	constexpr int HOSTBUFFERSIZE = 16;
	char hostBuffer[HOSTBUFFERSIZE];
	gethostname(hostBuffer, HOSTBUFFERSIZE);

	addrinfo* info = nullptr;
	errorCode = getaddrinfo(hostBuffer, portString.c_str(), &hints, &info);
	if ((errorCode) || (info == nullptr)) {
		std::cerr << "getaddrinfo() failed." << std::endl;
		WSACleanup();
		return errorCode;
	}

	sockaddr_in* address = reinterpret_cast<sockaddr_in*>(info->ai_addr);
	char ipBuffer[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(address->sin_addr), ipBuffer, INET_ADDRSTRLEN);
	std::cout << "Server IP Address: " << ipBuffer << "\n";
	std::cout << "Server Port Number: " << portString << "\n";
	std::cout << std::endl;

	// Create and Bind Socket for listening
	SOCKET listenerSocket = socket(
		info->ai_family,
		info->ai_socktype,
		info->ai_protocol);
	if (listenerSocket == INVALID_SOCKET) {
		std::cerr << "socket() failed." << std::endl;
		freeaddrinfo(info);
		WSACleanup();
		return 1;
	}

	errorCode = bind(
		listenerSocket,
		info->ai_addr,
		static_cast<int>(info->ai_addrlen));
	if (errorCode != NO_ERROR) {
		std::cerr << "bind() failed." << std::endl;
		closesocket(listenerSocket);
		listenerSocket = INVALID_SOCKET;
	}

	freeaddrinfo(info);

	if (listenerSocket == INVALID_SOCKET) {
		std::cerr << "bind() failed." << std::endl;
		WSACleanup();
		return 2;
	}

	// Set to listening mode, for 1 client
	errorCode = listen(listenerSocket, SOMAXCONN);
	if (errorCode != NO_ERROR) {
		std::cerr << "listen() failed." << std::endl;
		closesocket(listenerSocket);
		WSACleanup();
		return 3;
	}
}