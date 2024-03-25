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

#include "main.h"
#include "Collision.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// object flag definition

const unsigned long FLAG_ACTIVE				= 0x00000001;

/******************************************************************************/
/*!
	Static Variables
*/
/******************************************************************************/

// list of original object
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



// ---------------------------------------------------------------------------

// functions to create/destroy a game object instance
GameObjInst *		gameObjInstCreate (unsigned long type, float scale, 
											   AEVec2 * pPos, AEVec2 * pVel, float dir);
void					gameObjInstDestroy(GameObjInst * pInst);


s8 fontid;
/******************************************************************************/
/*!
	"Load" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsLoad(void)
{
	fontid = AEGfxCreateFont("../Resources/Fonts/Arial Italic.ttf", 20);

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
	// Create the main ship
	// Ship ID 0
	//spShip = gameObjInstCreate(TYPE_SHIP, SHIP_SIZE, nullptr, nullptr, 0.0f);
	//AE_ASSERT(spShip);
	//
	// Create astroids

	// Creates initial bullet instance
	//GameObjInst * bullet = gameObjInstCreate(TYPE_BULLET, 0, nullptr, nullptr, 0.0f);
	//gameObjInstDestroy(bullet);
	
	// reset the score and the number of ships
	//sScore      = 0;
	//sShipLives  = SHIP_INITIAL_NUM;
}

/******************************************************************************/
/*!
	"Update" function of this state
*/
/******************************************************************************/
void GameStateAsteroidsUpdate(void)
{

	// =========================================
	// send message to server according to input
	// =========================================
	{
		std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
		// ====================
		// check for collision
		// ====================
		for (unsigned long i = 0; i < GAME_OBJ_INST_NUM_MAX; i++)
		{
			GameObjInst* pInst = sGameObjInstList + i;

			if ((pInst->flag & FLAG_ACTIVE) == 0)
				continue;

			pInst->boundingBox.min.x = pInst->posCurr.x - (((1.f / 2.0f) * pInst->scale));
			pInst->boundingBox.min.y = pInst->posCurr.y - (((1.f / 2.0f) * pInst->scale));

			pInst->boundingBox.max.x = pInst->posCurr.x + (((1.f / 2.0f) * pInst->scale));
			pInst->boundingBox.max.y = pInst->posCurr.y + (((1.f / 2.0f) * pInst->scale));
			if (pInst->pObject->type == TYPE_BULLET) {
				if (pInst->posCurr.x < AEGfxGetWinMinX() || pInst->posCurr.x > AEGfxGetWinMaxX() || pInst->posCurr.y > AEGfxGetWinMaxY() || pInst->posCurr.y < AEGfxGetWinMinY()) {
					gameObjInstDestroy(pInst);
				}
			}
			if (pInst->pObject->type == TYPE_ASTEROID) {
				for (unsigned long x = 0; x < GAME_OBJ_INST_NUM_MAX; x++)
				{
					GameObjInst* pInst2 = sGameObjInstList + x;
					if ((pInst2->flag & FLAG_ACTIVE) == 0)
						continue;


					pInst2->boundingBox.min.x = pInst2->posCurr.x - (((1.f / 2.0f) * pInst2->scale));
					pInst2->boundingBox.min.y = pInst2->posCurr.y - (((1.f / 2.0f) * pInst2->scale));

					pInst2->boundingBox.max.x = pInst2->posCurr.x + (((1.f / 2.0f) * pInst2->scale));
					pInst2->boundingBox.max.y = pInst2->posCurr.y + (((1.f / 2.0f) * pInst2->scale));

					if (pInst2->pObject->type == TYPE_SHIP) {
						if (CollisionIntersection_RectRect(pInst->boundingBox, pInst->velCurr, pInst2->boundingBox, pInst2->velCurr)) {
							gameObjInstDestroy(pInst);

							//Reset Ship Position
							AEVec2 zero = { 0,0 };
							pInst2->velCurr = zero;
							pInst2->posCurr = zero;

							//onValueChange = true;
						}
					}
					if (pInst2->pObject->type == TYPE_BULLET) {
						if (CollisionIntersection_RectRect(pInst->boundingBox, pInst->velCurr, pInst2->boundingBox, pInst2->velCurr)) {
							gameObjInstDestroy(pInst);

							//Reset Ship Position
							AEVec2 zero = { 0,0 };
							pInst2->velCurr = zero;
							pInst2->posCurr = zero;

							//onValueChange = true;
						}
					}
				}
			}
		}
	}

	
	if (AEInputCheckCurr(AEVK_UP))
	{
		SendEventToServer(assignedShipID, MESSAGE_TYPE::TYPE_MOVEMENT_UP);
	}

	if (AEInputCheckCurr(AEVK_DOWN))
	{
		SendEventToServer(assignedShipID, MESSAGE_TYPE::TYPE_MOVEMENT_DOWN);
	}

	if (AEInputCheckCurr(AEVK_LEFT))
	{
		SendEventToServer(assignedShipID, MESSAGE_TYPE::TYPE_MOVEMENT_LEFT);
	}

	if (AEInputCheckCurr(AEVK_RIGHT))
	{
		SendEventToServer(assignedShipID, MESSAGE_TYPE::TYPE_MOVEMENT_RIGHT);
	}

	if (AEInputCheckTriggered(AEVK_SPACE))
	{
 		SendEventToServer(assignedShipID, MESSAGE_TYPE::TYPE_SHOOT);
	}
	
	// ===================================================
	// update active game object instances based on server
	// ===================================================
	// Receive Position Information from Server
	/*
	char buffer[sizeof(SERVER_MESSAGE_FORMAT)];
	sockaddr_in servAddr;
	int servAddrLen = sizeof(servAddr);

	int bytesRead = recvfrom(clientSocket, buffer, sizeof(buffer), 0,
		reinterpret_cast<sockaddr*>(&servAddr), &servAddrLen);
	if (bytesRead == SOCKET_ERROR) {
		std::cerr << "recvfrom() failed: " << WSAGetLastError() << std::endl;
	}

	SERVER_MESSAGE_FORMAT recv{ *reinterpret_cast<SERVER_MESSAGE_FORMAT*>(buffer) };
	sGameObjInstList[recv.ObjectID].posCurr = recv.position;
	*/


	// =====================================
	// calculate the matrix for all objects
	// =====================================

	std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);
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
}

/******************************************************************************/
/*!
	Draws objects so that they can be seen on screen
*/
/******************************************************************************/
void GameStateAsteroidsDraw(void)
{
	std::lock_guard<std::mutex> lock(GAME_OBJECT_LIST_MUTEX);

	char strBuffer[1024];
	
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	//AEGfxTextureSet(NULL, 0, 0);

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
	//if(true)
	//{
		sprintf_s(strBuffer, "Score: %d", sScore);
		AEGfxPrint(static_cast<s8>(fontid), strBuffer, .0f, .5f, 1.5f, 1.f, 0.f, 1.f);

	//	printf("%s \n", strBuffer);

		sprintf_s(strBuffer, "Ship Left: %d", sShipLives >= 0 ? sShipLives : 0);
		AEGfxPrint(static_cast<s8>(fontid), strBuffer, .0f, .8f, 1.5f, 1.f, 0.f, 1.f);

	//	//AEGfxPrint(600, 10, (u32)-1, strBuffer);
	//	printf("%s \n", strBuffer);

	//	// display the game over message
	//	if (sShipLives < 0)
	//	{
	//		//AEGfxPrint(280, 260, 0xFFFFFFFF, "       GAME OVER       ");
	//		printf("       GAME OVER       \n");
	//	}

	//	if (sScore >= 5000) {
	//		printf("You Rock \n");
	//	}

	//	onValueChange = false;
	//}
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

void resetNonGameObjs(int offset)
{
	std::cout << "resetting\n";
	for (unsigned long i = offset; i < GAME_OBJ_INST_NUM_MAX; i++)
	{
		GameObjInst* pInst = sGameObjInstList + i;
		gameObjInstDestroy(pInst);
	}
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

	AE_ASSERT_PARM(type < sGameObjNum);
	
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

/******************************************************************************/
/*!
	Sends an event to the server
*/
/******************************************************************************/
void SendEventToServer(int shipID, MESSAGE_TYPE messageType) {
	CLIENT_MESSAGE_FORMAT toSend{};
	toSend.MessageType = static_cast<int>(messageType);
	toSend.ShipID = shipID;

	int errorCode = sendto(clientSocket, 
		reinterpret_cast<const char*>(&toSend), 
		sizeof(CLIENT_MESSAGE_FORMAT),
		0, 
		serverInfo->ai_addr,
		static_cast<int>(serverInfo->ai_addrlen));

	if (errorCode == SOCKET_ERROR) {
		std::cerr << "sendto() failed: " << WSAGetLastError() << std::endl;
		freeaddrinfo(serverInfo);
		closesocket(clientSocket);
		WSACleanup();
		return;
	}
}