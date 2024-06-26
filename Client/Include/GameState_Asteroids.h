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
#include "Collision.h"
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
const float					SHIP_ROT_SPEED = (2.0f * 3.14159265358979323846f);	// ship rotation speed (degree/second)

const float					BULLET_SIZE = 3.0f;
const float					BULLET_SPEED = 150.0f;				// bullet speed (m/s)

const float					ASTEROID_SIZE = 70.f;
const float					ASTEROID_SPEED = 50.f;
const float         BOUNDING_RECT_SIZE = 1.0f;

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
	AABB				boundingBox;// object bouding box that encapsulates the object
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

};

struct OTHER_OBJ_INFO
{
	int objID;
	int type;
	float				scale;		// scaling value of the object instance
	AEVec2				position;	// object current position
	AEVec2				velCurr;	// object current velocity
	float				dirCurr;	// object current direction

};

struct DEAD_RECK_INFO
{
	bool					m_toInterpolate; //Bool to determine if the obj need to interpolate (for asteroids, bullets)
	AEVec2				CorrectionVec;	// direction where the obj needs to move to dead reckon
	float				  CorrectionRotation;	// direction where the obj needs to rotate to dead reckon
	DEAD_RECK_INFO(bool interpol, AEVec2 v, float r);
	DEAD_RECK_INFO();
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
void SetDeadReckInfo(int id, bool i, AEVec2 v, float rot);
AEVec2 GetObjPos(int id);
float GetObjRot(int id);
int GetShipLive(int id);
void RespawnShip(int id, unsigned long type, float scale, AEVec2* pPos, AEVec2* pVel, float dir);
void SetPackageInterval();
void resetNonGameObjs(int offset);

extern GameObjInst sGameObjInstList[GAME_OBJ_INST_NUM_MAX];

// ---------------------------------------------------------------------------

#endif // ASS4_GAME_STATE_PLAY_H_


