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

SOCKET listenerSocket;
std::vector<sockaddr_in> ClientSocket;
std::mutex GAME_OBJECT_LIST_MUTEX;

void ReceiveClientMessages(SOCKET clientSocket);

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


		int ret{ WinsockServerSetup() };
		if (ret) {
			return ret;
		}

		// Initialize the gamestate
		GameStateInit();

		// Create recieve thread
		std::thread receiveThread(ReceiveClientMessages, listenerSocket);

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
		if (receiveThread.joinable()) {
			receiveThread.join();
		}
	}

	// free the system
	AESysExit();

	return 0;
}

int WinsockServerSetup() {
	HWND hwndConsole = GetConsoleWindow();
	SetForegroundWindow(hwndConsole);

	std::string portString{};
	std::cout << "Server Port Number: ";
	std::cin >> portString;
	std::cout << std::endl;
	//double interval{};
	//std::cout << "Server packet interval: ";
	//std::cin >> interval;
	//std::cout << std::endl;
	//PACKAGE_INTERVAL = interval;

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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
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
	listenerSocket = socket(
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

	// Receive datagrams
	char buffer[1024];
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	int currClient{};
	while (currClient < MAX_CLIENTS) {
		std::cout << "Waiting for Client\n";
		int bytesRead = recvfrom(listenerSocket, buffer, sizeof(buffer), 0,
			reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
		if (bytesRead == SOCKET_ERROR) {
			std::cerr << "recvfrom() failed: " << WSAGetLastError() << std::endl;
			break;
		}
		buffer[bytesRead] = '\0';

		std::cout << "Received datagram: " << buffer << std::endl;

		

		// Send Ship ID to client
		SERVER_INITIAL_MESSAGE_FORMAT toSend{};
		toSend.ShipID = AddNewShip();
		std::cout << "CREATED SHIP: " << toSend.ShipID << "\n";

		int errorCode = sendto(listenerSocket,
			reinterpret_cast<const char*>(&toSend),
			sizeof(SERVER_INITIAL_MESSAGE_FORMAT),
			0,
			reinterpret_cast<sockaddr*>(&clientAddr), 
			clientAddrLen);

		ClientSocket.push_back(clientAddr);
		currClient++;
	
		std::cout << "Added Client\n";
	}

	return 0;
}

void ReceiveClientMessages(SOCKET clientSocket) {
	const float					SHIP_ACCEL_FORWARD = 60.0f;			// ship forward acceleration (in m/s^2)
	const float					SHIP_ACCEL_BACKWARD = 60.0f;		// ship backward acceleration (in m/s^2)
	const float					SHIP_ROT_SPEED = (2.0f * PI);		// ship rotation speed (degree/second)

	while (true) {
		sockaddr_in clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		char buffer[sizeof(CLIENT_MESSAGE_FORMAT)];

		int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
			reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
		if (bytesRead == SOCKET_ERROR) {
			std::cerr << "recvfrom() failed: " << WSAGetLastError() << std::endl;
		}

		CLIENT_MESSAGE_FORMAT recv{ *reinterpret_cast<CLIENT_MESSAGE_FORMAT*>(buffer) };

		std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
		GameObjInst& currShip{ sGameObjInstList[recv.ShipID] };

		if (recv.MessageType == static_cast<int>(MESSAGE_TYPE::TYPE_MOVEMENT_UP)) {
			AEVec2 accel;
			AEVec2Set(&accel, static_cast<f32>(cosf(currShip.dirCurr)),
				static_cast<f32>(sinf(currShip.dirCurr))); //normalized acceleration vector

			if ((currShip.flag & FLAG_ACTIVE) == 0)
				std::cout << "SHIP NULL: " << recv.ShipID << "\n";

			accel = { accel.x * SHIP_ACCEL_FORWARD, accel.y * SHIP_ACCEL_FORWARD }; //full acceleration vector
			currShip.velCurr = { accel.x * static_cast<f32>(AEFrameRateControllerGetFrameTime()) + currShip.velCurr.x,
				accel.y * static_cast<f32>(AEFrameRateControllerGetFrameTime()) + currShip.velCurr.y };
			currShip.velCurr = { currShip.velCurr.x * static_cast<f32>(0.99), currShip.velCurr.y * static_cast<f32>(0.99) };
		}

		if (recv.MessageType == static_cast<int>(MESSAGE_TYPE::TYPE_MOVEMENT_DOWN)) {
			AEVec2 accel;
			AEVec2Set(&accel, static_cast<f32>(-cosf(currShip.dirCurr)), 
				static_cast<f32>(-sinf(currShip.dirCurr))); //normalized acceleration vector
			accel = { accel.x * SHIP_ACCEL_FORWARD, accel.y * SHIP_ACCEL_FORWARD }; //full acceleration vector
			currShip.velCurr = { accel.x * static_cast<f32>(AEFrameRateControllerGetFrameTime()) + currShip.velCurr.x,
				accel.y * static_cast<f32>(AEFrameRateControllerGetFrameTime()) + currShip.velCurr.y };
			currShip.velCurr = { currShip.velCurr.x * static_cast<f32>(0.99), currShip.velCurr.y * static_cast<f32>(0.99) };
		}

		if (recv.MessageType == static_cast<int>(MESSAGE_TYPE::TYPE_MOVEMENT_LEFT)) {
			currShip.dirCurr += SHIP_ROT_SPEED * (float)(AEFrameRateControllerGetFrameTime());
			currShip.dirCurr = AEWrap(currShip.dirCurr, -PI, PI);
		}

		if (recv.MessageType == static_cast<int>(MESSAGE_TYPE::TYPE_MOVEMENT_RIGHT)) {
			currShip.dirCurr -= SHIP_ROT_SPEED * (float)(AEFrameRateControllerGetFrameTime());
			currShip.dirCurr = AEWrap(currShip.dirCurr, -PI, PI);
		}

		if (recv.MessageType == static_cast<int>(MESSAGE_TYPE::TYPE_SHOOT)) {
			AEVec2 vel;
			AEVec2Set(&vel, cosf(currShip.dirCurr), sinf(currShip.dirCurr));
			vel.x = vel.x * BULLET_SPEED;
			vel.y = vel.y * BULLET_SPEED;

			// Create an instance
			FireBullet(recv.ShipID, currShip.posCurr, vel);
		}
	}
}