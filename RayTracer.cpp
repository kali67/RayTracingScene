#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include <GL/glut.h>
#include "Plane.h"
#include "TextureBMP.h"
using namespace std;

const float WIDTH = 35.;
const float HEIGHT = 35.;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 5;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene
TextureBMP texture;
TextureBMP paint;


glm::vec3 textureSphere1(Ray ray) {
	glm::vec3 center(-25.0, 6.0, -60.);
	float radius = 6.;

	float xDiff = abs(ray.xpt.x - center.x);
	float yDiff = abs(ray.xpt.y - center.y);

	float xAngle = asinf(xDiff/radius);
	float yAngle = asinf(yDiff / radius);


	float xPlane = 2 * 3.14*radius * (xAngle / 360);
	float yPlane = 2 * 3.14*radius * (yAngle / 360);

	return paint.getColorAt(xPlane, yPlane);
}


glm::vec3 textureSphere0(Ray ray) {
	glm::vec3 center(-5.0, -5.0, -90.0);
	float radius = 15.;

	float xDiff = abs(ray.xpt.x - center.x);
	float yDiff = abs(ray.xpt.y - center.y);

	float xAngle = asinf(xDiff / radius);
	float yAngle = asinf(yDiff / radius);


	float xPlane = 2 * 3.14*radius * (xAngle / 360);
	float yPlane = 2 * 3.14*radius * (yAngle / 360);

	return texture.getColorAt(xPlane, yPlane);
}



//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(100,90, -3);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
	glm::vec3 ambientGlassCol(0.854, 0.843, 0.843);
	glm::vec3 colorSum;
	glm::vec3 g;
	glm::vec3 m;
	glm::vec3 h;

    ray.closestPt(sceneObjects);		//Compute the closest point of intersetion of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour
	glm::vec3 materialCol = sceneObjects[ray.xindex]->getColor(); //else return object's colour
	glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt);
	glm::vec3 lightVector = glm::normalize(light - ray.xpt);

	Ray shadow(ray.xpt, lightVector); //create the shadow ray in the direction of the light vector
	shadow.closestPt(sceneObjects); // finds the closest point of intersection with a scene object

	float ldot = glm::dot(lightVector, normalVector);
	glm::vec3 reflVector = glm::reflect(-lightVector, normalVector);
	float dot = glm::dot(reflVector, glm::normalize(glm::vec3(ray.pt - ray.xpt)));
	float lightDist = glm::length(glm::vec3(light - ray.xpt));	 

	glm::vec3 spec(0);


	if (ray.xindex == 4 || shadow.xindex == 4) {
		materialCol = textureSphere1(ray);
	}
	if (ray.xindex == 0 || shadow.xindex == 0) {
		materialCol = textureSphere0(ray);
	}
	//shadows
	if (ldot <= 0 || shadow.xindex > -1 && (shadow.xdist < lightDist)) {
		colorSum = ambientCol * materialCol;
	}
	else { 
		if (dot < 0) {
			spec = glm::vec3(0, 0, 0) * pow(dot, 15.0f);
			colorSum += ambientCol * materialCol + materialCol * ldot + glm::vec3(0, 0, 0) * pow(dot, 15.0f);
		}
		else { 
			spec = glm::vec3(1, 1, 1) * pow(dot, 15.0f);
			colorSum += ambientCol * materialCol + materialCol * ldot + glm::vec3(1, 1, 1) * pow(dot, 15.0f);
		}
	}

	//Floor
	if (ray.xindex == 3) {
		int modx = (int)((ray.xpt.x + 50) / 8) % 2;
		int modz = (int)((ray.xpt.z + 200) / 8) % 2;
		if ((modx && modz) || (!modx && !modz)) {
			colorSum = glm::vec3(0.819, 0.121, 0.474);
		}
		else {
			colorSum = glm::vec3(1, 1, 1);
		}
		//Shadow
		if (ldot <= 0 || shadow.xindex > -1 && (shadow.xdist < lightDist)) { //if in shadow
			if (shadow.xindex == 1 || shadow.xindex == 5) { //assign lighter color shadow to glass sphere
				colorSum *= ambientGlassCol;
			}
			else {
				colorSum *= ambientCol;
			}	
		}
	}
	


	//Refraction
	if ((ray.xindex == 1) && step < MAX_STEPS) {
		g = glm::refract(ray.dir, normalVector, (1.f / 1.01f));
		Ray refrRay1(ray.xpt, g);
		refrRay1.closestPt(sceneObjects);
		m = sceneObjects[refrRay1.xindex]->normal(refrRay1.xpt);
		h = glm::refract(g, -m, 1.0f / (1.f / 1.01f));
		Ray reflectedRay(refrRay1.xpt, h);
		glm::vec3 reflectedCol = trace(reflectedRay, step + 1);   
		colorSum = (0.9f*reflectedCol) + spec; 
	}
	//give reflections to floor  and main sphere
	if ((ray.xindex == 0) && step < 2) {
		glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
		Ray reflectedRay(ray.xpt, reflectedDir);
		glm::vec3 reflectedCol = trace(reflectedRay, step + 1);  //Recursion!  
		colorSum += (0.8f*reflectedCol);
	}

	//texture sphere
	if (ray.xindex == 2) {
		if ((int(ray.xpt.x) + int(ray.xpt.y)) % 2 == 1) {
			colorSum += glm::vec3(0,0,0);
		}
		else {
			colorSum += glm::vec3(1, 1, 1);
		}
	}

	if ((ray.xindex == 5)) {
		Ray transRay(ray.xpt, ray.dir);
		colorSum += glm::vec3(0.309, 0.847, 0.666) * trace(transRay, step + 1);
	}

	return colorSum;

}


glm::vec3 anti_alising(glm::vec3 eye, float cellX, float cellY, float xp, float yp) {
	float quarter = cellX * (0.25); //get a quarter of the cells X distance

	glm::vec3 colorSum(0);
	glm::vec3 avg(0.25);

	Ray ray = Ray(eye, glm::vec3(xp + quarter, yp + quarter, -EDIST)); //top right center
	ray.normalize();
	colorSum += trace(ray, 1);


	ray = Ray(eye, glm::vec3(xp - quarter, yp - quarter, -EDIST)); //bottom left center
	ray.normalize();
	colorSum += trace(ray, 1);

	ray = Ray(eye, glm::vec3(xp - quarter, yp + quarter, -EDIST)); //top left center
	ray.normalize();
	colorSum += trace(ray, 1);

	ray = Ray(eye, glm::vec3(xp + quarter, yp - quarter, -EDIST)); //bottom right center
	ray.normalize();
	colorSum += trace(ray, 1);

	colorSum *= avg;
	return colorSum;
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{

			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector

		glm::vec3 col = anti_alising(eye, cellX, cellY, xp, yp); //Trace the primary ray and get the colour value

		//   glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}

	
glm::vec3 rotate(glm::vec4 point) {
	glm::mat4 rotationMatrix = { 0.707f, 0, 0.7068f, 0,
		0.f, 1.f, 0.f, 0.f,
		-0.7068f,0, 0.707f,0.f ,
		0.f,0.f, 0.f,0.f };
	return glm::vec3((rotationMatrix * point))-glm::vec3(46,0,0); //TODO: 
}

void drawCube(float x, float y, float z, float scale, glm::vec3 color) {

	Plane *topPlane = new Plane(
		rotate(glm::vec4(x - (scale/2), y + (scale/2), z + (scale/2), 1)), //A
		rotate(glm::vec4(x - (scale / 2), y + (scale / 2), z - (scale / 2), 1)), //B
		rotate(glm::vec4(x + (scale / 2), y + (scale / 2), z - (scale / 2), 1)), //C
		rotate(glm::vec4(x + (scale / 2), y + (scale / 2), z + (scale / 2), 1)), //D
		color
	);

	Plane *bottomPlane = new Plane(
		rotate(glm::vec4(x - (scale / 2), y - (scale / 2), z + (scale / 2), 1)), //A
		rotate(glm::vec4(x - (scale / 2), y - (scale / 2), z - (scale / 2), 1)), //B
		rotate(glm::vec4(x + (scale / 2), y - (scale / 2), z - (scale / 2), 1)), //C
		rotate(glm::vec4(x + (scale / 2), y - (scale / 2), z + (scale / 2), 1)), //D
		color
	);
	Plane *rightPlane = new Plane(
		rotate(glm::vec4(x + (scale / 2), y + (scale / 2), z + (scale / 2), 1)), //A
		rotate(glm::vec4(x + (scale / 2), y - (scale / 2), z + (scale / 2), 1)), //C
		rotate(glm::vec4(x + (scale / 2), y - (scale / 2), z - (scale / 2), 1)), //D
		rotate(glm::vec4(x + (scale / 2), y + (scale / 2), z - (scale / 2), 1)), //B
		color
	);

	Plane *leftPlane = new Plane(
		rotate(glm::vec4(x - (scale / 2), y + (scale / 2), z + (scale / 2), 1)), //A
		rotate(glm::vec4(x - (scale / 2), y - (scale / 2), z + (scale / 2), 1)), //C
		rotate(glm::vec4(x - (scale / 2), y - (scale / 2), z - (scale / 2), 1)), //D
		rotate(glm::vec4(x - (scale / 2), y + (scale / 2), z - (scale / 2), 1)), //B
		color
	);

	Plane *frontPlane = new Plane(
		rotate(glm::vec4(x - (scale / 2), y + (scale / 2), z + (scale / 2), 1)), //A
		rotate(glm::vec4(x - (scale / 2), y - (scale / 2), z + (scale / 2), 1)), //B
		rotate(glm::vec4(x + (scale / 2), y - (scale / 2), z + (scale / 2), 1)), //C
		rotate(glm::vec4(x + (scale / 2), y + (scale / 2), z + (scale / 2), 1)), //D
		color
	);
	sceneObjects.push_back(topPlane);
	sceneObjects.push_back(bottomPlane);
	sceneObjects.push_back(rightPlane);
	sceneObjects.push_back(leftPlane);
	sceneObjects.push_back(frontPlane);
}

//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
	texture = TextureBMP("..\\..\\..\\Downloads\\Lab 07 Files\\Lab 07 Files\\stone.bmp");
	paint = TextureBMP("..\\..\\..\\Downloads\\Lab 07 Files\\Lab 07 Files\\redMarble.bmp");
	//-- Create a pointer to a sphere 
	Sphere *sphere1 = new Sphere(glm::vec3(-5.0, -5.0, -90.0), 15., glm::vec3(0, 0, 1));

	//--Add the above to the list of scene objects.
	sceneObjects.push_back(sphere1); 

	Sphere *sphere2 = new Sphere(glm::vec3(22.0, -12., -80.0), 8., glm::vec3(1, 1, 1));
	sceneObjects.push_back(sphere2);

	Sphere *sphere3 = new Sphere(glm::vec3(15.0, 6.0, -90.), 8., glm::vec3(1, 0, 0));
	sceneObjects.push_back(sphere3);


	Plane *plane = new Plane(glm::vec3(-90., -20, -40),    //Point A   
		glm::vec3(90., -20, -40),     //Point B   
		glm::vec3(90., -20, -200),    //Point C 
		glm::vec3(-90., -20, -200),    //Point D 
		glm::vec3(0.5, 0.5, 0));   //Colour 

	sceneObjects.push_back(plane);

	Sphere *sphere4 = new Sphere(glm::vec3(-25.0, 6.0, -90.), 6., glm::vec3(1, 0, 0));
	sceneObjects.push_back(sphere4);

	Sphere *sphere5 = new Sphere(glm::vec3(6., -16., -50.), 4., glm::vec3(1, 0, 0));
	sceneObjects.push_back(sphere5);

	drawCube(-15., -16., -60.0, 8, glm::vec3(0.831, 0.831, 0.109));


}




int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("Raytracer");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
