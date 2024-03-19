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

/******************************************************************************/
/*!
	Defines
*/
/******************************************************************************/
const unsigned int	GAME_OBJ_NUM_MAX = 32;						// The total number of different objects (Shapes)
const unsigned int	GAME_OBJ_INST_NUM_MAX = 2048;					// The total number of different game object instances


const unsigned int	SHIP_INITIAL_NUM = 3;						// initial number of ship lives
const float					SHIP_SIZE = 16.0f;				// ship size
const float					SHIP_ACCEL_FORWARD = 60.0f;				// ship forward acceleration (in m/s^2)
const float					SHIP_ACCEL_BACKWARD = 60.0f;				// ship backward acceleration (in m/s^2)
const float					SHIP_ROT_SPEED = (2.0f * PI);	// ship rotation speed (degree/second)

const float					BULLET_SPEED = 150.0f;				// bullet speed (m/s)

/******************************************************************************/
/*!
	Struct/Class Definitions
*/
/******************************************************************************/

//Game object structure
struct GameObj
{
	unsigned long		type;		// object type
	AEGfxVertexList* pMesh;		// This will hold the triangles which will form the shape of the object
};

// ---------------------------------------------------------------------------

//Game object instance structure
struct GameObjInst
{
	GameObj* pObject;	// pointer to the 'original' shape
	unsigned long		flag;		// bit flag or-ed together
	float				scale;		// scaling value of the object instance
	AEVec2				posCurr;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction
	AEMtx33				transform;	// object transformation matrix: Each frame, 
	// calculate the object instance's transformation matrix and save it here
};

enum TYPE
{
	// list of game object types
	TYPE_SHIP = 0,
	TYPE_BULLET,
	TYPE_ASTEROID,
	TYPE_NUM
};


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

struct SERVER_MESSAGE_FORMAT
{
	int ObjectID;
	AEVec2 position;
	float dirCurr;
};

struct SHIP_OBJ_INFO
{
	int shipID;
	int score;
	int live;
	AEVec2 position;
	float dirCurr;
};

struct OTHER_OBJ_INFO
{
	int objID;
	AEVec2 position;
	float dirCurr;
};


// ---------------------------------------------------------------------------

void GameStateAsteroidsLoad(void);
void GameStateAsteroidsInit(void);
void GameStateAsteroidsUpdate(void);
void GameStateAsteroidsDraw(void);
void GameStateAsteroidsFree(void);
void GameStateAsteroidsUnload(void);

void SendEventToServer(int shipID, MESSAGE_TYPE messageType);
void gameObjInstSet(int id, unsigned long type, float scale, AEVec2* pPos, AEVec2* pVel, float dir);

extern GameObjInst sGameObjInstList[GAME_OBJ_INST_NUM_MAX];

// ---------------------------------------------------------------------------

#endif // ASS4_GAME_STATE_PLAY_H_


