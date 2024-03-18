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

//------------------------------------
// Globals

extern float	g_dt;
extern double	g_appTime;

extern std::string serverIP;
extern std::string serverPort;

int WinsockServerConnection();

#endif











