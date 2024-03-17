/******************************************************************************/
/*!
\file		Collision.h
\author 	Ernest Cheo
\par    	email: e.cheo\@digipen.edu
\date   	January 29, 2023
\brief		This is the collision header file that has the function
			CollisionIntersection_RectRect that will check for both
			dynamic and static collision between 2 AABBs.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
 /******************************************************************************/

#ifndef CSD1130_COLLISION_H_
#define CSD1130_COLLISION_H_

#include "AEEngine.h"

/**************************************************************************/
/*!

	*/
/**************************************************************************/
struct AABB
{
	//AEVec2	c; // center
	//float  r[2]; // holds half width and half height
	
	AEVec2	min;
	AEVec2	max;
};

bool CollisionIntersection_RectRect(const AABB & aabb1, const AEVec2 & vel1, 
									const AABB & aabb2, const AEVec2 & vel2);


#endif // CSD1130_COLLISION_H_