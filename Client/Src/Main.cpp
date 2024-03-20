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
//#define PrintMessage

// ---------------------------------------------------------------------------
// Globals
float		g_dt;
double	g_appTime;

std::string serverIP;
std::string serverPort;
addrinfo* serverInfo;
SOCKET clientSocket;
int assignedShipID;
std::mutex GAME_OBJECT_LIST_MUTEX;


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




	// Changing the window title
	AESysSetWindowTitle("Asteroids Client");

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

		int ret{ WinsockServerConnection() };
		if (ret) {
			return ret;
		}

		std::thread receiveThread(ReceiveServerMessages, clientSocket);

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

	return 0;
}

int WinsockServerConnection() {
	HWND hwndConsole = GetConsoleWindow();
	SetForegroundWindow(hwndConsole);

	std::cout << "Server IP Address: ";
	std::cin >> serverIP;
	std::cout << std::endl;
	std::cout << "Server Port Number: ";
	std::cin >> serverPort;
	std::cout << std::endl;

	// Start Winsock
	WSADATA wsaData{};
	SecureZeroMemory(&wsaData, sizeof(wsaData));
	int errorCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (errorCode != NO_ERROR) {
		std::cerr << "WSAStartup() failed." << std::endl;
		return errorCode;
	}

	// Get Address Info of Server
	addrinfo hints{};
	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;

	serverInfo = nullptr;
	errorCode = getaddrinfo(serverIP.c_str(), serverPort.c_str(), &hints, &serverInfo);
	if ((errorCode) || (serverInfo == nullptr)) {
		std::cerr << "getaddrinfo() failed." << std::endl;
		WSACleanup();
		return errorCode;
	}

	// Create UDP socket
	clientSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "socket() failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(serverInfo);
		WSACleanup();
		return 2;
	}

	// Send a test message
	const char* message = "Client Connection";
	errorCode = sendto(clientSocket, message, strlen(message), 0, serverInfo->ai_addr, static_cast<int>(serverInfo->ai_addrlen));
	if (errorCode == SOCKET_ERROR) {
		std::cerr << "sendto() failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(serverInfo);
		closesocket(clientSocket);
		WSACleanup();
		return 3;
	}

	std::cout << "Message sent successfully." << std::endl;

	// Receive Ship ID from server
	char buffer[sizeof(SERVER_INITIAL_MESSAGE_FORMAT)];
	sockaddr_in servAddr;
	int servAddrLen = sizeof(servAddr);

	int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
		reinterpret_cast<sockaddr*>(&servAddr), &servAddrLen);
	if (bytesRead == SOCKET_ERROR) {
		std::cerr << "recvfrom() failed: " << WSAGetLastError() << std::endl;
		return 1;
	}

	SERVER_INITIAL_MESSAGE_FORMAT recv{ *reinterpret_cast<SERVER_INITIAL_MESSAGE_FORMAT*>(buffer) };
	assignedShipID = recv.ShipID;

	std::cout << "Assigned ID: " << assignedShipID << std::endl;

	return 0;
}

void ReceiveServerMessages(SOCKET clientSocket) {
	while (true) {
#ifdef PrintMessage
		std::cout << "------------------------\n";
#endif

		char buffer[100000];

		sockaddr_in servAddr;
		int servAddrLen = sizeof(servAddr);

		int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
			reinterpret_cast<sockaddr*>(&servAddr), &servAddrLen);
		if (bytesRead == SOCKET_ERROR) {
			std::cerr << "recvfrom() failed: " << WSAGetLastError() << std::endl;
		}

		int numOfShips{};
		int  numOfOtherObj{};
		memcpy(&numOfShips, &buffer[0], sizeof(int));
		memcpy(&numOfOtherObj, &buffer[sizeof(uint32_t)], sizeof(uint32_t));
#ifdef PrintMessage
		std::cout << "numOfShips: " << static_cast<int>(numOfShips) << "\n";
#endif
		SHIP_OBJ_INFO shipInfo{};
		OTHER_OBJ_INFO otherObj{};
		for (int i = 0; i < numOfShips; ++i)
		{
			memcpy(&shipInfo, &buffer[8 + (i*sizeof(SHIP_OBJ_INFO))], sizeof(SHIP_OBJ_INFO));

			std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
			gameObjInstSet(shipInfo.shipID,TYPE_SHIP, SHIP_SIZE, &shipInfo.position, nullptr, shipInfo.dirCurr);
			//sGameObjInstList[shipInfo.shipID].posCurr = shipInfo.position;
			//sGameObjInstList[shipInfo.shipID].dirCurr = shipInfo.dirCurr;
		
#ifdef PrintMessage
			std::cout << "Ship " << shipInfo.shipID << "\n";
			std::cout << "Ship score" << shipInfo.score << "\n";
			std::cout << "Ship live" << shipInfo.live << "\n";
			std::cout << "Ship pos" << shipInfo.position.x << "," << shipInfo.position.y << "\n";
			std::cout << "Ship dir" << shipInfo.dirCurr << "\n\n";
#endif
		}

		for (int i = 0; i < numOfOtherObj; ++i)
		{
			memcpy(&otherObj, &buffer[8 + (numOfShips * sizeof(SHIP_OBJ_INFO)) + (i*sizeof(OTHER_OBJ_INFO))], sizeof(OTHER_OBJ_INFO));
			std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
			if (otherObj.objID == GAMEOBJ_TYPE::TYPE_BULLET)
				gameObjInstSet(otherObj.objID, otherObj.objtype, BULLET_SIZE, &otherObj.position, nullptr, otherObj.dirCurr);
			else 
				gameObjInstSet(otherObj.objID, otherObj.objtype, SHIP_SIZE, &otherObj.position, nullptr, otherObj.dirCurr);
			//sGameObjInstList[otherObj.objID].posCurr = otherObj.position;
			//sGameObjInstList[otherObj.objID].dirCurr = otherObj.dirCurr;
			//sGameObjInstList[otherObj.objID].flag = 1;
#ifdef PrintMessage
			std::cout << "Obj " << otherObj.objID << "\n";
			std::cout << "Obj pos" << otherObj.position.x << "," << shipInfo.position.y << "\n";
			std::cout << "Obj dir" << otherObj.dirCurr << "\n\n";
#endif

		}

#ifdef PrintMessage
		std::cout << "------------------------\n\n";
#endif
	//std::cout << "Object ID: " << recv.ObjectID << " Position: " << recv.position.x << " " << recv.position.y << "\n";

	//std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
	//sGameObjInstList[recv.ObjectID].posCurr = recv.position;
	//sGameObjInstList[recv.ObjectID].dirCurr = recv.dirCurr;


		//char buffer[sizeof(SERVER_MESSAGE_FORMAT)];
		//sockaddr_in servAddr;
		//int servAddrLen = sizeof(servAddr);

		//int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
		//	reinterpret_cast<sockaddr*>(&servAddr), &servAddrLen);
		//if (bytesRead == SOCKET_ERROR) {
		//	std::cerr << "recvfrom() failed: " << WSAGetLastError() << std::endl;
		//}

		//SERVER_MESSAGE_FORMAT recv{ *reinterpret_cast<SERVER_MESSAGE_FORMAT*>(buffer) };
		////std::cout << "Object ID: " << recv.ObjectID << " Position: " << recv.position.x << " " << recv.position.y << "\n";

		//std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
		//sGameObjInstList[recv.ObjectID].posCurr = recv.position;
		//sGameObjInstList[recv.ObjectID].dirCurr = recv.dirCurr;
	}
}
