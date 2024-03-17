/******************************************************************************/
/*!
\file			Collision.cpp
\author 	
\par    	
\date   	
\brief		This is the collision source file that has the function
					CollisionIntersection_RectRect that will check for both
					dynamic and static collision between 2 AABBs.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
/******************************************************************************/

#include "main.h"

/**************************************************************************/
/*!

	*/
/**************************************************************************/
bool CollisionIntersection_RectRect(const AABB& aabb1, const AEVec2& vel1,
	const AABB& aabb2, const AEVec2& vel2)
{
	if ((aabb1.max.x < aabb2.min.x) || (aabb1.max.y < aabb2.min.y) || (aabb1.min.x > aabb2.max.x) || (aabb1.min.y > aabb2.max.y)) {
		float timeFirst = 0;
		float timeLast = static_cast<float>(AEFrameRateControllerGetFrameTime());

		AEVec2 vb = { vel2.x - vel1.x, vel2.y - vel1.y }; //Vrel

		if (vb.x < 0) {
			// Case 1
			if (aabb1.min.x > aabb2.max.x) {
				return false; //No intersection and b is moving away
			}

			// Case 4
			if (aabb1.max.x < aabb2.min.x) {
				timeFirst = max(timeFirst, (aabb1.max.x - aabb2.min.x) / vb.x);
			}

			if (aabb1.min.x < aabb2.max.x) {
				timeLast = min(timeLast, (aabb1.min.x - aabb2.max.x) / vb.x);
			}
		}

		if (vb.y < 0) {
			// Case 1
			if (aabb1.min.y > aabb2.max.y) {
				return false; //No intersection and b is moving away
			}

			// Case 4
			if (aabb1.max.y < aabb2.min.y) {
				timeFirst = max(timeFirst, (aabb1.max.y - aabb2.min.y) / vb.y);
			}

			if (aabb1.min.y < aabb2.max.y) {
				timeLast = min(timeLast, (aabb1.min.y - aabb2.max.y) / vb.y);
			}
		}

		if (vb.x > 0) {
			// Case 3
			if (aabb1.max.x < aabb2.min.x) {
				return false; //No intersection and b is moving away
			}

			// Case 2
			if (aabb1.min.x > aabb2.max.x) { //The shortest distance before collision
				timeFirst = max(timeFirst, (aabb1.min.x - aabb2.max.x) / vb.x);
			}

			if (aabb1.max.x > aabb2.min.x) { //The longest distance before collision
				timeLast = min(timeLast, (aabb1.max.x - aabb2.min.x) / vb.x);
			}
		}

		if (vb.y > 0) {
			//Case 3
			if (aabb1.max.y < aabb2.min.y) {
				return false; //No intersection and b is moving away
			}

			// Case 2
			if (aabb1.min.y > aabb2.max.y) { //The shortest distance before collision
				timeFirst = max(timeFirst, (aabb1.min.y - aabb2.max.y) / vb.y);
			}

			if (aabb1.max.y > aabb2.min.y) { //The longest distance before collision
				timeLast = min(timeLast, (aabb1.max.y - aabb2.min.y) / vb.y);
			}
		}

		// Case 5
		if (timeFirst >= timeLast) {
			return false;
		}
	}
	return true;
}