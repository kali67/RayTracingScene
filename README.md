
![alt text](https://github.com/kali67/RayTracingScene/blob/master/finalScene.PNG)

# Procedural Texturing

  To implement this for the sphere i used the equation:

                     int(ray.xpt.x) + int(ray.xpt.y)) % 2 == 1

  If the sum of the x and y point is odd then a darker colour will be assigned, otherwise a white will be.

  To implement this for the plane used the equations:

                      int modx = (int)((ray.xpt.x + 50) / 8) % 2;
                      int modz = (int)((ray.xpt.z + 200) / 8) % 2;

  If both modx and modz were even OR both modx and modz were not even then i would assign a pink colour otherwise a white is assigned.



# Texture Mapping Spheres with Images

To implement this on the two spheres i needed to calculate the x and y coordinates of a particular point taking into account the surface of the sphere. To do this i used trigonometry.
To map a point x,y onto the texture i first needed to calculate the angle theta. To do this i used these equations (diagram shows the method for getting the x opposite edge):

                     float xDiff = abs(ray.xpt.x - center.x); // Opposite of theta 
                     float yDiff = abs(ray.xpt.y - center.y); // need to do the same for y

I also used the hypotenuse as the radius of the sphere. It was then possible for me to use this:

                    float xAngle = asinf(xDiff/radius);
                    float yAngle = asinf(yDiff / radius);

To get both angles for x and for y, this would be angle from the centre of the sphere to point of interest. Using the equation for the circumference of a circle (2Pir) i was able to multiply this by the angles calculated above to get the arc distance along x and y axis.

                    float xPlane = 2 * 3.14*radius * (xAngle / 360);
                    float yPlane = 2 * 3.14*radius * (yAngle / 360);
        
I then used these points to get the color value from the texture.





# Transparent Sphere

The red sphere is transparent and the way i implemented this was to create a new ray at the point of contact with the sphere (going in the same direction) and then recursively trace this to get the colour value behind the sphere. A red tint was given to it to make it look nicer.
 
            Ray transRay(ray.xpt, ray.dir);
            colorSum += glm::vec3(0.309, 0.847, 0.666) * trace(transRay, step + 1);






# Rotation of the Cube

To rotate the cube i rotated each vertex of each plane by 45 degrees. This was done by multiplying the rotation matrix along the y-axis by the vector of the vertex. This was done for each of them. The matrix i used was :

        glm::mat4(0.707f, 0, 0.7068f, 0,
                0.f, 1.f, 0.f, 0.f,
               -0.7068f,0, 0.707f,0.f ,
                 0.f,0.f, 0.f,1.f );

Which was obtained by using angle of 45 degrees and solving the rotation matrix about the y-axis.






# Refraction

One of my spheres has refraction with a eta of 1.01. This was done by recursively tracing the rays. Two rays are calculated for each trace. One going through the medium and one exiting. The colour value returned was given a specular highlight to give a more realistic look. I chose not to make the sphere reflective also as the checkered floor did not look great on it.







# Anti-Aliasing 

To implement anti-aliasing i created four rays that were located in each center of the cell that was split in 4. This was done by calculating the quarter width of the cell and then adding and subtracting this value from the rays x and y positions to get a new ray direction. The new ray would then be traced and have a final colour value. This was done for all 4 corners and then the colour values were averaged. This gave the final colour that is shown on the screen. Although it is hard to see as the images are so small, i have given a side by side of the final solution with and without AA.





# Success and Failures


The centre marble was a success as i managed to find a really nice texture for it. It looks very realistic with the reflections on specular highlight. I am also happy with the shadows for the transparent and reflective spheres. They have a slightly lighter colour. This was achieved by multiplying the colour value by a lighter ambient colour if the point is in shadow. Something that did not work so well was the procedural texturing of the sphere near the bottom you can see that the quads are stretched slightly.


References

I used lab 7 & 8 as a base for this assignment
Textures from: https://www.textures.com/


Time to run

With 4xAA and 1000x1000 it takes about 30 seconds to run.

Build Commands
g++ -Wall -o %e %f Plane.cpp Sphere.cpp SceneObject.cpp Ray.cpp TextureBMP.cpp -lm -lGL -lGLU -lglut
