/******************************************************************************/
/*!
\file		Main.h
\author 	
\par    	
\date   	
\brief		This is where everything starts! It is main, the starting point
					of the application. This source file also defines some global vars.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/


#ifndef ASS4_MAIN_H_
#define ASS4_MAIN_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// ---------------------------------------------------------------------------
// includes

#include "AEEngine.h"
#include "Math.h"

#include "GameStateMgr.h"
#include "GameState_Asteroids.h"

#include "ws2tcpip.h"
#pragma comment(lib, "ws2_32.lib")

#include <string>
#include <iostream>
#include <memory>
#include <thread>
#include <mutex>

struct SERVER_INITIAL_MESSAGE_FORMAT
{
	int ShipID;
};

struct GAME_SCORE {
	int score;
	int live;
	bool isDead;
};

//------------------------------------
// Globals

extern float	g_dt;
extern double	g_appTime;

extern std::string serverIP;
extern std::string serverPort;
extern addrinfo* serverInfo;
extern SOCKET clientSocket;
extern int assignedShipID;
extern GAME_SCORE gameScore;
extern std::mutex GAME_OBJECT_LIST_MUTEX;
extern std::mutex GAME_SCORE_MUTEX;

int WinsockServerConnection();
void ReceiveServerMessages(SOCKET clientSocket);

#endif











