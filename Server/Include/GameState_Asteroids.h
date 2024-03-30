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

#include "main.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>
#include "Collision.h"

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

//Game object instance structure
struct GameObjInst
{
	GameObj* pObject;	// pointer to the 'original' shape
	unsigned long		flag;		// bit flag or-ed together
	float				scale;		// scaling value of the object instance
	AEVec2				posCurr;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction
	AABB				boundingBox;// object bouding box that encapsulates the object
	AEMtx33				transform;	// object transformation matrix: Each frame, 
	int					fromShipIdx;
	// calculate the object instance's transformation matrix and save it here
};

/******************************************************************************/
/*!
	Defines
*/
/******************************************************************************/
const unsigned int	GAME_OBJ_NUM_MAX = 32;					// The total number of different objects (Shapes)
const unsigned int	GAME_OBJ_INST_NUM_MAX = 2048;		// The total number of different game object instances
const unsigned long FLAG_ACTIVE = 0x00000001;

const unsigned int	SHIP_INITIAL_NUM = 3;						// initial number of ship lives
const float					SHIP_SIZE = 16.0f;							// ship size
const float					SHIP_ACCEL_FORWARD = 60.0f;			// ship forward acceleration (in m/s^2)
const float					SHIP_ACCEL_BACKWARD = 60.0f;		// ship backward acceleration (in m/s^2)
const float					SHIP_ROT_SPEED = (2.0f * PI);		// ship rotation speed (degree/second)

const float					BULLET_SPEED = 150.0f;					// bullet speed (m/s)
const float					BULLET_SIZE = 3.0f;

const float					ASTEROID_SIZE = 70.f;
const float					ASTEROID_SPEED = 50.f;

const float					BOUNDING_RECT_SIZE = 1.0f;      // this is the normalized bounding rectangle (width and height) sizes - AABB collision data
extern double				PACKAGE_INTERVAL;					  // How often (secs) will the server send packages to all the clients 

enum MESSAGE_TYPE
{
	// list of possible message types
	TYPE_MOVEMENT_UP,
	TYPE_MOVEMENT_DOWN,
	TYPE_MOVEMENT_LEFT,
	TYPE_MOVEMENT_RIGHT,
	TYPE_SHOOT
};

struct SHIP_OBJ
{
	int objectID;
	int shipLive;
	int score;
	bool isDead;
};


struct SHIP_OBJ_INFO
{
	int dead;
	int shipID;
	int score;
	int live;
	float				scale;		// scaling value of the object instance
	AEVec2				position;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction
	SHIP_OBJ_INFO(int ded, int sid, int s, int l, float sc, AEVec2 p, AEVec2 v, float d);
};

struct OTHER_OBJ_INFO
{
	int objID;
	int type;
	float				scale;		// scaling value of the object instance
	AEVec2				position;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction
	OTHER_OBJ_INFO(int oid, int t, float s, AEVec2 p, AEVec2 v, float d);
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
int AddNewShip();
int FireBullet(int shipid, AEVec2& pos, AEVec2& vel);
void gameObjInstSet(int id, unsigned long type, float scale, AEVec2* pPos, AEVec2* pVel, float dir);
extern GameObjInst sGameObjInstList[GAME_OBJ_INST_NUM_MAX];


// ---------------------------------------------------------------------------

#endif // ASS4_GAME_STATE_PLAY_H_


