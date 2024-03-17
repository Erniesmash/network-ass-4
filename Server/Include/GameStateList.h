/******************************************************************************/
/*!
\file			GameStateList.h
\author 	
\par    	
\date   	
\brief		This is the GameStateList header file that simply defines the
					enum for the various states in the game.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef ASS4_GAME_STATE_LIST_H_
#define ASS4_GAME_STATE_LIST_H_

// ---------------------------------------------------------------------------
// game state list

enum
{
	// list of all game states 
	GS_ASTEROIDS = 0, 
	
	// special game state. Do not change
	GS_RESTART,
	GS_QUIT, 
	GS_NONE
};

// ---------------------------------------------------------------------------

#endif // ASS4_GAME_STATE_LIST_H_