/******************************************************************************/
/*!
\file			GameState_Asteroids.cpp
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

#include "GameState_Asteroids.h"
#include <random>

int currentAliveObjects{};


// -----------------------------------------------------------------------------
enum TYPE
{
	// list of game object types
	TYPE_SHIP = 0, 
	TYPE_BULLET,
	TYPE_ASTEROID,

	TYPE_NUM
};

// -----------------------------------------------------------------------------
// object flag definition



/******************************************************************************/
/*!
	Static Variables
*/
/******************************************************************************/

// list of original object
static std::vector<SHIP_OBJ> allShipInfo{};
static std::vector<GameObjInst*> allOtherObjsInfo{};
static GameObj				sGameObjList[GAME_OBJ_NUM_MAX];				// Each element in this array represents a unique game object (shape)
static unsigned long		sGameObjNum;								// The number of defined game objects

// list of object instances
GameObjInst			sGameObjInstList[GAME_OBJ_INST_NUM_MAX];	// Each element in this array represents a unique game object instance (sprite)
static unsigned long		sGameObjInstNum;							// The number of used game object instances

// pointer to the ship object
static GameObjInst *		spShip;										// Pointer to the "Ship" game object instance

// number of ship available (lives 0 = game over)
static long					sShipLives;									// The number of lives left

// the score = number of asteroid destroyed
static unsigned long		sScore;										// Current score

static bool onValueChange = true;


SHIP_OBJ_INFO::SHIP_OBJ_INFO(int sid, int s, int l, float sc, AEVec2 p, AEVec2 v, float d) :shipID{ sid }, score{ s }, live{ l }, scale{ sc }, position{ p }, velCurr{v}, dirCurr { d } {}

OTHER_OBJ_INFO::OTHER_OBJ_INFO(int oid, int t, float s, AEVec2 p, AEVec2 v, float d) : objID{ oid }, type{ t }, scale{ s }, position{ p }, velCurr{ v }, dirCurr{ d } {}
// ---------------------------------------------------------------------------

// functions to create/destroy a game object instance
GameObjInst *		gameObjInstCreate (unsigned long type, float scale, 
											   AEVec2 * pPos, AEVec2 * pVel, float dir);
void					gameObjInstDestroy(GameObjInst * pInst);


/******************************************************************************/
/*!
	"Load" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsLoad(void)
{
	// zero the game object array
	memset(sGameObjList, 0, sizeof(GameObj) * GAME_OBJ_NUM_MAX);
	// No game objects (shapes) at this point
	sGameObjNum = 0;

	// zero the game object instance array
	memset(sGameObjInstList, 0, sizeof(GameObjInst) * GAME_OBJ_INST_NUM_MAX);
	// No game object instances (sprites) at this point
	sGameObjInstNum = 0;

	// The ship object instance hasn't been created yet, so this "spShip" pointer is initialized to 0
	spShip = nullptr;

	// load/create the mesh data (game objects / Shapes)
	GameObj * pObj;

	// =====================
	// create the ship shape
	// =====================

	pObj		= sGameObjList + sGameObjNum++;
	pObj->type	= TYPE_SHIP;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f,  0.5f, 0xFFFF0000, 0.0f, 0.0f, 
		-0.5f, -0.5f, 0xFFFF0000, 0.0f, 0.0f,
		 0.5f,  0.0f, 0xFFFFFFFF, 0.0f, 0.0f );  

	pObj->pMesh = AEGfxMeshEnd();//saves triangles into pMesh
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");


	// =======================
	// create the bullet shape
	// =======================
	pObj = sGameObjList + sGameObjNum++;
	pObj->type = TYPE_BULLET;
	
	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFF00FF, 0.0f, 1.0f,
		0.5f, -0.5f, 0xFFFFFF00, 1.0f, 1.0f,
		-0.5f, 0.5f, 0xFF00FFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
	pObj->pMesh = AEGfxMeshEnd();//saves triangles into pMesh
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");

	// =========================
	// create the asteroid shape
	// =========================
	pObj = sGameObjList + sGameObjNum++;
	pObj->type = TYPE_ASTEROID;

	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
	pObj->pMesh = AEGfxMeshEnd(); //saves triangles into pMesh
	AE_ASSERT_MESG(pObj->pMesh, "fail to create object!!");
	
}

/******************************************************************************/
/*!
	"Initialize" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsInit(void)
{
	// create the main ship

	//// Ship ID 0
/*	spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, nullptr, nullptr, 0.0f);
	AE_ASSERT(spShip);*/	
	//currentAliveObjects++;
	//
	// CREATE THE INITIAL ASTEROIDS INSTANCES USING THE "gameObjInstCreate" FUNCTION
	
	AEVec2 asteroidVelocity;
	AEVec2 asteroidPos;
	std::random_device rd;
	std::mt19937 gen(rd());

	// Create a uniform distribution for floats between 0 and 2π
	std::uniform_real_distribution<float> dis(0.0f, 2.0f * 3.14159265358979323846f);
	for (int i = 0; i < 4; i++) {
		// Generate a random float between 0 and 2π
		float asteroidDir = dis(gen);
		asteroidVelocity.x = cosf(asteroidDir) * ASTEROID_SPEED;
		asteroidVelocity.y = sinf(asteroidDir) * ASTEROID_SPEED;
		asteroidPos = { AEGfxGetWinMinX() - 50.0f, ((std::rand() % static_cast<int>((AEGfxGetWinMinY() - AEGfxGetWinMaxY() + 1)) + AEGfxGetWinMinY() ))};
		auto goptr = gameObjInstCreate(TYPE_ASTEROID, ASTEROID_SIZE, &asteroidPos, &asteroidVelocity, 0.0f);
		allOtherObjsInfo.push_back(goptr);
	}

	// Creates initial bullet instance
	//GameObjInst * bullet = gameObjInstCreate(TYPE_BULLET, 0, nullptr, nullptr, 0.0f);
	//gameObjInstDestroy(bullet);
	

	// reset the score and the number of ships
	//sScore      = 0;
	//sShipLives  = 0;
}


int AddNewShip()
{
	std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
	// Add a new SHip
	GameObjInst* newShipInst = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, nullptr, nullptr, 0.0f);	
	AE_ASSERT(newShipInst);
	currentAliveObjects++;

	unsigned int shipID = newShipInst - sGameObjInstList;
	SHIP_OBJ newShipData{};
	newShipData.objectID = static_cast<int>(shipID);
	newShipData.shipLive = 3;
	allShipInfo.push_back(newShipData);
	return static_cast<int>(shipID);

 //reset the score and the number of ship
}

int FireBullet(AEVec2& pos, AEVec2& vel)
{
	GameObjInst* newBulletInst = gameObjInstCreate(TYPE_BULLET, BULLET_SIZE, &pos, &vel, 0.0f);
	AE_ASSERT(newBulletInst);
	currentAliveObjects++;

	unsigned int bulletID = newBulletInst - sGameObjInstList;
	GameObjInst* newBulletData{};
	allOtherObjsInfo.push_back(newBulletInst);

	return static_cast<int>(bulletID);
}


/******************************************************************************/
/*!
	"Update" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsUpdate(void)
{
	// =========================
	// receive from client
	// =========================
	// Done in main

	// =========================
	// update according to input
	// =========================
	// Done in main172.28.80.1
	std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);

	// ======================================================
	// update physics of all active game object instances
	//  -- Get the AABB bounding rectangle of every active instance:
	//		boundingRect_min = -(BOUNDING_RECT_SIZE/2.0f) * instance->scale + instance->pos
	//		boundingRect_max = +(BOUNDING_RECT_SIZE/2.0f) * instance->scale + instance->pos
	//
	//	-- Positions of the instances are updated here with the already computed velocity (above)
	// ======================================================

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{

		GameObjInst* pInst = sGameObjInstList + i;

		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		pInst->boundingBox.min.x = pInst->posCurr.x - (((BOUNDING_RECT_SIZE / 2.0f) * pInst->scale));
		pInst->boundingBox.min.y = pInst->posCurr.y - (((BOUNDING_RECT_SIZE / 2.0f) * pInst->scale));

		pInst->boundingBox.max.x = pInst->posCurr.x + (((BOUNDING_RECT_SIZE / 2.0f) * pInst->scale));
		pInst->boundingBox.max.y = pInst->posCurr.y + (((BOUNDING_RECT_SIZE / 2.0f) * pInst->scale));

		pInst->posCurr = { pInst->velCurr.x * static_cast<f32>(AEFrameRateControllerGetFrameTime()) + pInst->posCurr.x,
			pInst->velCurr.y * static_cast<f32>(AEFrameRateControllerGetFrameTime()) + pInst->posCurr.y };

		if (sScore >= 5000) {
			if (pInst->pObject->type == TYPE_SHIP) {
				//Reset Ship Position
				AEVec2 zero = { 0,0 };
				pInst->velCurr = zero;
				pInst->posCurr = zero;
			}
		}
	}
	
	// ====================
	// check for collision
	// ====================
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		if (pInst->pObject->type == TYPE_ASTEROID) {
			for (unsigned long x = 0; x < GAME_OBJ_INST_NUM_MAX; x++)
			{
				GameObjInst* pInst2 = sGameObjInstList + x;
				if ((pInst2->flag & FLAG_ACTIVE) == 0)
					continue;

				if (pInst2->pObject->type == TYPE_SHIP) {		
					if (CollisionIntersection_RectRect(pInst->boundingBox, pInst->velCurr, pInst2->boundingBox, pInst2->velCurr)) {
						//gameObjInstDestroy(pInst);

						//Reset Ship Position
						AEVec2 zero = { 0,0 };
						pInst2->velCurr = zero;
						pInst2->posCurr = zero;

						onValueChange = true;
					}
				}
				if (pInst2->pObject->type == TYPE_BULLET) {
					if (CollisionIntersection_RectRect(pInst->boundingBox, pInst->velCurr, pInst2->boundingBox, pInst2->velCurr)) {
						//gameObjInstDestroy(pInst);
						AEVec2 asteroidVelocity;
						AEVec2 asteroidPos;
						std::random_device rd;
						std::mt19937 gen(rd());

						// Create a uniform distribution for floats between 0 and 2π
						std::uniform_real_distribution<float> dis(0.0f, 2.0f * 3.14159265358979323846f);

						float asteroidDir = dis(gen);
						asteroidVelocity.x = cosf(asteroidDir) * ASTEROID_SPEED;
						asteroidVelocity.y = sinf(asteroidDir) * ASTEROID_SPEED;
						asteroidPos = { AEGfxGetWinMinX() - 50.0f, ((std::rand() % static_cast<int>((AEGfxGetWinMinY() - AEGfxGetWinMaxY() + 1)) + AEGfxGetWinMinY())) };
						gameObjInstSet(i, TYPE_ASTEROID, ASTEROID_SIZE, &asteroidPos, &asteroidVelocity, 0.0f);

						//Reset Ship Position
						AEVec2 zero = { 0,0 };
						pInst2->velCurr = zero;
						pInst2->posCurr = zero;

						onValueChange = true;
					}
				}
			}
		}
	}

	// ===================================
	// update active game object instances
	// Example:
	//		-- Wrap specific object instances around the world (Needed for the assignment)
	//		-- Removing the bullets as they go out of bounds (Needed for the assignment)
	//		-- If you have a homing missile for example, compute its new orientation 
	//			(Homing missiles are not required for the Asteroids project)
	//		-- Update a particle effect (Not required for the Asteroids project)
	// ===================================
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// check if the object is a ship
		if (pInst->pObject->type == TYPE_SHIP)
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, AEGfxGetWinMinX() - SHIP_SIZE, 
														AEGfxGetWinMaxX() + SHIP_SIZE);
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, AEGfxGetWinMinY() - SHIP_SIZE, 
														AEGfxGetWinMaxY() + SHIP_SIZE);
		}

		// Wrap asteroids here
		if (pInst->pObject->type == TYPE_ASTEROID)
		{
			// warp the ship from one end of the screen to the other
			pInst->posCurr.x = AEWrap(pInst->posCurr.x, AEGfxGetWinMinX() - (BOUNDING_RECT_SIZE * pInst->scale),
				AEGfxGetWinMaxX() + (BOUNDING_RECT_SIZE * pInst->scale));
			pInst->posCurr.y = AEWrap(pInst->posCurr.y, AEGfxGetWinMinY() - (BOUNDING_RECT_SIZE * pInst->scale),
				AEGfxGetWinMaxY() + (BOUNDING_RECT_SIZE * pInst->scale));
		}

		// Remove bullets that go out of bounds
		if (pInst->pObject->type == TYPE_BULLET) {
			if (pInst->posCurr.x < AEGfxGetWinMinX() || pInst->posCurr.x > AEGfxGetWinMaxX() || pInst->posCurr.y > AEGfxGetWinMaxY() || pInst->posCurr.y < AEGfxGetWinMinY()) {
				gameObjInstDestroy(pInst);
				auto it = std::find(allOtherObjsInfo.begin(), allOtherObjsInfo.end(), pInst);

				// Check if the element was found
				if (it != allOtherObjsInfo.end()) {
					// Erase the element from the vector
					allOtherObjsInfo.erase(it);
				}
			}
		}
	}
	
	// =====================================
	// calculate the matrix for all objects
	// =====================================

	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;
		AEMtx33		 trans = {0}, rot = {0}, scale = {0};

		UNREFERENCED_PARAMETER(trans);
		UNREFERENCED_PARAMETER(rot);
		UNREFERENCED_PARAMETER(scale);

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;

		// Compute the scaling matrix
		// Compute the rotation matrix 
		// Compute the translation matrix
		// Concatenate the 3 matrix in the correct order in the object instance's "transform" matrix
		AEMtx33Scale(&scale, pInst->scale, pInst->scale);

		AEMtx33Rot(&rot, pInst->dirCurr);

		AEMtx33Trans(&trans, pInst->posCurr.x, pInst->posCurr.y);

		// Concat the matrices (TRS)
		AEMtx33Concat(&pInst->transform, &rot, &scale);
		AEMtx33Concat(&pInst->transform, &trans, &pInst->transform);
	}

	// ========================================
	// send new position information to clients
	// ========================================
	// Send Position Info to client

	//Generate the Message 
	//static std::vector<SHIP_OBJ> allShipInfo{};
	//static std::vector<GameObj*> allOtherObjsInfo{};
	std::vector<SHIP_OBJ_INFO> shipMsg{};
	std::vector<OTHER_OBJ_INFO> otherObjMsg{};
	int numofShips{};
	int numofObjs{ static_cast<int>(allOtherObjsInfo.size()) };
	//std::cout << allShipInfo.size() << "::num Of Ships Created\n";

	for (const SHIP_OBJ& s : allShipInfo)
	{
		if (s.isDead)
			continue;
		++numofShips;
		shipMsg.emplace_back(
			s.objectID, 
			s.score,
			s.shipLive,
			sGameObjInstList[s.objectID].scale,
			sGameObjInstList[s.objectID].posCurr, 
			sGameObjInstList[s.objectID].velCurr,
			sGameObjInstList[s.objectID].dirCurr);
	}

	for (GameObjInst* o : allOtherObjsInfo)
	{
		otherObjMsg.emplace_back(
			o-sGameObjInstList, 
			o->pObject->type,  
			o->scale,
			o->posCurr,
			o->velCurr,
			o->dirCurr);
	}

	size_t sizeNeeded = (2 * sizeof(int)) + (shipMsg.size() * sizeof(SHIP_OBJ_INFO)) + (otherObjMsg.size() * sizeof(OTHER_OBJ_INFO));
	std::string text(sizeNeeded, ' ');
	memcpy(&text[0], &numofShips, sizeof(int));
	memcpy(&text[sizeof(int)], &numofObjs, sizeof(int));
	for (int x{ 0 }; x < numofShips; ++x)
	{
		memcpy(&text[(2 * sizeof(int)) + (x* sizeof(SHIP_OBJ_INFO))], &shipMsg[x], sizeof(SHIP_OBJ_INFO));
	}

	for (int x{ 0 }; x < numofObjs; ++x)
	{
		memcpy(&text[(2 * sizeof(int)) + (shipMsg.size() * sizeof(SHIP_OBJ_INFO)) + (x * sizeof(OTHER_OBJ_INFO))], &otherObjMsg[x], sizeof(OTHER_OBJ_INFO));
	}


	for (size_t i{0};i<ClientSocket.size();++i)
	{
		int clientAddrLen = sizeof(ClientSocket[i]);
		int errorCode = sendto(listenerSocket,
			text.c_str(),
			static_cast<int>(text.size()),
			0,
			reinterpret_cast<sockaddr*>(&ClientSocket[i]),
			clientAddrLen);

		if (errorCode == SOCKET_ERROR) {
			std::cerr << "sendto() failed: " << WSAGetLastError() << std::endl;
		}
	}

	/*for (int x{}; x < MAX_CLIENTS; ++x) {
		for (int i{}; i < currentAliveObjects; ++i) {
			int clientAddrLen = sizeof(ClientSocket[i]);
			SERVER_MESSAGE_FORMAT toSend{};
			toSend.ObjectID = i;
			toSend.position = sGameObjInstList[i].posCurr;
			toSend.dirCurr = sGameObjInstList[i].dirCurr;

		
		}
	}*/
}

/******************************************************************************/
/*!
	Draws objects so that they can be seen on screen
*/
/******************************************************************************/

// TO REMOVE AS SERVER SHOULDNT DRAW
void GameStateAsteroidsDraw(void)
{
	char strBuffer[1024];
	
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0, 0);

	// draw all object instances in the list
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// skip non-active object
		if ((pInst->flag & FLAG_ACTIVE) == 0)
			continue;
		
		// Set the current object instance's transform matrix using "AEGfxSetTransform"
		AEGfxSetTransform(pInst->transform.m);
		// Draw the shape used by the current object instance using "AEGfxMeshDraw"
		AEGfxMeshDraw(pInst->pObject->pMesh, AE_GFX_MDM_TRIANGLES);
	}

	//You can replace this condition/variable by your own data.
	//The idea is to display any of these variables/strings whenever a change in their value happens
	//static bool onValueChange = true;
	/*
	if(onValueChange)
	{
		sprintf_s(strBuffer, "Score: %d", sScore);
		//AEGfxPrint(10, 10, (u32)-1, strBuffer);
		printf("%s \n", strBuffer);

		sprintf_s(strBuffer, "Ship Left: %d", sShipLives >= 0 ? sShipLives : 0);
		//AEGfxPrint(600, 10, (u32)-1, strBuffer);
		printf("%s \n", strBuffer);

		// display the game over message
		if (sShipLives < 0)
		{
			//AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");
			printf("       GAME OVER       \n");
		}

		if (sScore >= 5000) {
			printf("You Rock \n");
		}

		onValueChange = false;
	}
	*/
}

/******************************************************************************/
/*!
	Frees gameObjects using the gameObjInstDestroy function
*/
/******************************************************************************/
void GameStateAsteroidsFree(void)
{
	// kill all object instances in the array using "gameObjInstDestroy"
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		gameObjInstDestroy(pInst);
	}
}

/******************************************************************************/
/*!
	Frees mesh data in order to prevent memory leaks
*/
/******************************************************************************/
void GameStateAsteroidsUnload(void)
{
	// free all mesh data (shapes) of each object using "AEGfxTriFree"
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;

		if (pInst->pObject != nullptr) {
			AEGfxMeshFree(pInst->pObject->pMesh);
			break;
		}
	}	
}


void gameObjInstSet(int id, unsigned long type,
	float scale,
	AEVec2* pPos,
	AEVec2* pVel,
	float dir)
{

	AEVec2 zero;
	AEVec2Zero(&zero);

	GameObjInst* pInst = sGameObjInstList + id;

	// check if current instance is not used
	// it is not used => use it to create the new instance
	pInst->pObject = sGameObjList + type;
	pInst->flag = FLAG_ACTIVE;
	pInst->scale = scale;
	pInst->posCurr = pPos ? *pPos : zero;
	pInst->velCurr = pVel ? *pVel : zero;
	pInst->dirCurr = dir;

	if (pInst->pObject == nullptr)
		std::cout << "ISNULL\n";
}

/******************************************************************************/
/*!
	Creates game objects
*/
/******************************************************************************/
GameObjInst * gameObjInstCreate(unsigned long type, 
							   float scale, 
							   AEVec2 * pPos, 
							   AEVec2 * pVel, 
							   float dir)
{
	AEVec2 zero;
	AEVec2Zero(&zero);
	std::cout << type << "\n";
	//AE_ASSERT_PARM(type < sGameObjNum);
	
	// loop through the object instance list to find a non-used object instance
	for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst * pInst = sGameObjInstList + i;

		// check if current instance is not used
		if (pInst->flag == 0)
		{
			// it is not used => use it to create the new instance
			pInst->pObject	= sGameObjList + type;
			pInst->flag		= FLAG_ACTIVE;
			pInst->scale	= scale;
			pInst->posCurr	= pPos ? *pPos : zero;
			pInst->velCurr	= pVel ? *pVel : zero;
			pInst->dirCurr	= dir;
			
			// return the newly created instance
			return pInst;
		}
	}

	// cannot find empty slot => return 0
	return 0;
}

/******************************************************************************/
/*!
	Destroys game ojects
*/
/******************************************************************************/
void gameObjInstDestroy(GameObjInst * pInst)
{
	// if instance is destroyed before, just return
	if (pInst->flag == 0)
		return;

	// zero out the flag
	pInst->flag = 0;
}