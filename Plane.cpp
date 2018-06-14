/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Plane class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Plane.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using 
* member variables a, b, c, d.
*/
bool Plane::isInside(glm::vec3 pt)
{
	glm::vec3 uA = b - a; glm::vec3 vA = pt - a;
	glm::vec3 uB = c - b; glm::vec3 vB = pt - b;
	glm::vec3 uC = d - c; glm::vec3 vC = pt - c;
	glm::vec3 uD = a - d; glm::vec3 vD = pt - d;

	float A = glm::dot(glm::cross(uA, vA), normal(pt));
	float B = glm::dot(glm::cross(uB, vB), normal(pt));
	float C = glm::dot(glm::cross(uC, vC), normal(pt));
	float D = glm::dot(glm::cross(uD, vD), normal(pt));
	if (A >= 0 && B >= 0 && C >= 0 && D >= 0) {
		return true;
	}
	
	return false;

}

/**
* Plane's intersection method.  The input is a ray (pos, dir). 
*/
float Plane::intersect(glm::vec3 posn, glm::vec3 dir)
{
	glm::vec3 n = normal(posn);
	glm::vec3 vdif = a - posn;
	float vdotn = glm::dot(dir, n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	glm::vec3 q = posn + dir*t;
	if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the plane's normal vector using 
* member variables a, b, c, d.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Plane::normal(glm::vec3 pt)
{
	return glm::normalize(glm::cross((b - a), (d - a)));
}



