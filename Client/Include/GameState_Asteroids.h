/******************************************************************************/
/*!
\file			GameState_Asteroids.h
\author 	
\par    	
\date   	
\brief		This is the level source file that has the main level functions
					load, init, update, draw, free, unload.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ASS4_GAME_STATE_PLAY_H_
#define ASS4_GAME_STATE_PLAY_H_

enum MESSAGE_TYPE
{
	// list of possible message types
	TYPE_MOVEMENT_UP,
	TYPE_MOVEMENT_DOWN,
	TYPE_MOVEMENT_LEFT,
	TYPE_MOVEMENT_RIGHT,
	TYPE_SHOOT
};

struct CLIENT_MESSAGE_FORMAT
{
	int ShipID;
	int MessageType;
};

// ---------------------------------------------------------------------------

void GameStateAsteroidsLoad(void);
void GameStateAsteroidsInit(void);
void GameStateAsteroidsUpdate(void);
void GameStateAsteroidsDraw(void);
void GameStateAsteroidsFree(void);
void GameStateAsteroidsUnload(void);

void SendEventToServer(int shipID, MESSAGE_TYPE messageType);

// ---------------------------------------------------------------------------

#endif // ASS4_GAME_STATE_PLAY_H_


