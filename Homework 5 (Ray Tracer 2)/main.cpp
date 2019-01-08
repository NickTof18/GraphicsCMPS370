//  Created by Nicholas Tofani on 5/04/17.
//  Copyright © 2017 Nicholas Tofani. All rights reserved.
//
#include <iostream>
#include <gl/glew.h>
#include <GLUT/GLUT.h>
#include <stdio.h>
#include <complex>

struct vertices {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    //Only used for Spheres
    GLfloat radius;

    void settingXYZ(GLfloat newX, GLfloat newY, GLfloat newZ) {
        x = newX;
        y = newY;
        z = newZ;
    }

} eyePosition, screenPosition, light;

struct triangles {
    vertices one;
    vertices two;
    vertices three;
    vertices normal;

    void settingVertices(vertices newOne, vertices newTwo, vertices newThree) {
        one = newOne;
        two = newTwo;
        three = newThree;
    }

} T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, floorT1, floorT2, triArray[13];

struct color {
    GLfloat r;
    GLfloat g;
    GLfloat b;

    void setColors(GLfloat newR, GLfloat newG, GLfloat newB) {
        r = newR;
        g = newG;
        b = newB;
    }

};

// assume x,y 0-100 
void drawpixel(float x,float y,float r,float g,float b) {
	#define SZ  .004
    glBegin(GL_TRIANGLES);
    glColor3f(r,g,b);
    glVertex2f(-1.0 + .02 * (float)x,     -1.0 + .02 * (float)y);
    glVertex2f(-1.0 + .02 * (float)x,     -1.0 + .02 * (float)y + SZ);
    glVertex2f(-1.0 + .02 * (float)x + SZ, -1.0 + .02 * (float)y);
    
    glVertex2f(-1.0 + .02 * (float)x + SZ, -1.0 + .02 * (float)y);
    glVertex2f(-1.0 + .02 * (float)x + SZ, -1.0 + .02 * (float)y + SZ);
    glVertex2f(-1.0 + .02 * (float)x,     -1.0 + .02 * (float)y + SZ);
    glEnd();
}

GLfloat calcMagnitude(vertices number) {
    GLfloat temp = (number.x * number.x) + (number.y * number.y) + (number.z * number.z);
    return sqrtf(temp);
}

GLfloat calcUOfSphere(vertices P1, vertices P2, vertices P3) {
    GLfloat top = ((P3.x - P1.x)*(P2.x - P1.x)) + ((P3.y - P1.y)*(P2.y - P1.y)) + ((P3.z - P1.z)*(P2.z - P1.z));
    vertices temp2;
    temp2.settingXYZ((P2.x - P1.x), (P2.y - P1.y), (P2.z - P1.z));
    GLfloat bottom = calcMagnitude(temp2);
    return (top/(bottom * bottom));
}

vertices closestPointToSphere(GLfloat U, vertices P1, vertices P2) {
    vertices result, temp;
    temp.settingXYZ(U*(P2.x-P1.x), U*(P2.y-P1.y), U*(P2.z-P1.z));
    result.settingXYZ((P1.x + temp.x), (P1.y + temp.y), (P1.z + temp.z));
    return result;
}

GLfloat distanceFromCenterOfCircle(vertices P, vertices P3) {
    vertices temp;
    temp.settingXYZ((P.x - P3.x), (P.y - P3.y), (P.z - P3.z));
    return calcMagnitude(temp);
}

vertices crossProduct(vertices vert1, vertices vert2) {
    GLfloat newX = (vert1.y * vert2.z) - (vert1.z * vert2.y);
    GLfloat newY = (vert1.x * vert2.z) - (vert1.z * vert2.x);
    GLfloat newZ = (vert1.x * vert2.y) - (vert1.y * vert2.x);
    vertices willReturn;
    willReturn.settingXYZ(newX, newY, newZ);
    return willReturn;
}

vertices calcNormal(vertices vert1, vertices vert2, vertices vert3) {
    vertices temp1;
    temp1.settingXYZ((vert2.x - vert1.x), (vert2.y - vert1.y), (vert2.z - vert1.z));
    vertices temp2;
    temp2.settingXYZ((vert3.x - vert1.x), (vert3.y - vert1.y), (vert3.z - vert1.z));
    return crossProduct(temp1, temp2);
}

GLfloat dotProduct(vertices vert1, vertices vert2) {
    return ((vert1.x * vert2.x) + (vert1.y * vert2.y) + (vert1.z * vert2.z));
}

GLfloat calcU(vertices startingVert, vertices vert1, vertices direction, vertices normal) {
    vertices temp;
    temp.settingXYZ((vert1.x - startingVert.x), (vert1.y - startingVert.y), (vert1.z - startingVert.z));
    GLfloat top = dotProduct(normal, temp);
    vertices temp2;
    temp2.settingXYZ((direction.x - startingVert.x), (direction.y - startingVert.y), (direction.z - startingVert.z));
    GLfloat bottom = dotProduct(normal, temp2);
    return (top/bottom);
}

vertices calcIntersection(vertices startingVert, GLfloat U, vertices direction) {
    vertices temp;
    temp.settingXYZ( U*(direction.x - startingVert.x), U*(direction.y - startingVert.y), U*(direction.z - startingVert.z));
    vertices willReturn;
    willReturn.settingXYZ((startingVert.x + temp.x), (startingVert.y + temp.y), (startingVert.z + temp.z));
    return willReturn;
}

vertices calcVertex(vertices vert1, vertices intersection) {
    vertices willReturn;
    willReturn.settingXYZ((vert1.x - intersection.x), (vert1.y - intersection.y), (vert1.z - intersection.z));
    return willReturn;
}

color calcColor(vertices C1, vertices C2, vertices C3) {
    GLfloat D1 = dotProduct(C1, C2);
    GLfloat D2 = dotProduct(C2, C3);
    GLfloat D3 = dotProduct(C3, C1);
    color newColor;
    newColor.setColors(D1, D2, D3);
    if(D1 >= 0 && D2 >= 0 && D3 >= 0) {
        //white
        newColor.setColors(1.0f, 1.0f, 1.0f);
    }
    else {
        //black
        newColor.setColors(0.0f, 0.0f, 0.0f);
    }
    return newColor;
}

GLfloat ray(GLfloat startingX, GLfloat startingY, GLfloat startingZ,
            GLfloat directionX, GLfloat directionY, GLfloat directionZ,
            int bounces) {
    if(bounces == 2) {
        //Must be in the shadow if only using diffuse/Ambient shading
        return 0.1;
    }
    GLfloat bright = 0;
    vertices startingVertice;
    startingVertice.settingXYZ(startingX, startingY, startingZ);
    vertices directionVertice;
    directionVertice.settingXYZ(directionX, directionY, directionZ);
    bool hitTriangle = false;
    for(int i = 0; i != 14 && !hitTriangle; i++) {
        GLfloat Utri = calcU(startingVertice, triArray[i].three, directionVertice, triArray[i].normal);
        if(Utri > 0) {
            vertices intersection = calcIntersection(startingVertice, Utri, directionVertice);
            vertices V1 = calcVertex(triArray[i].one, intersection);
            vertices V2 = calcVertex(triArray[i].two, intersection);
            vertices V3 = calcVertex(triArray[i].three, intersection);
            vertices C1 = crossProduct(V1, V2);
            vertices C2 = crossProduct(V2, V3);
            vertices C3 = crossProduct(V3, V1);
            color currentRay = calcColor(C1, C2, C3);
			//checks if it hits a triangle
            if(currentRay.r > 0 && currentRay.g > 0 && currentRay.b > 0) { 
                vertices newRay;
                bright = .1;
                hitTriangle = true;
                newRay.settingXYZ((intersection.x - light.x), (intersection.y - light.y), (intersection.z - light.z));
                double willBeChecked = (dotProduct(directionVertice, triArray[i].normal));
                double willBeChecked2 = dotProduct(newRay, triArray[i].normal);
                if(willBeChecked * willBeChecked2 < 0) {
					//Stops the new ray from triangle from hitting the same triangle
                    vertices temp2;
                    temp2.settingXYZ((intersection.x - light.x), (intersection.y - light.y), (intersection.z - light.z));
                    bright = bright + (ray(intersection.x, intersection.y, intersection.z, light.x, light.y, light.z, bounces + 1) / calcMagnitude(temp2));
                    if(bright < .1) {
                        //Triangles bright must be atleast .1
                        bright = .1;
                    }
                }
             }
        }
    }
    if(!hitTriangle) {
        //If ray didn't hit a triangle see if it hits the light
        GLfloat U = calcUOfSphere(startingVertice, directionVertice, light);
        vertices P = closestPointToSphere(U, startingVertice, directionVertice);
        GLfloat distance = distanceFromCenterOfCircle(P, light);
        if(distance <= light.radius) {
            bright = 1;
        }
    }
    return bright;
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    //    1/500 = .002
    for(GLfloat x = 0; x <= 1; x = x + .002) {
        for (GLfloat y = 0; y <= 1; y = y + .002) {
            GLfloat newColor = ray(eyePosition.x, eyePosition.y, eyePosition.z, x, y, 0, 0);
            drawpixel((x*100) , (y*100), newColor, newColor, newColor);
        }
    }
    glFlush();
}

vertices calcTriNormal(triangles tri) {
    return calcNormal(tri.one, tri.two, tri.three);
}

void setupCube(void) {
    vertices V1, V2, V3, V4, V5, V6, V7, V8;
    V1.settingXYZ(.1, .25, .5);
    V2.settingXYZ(.3, .25, .5);
    V3.settingXYZ(.3, .05, .5);
    V4.settingXYZ(.1, .05, .5);
    V5.settingXYZ(.1, .25, .7);
    V6.settingXYZ(.3, .25, .7);
    V7.settingXYZ(.3, .05, .7);
    V8.settingXYZ(.1, .05, .7);

	//front
    T1.settingVertices(V1, V2, V3); 
    T1.normal = calcTriNormal(T1);
    triArray[0] = T1;
    T2.settingVertices(V3, V4, V1);  
    T2.normal = calcTriNormal(T2);
    triArray[1] = T2;

	//top
    T3.settingVertices(V5, V6, V2); 
    T3.normal = calcTriNormal(T3);
    triArray[2] = T3;
    T4.settingVertices(V2, V1, V5);  
    T4.normal = calcTriNormal(T4);
    triArray[3] = T4;

	//right
    T5.settingVertices(V2, V6, V7); 
    T5.normal = calcTriNormal(T5);
    triArray[4] = T5;
    T6.settingVertices(V7, V3, V2);  
    T6.normal = calcTriNormal(T6);
    triArray[5] = T6;

	//bottom
    T7.settingVertices(V4, V3, V7);
    T7.normal = calcTriNormal(T7);
    triArray[6] = T7;
    T8.settingVertices(V7, V8, V4);
    T8.normal = calcTriNormal(T8);
    triArray[7] = T8;

	//left
    T9.settingVertices(V5, V1, V4); 
    T9.normal = calcTriNormal(T9);
    triArray[8] = T9;
    T10.settingVertices(V4, V8, V5); 
    T10.normal = calcTriNormal(T10);
    triArray[9] = T10;

	//back
    T11.settingVertices(V6, V5, V8);
    T11.normal = calcTriNormal(T11);
    triArray[10] = T11;
    T12.settingVertices(V8, V7, V6);  
    T12.normal = calcTriNormal(T12);
    triArray[11] = T12;
}

void setupFloor(void) {
    vertices FloorV1, FloorV2, FloorV3, FloorV4;
    FloorV1.settingXYZ(0, 0, 1);
    FloorV2.settingXYZ(1, 0, 1);
    FloorV3.settingXYZ(1, 0, 0);
    FloorV4.settingXYZ(0, 0, 0);
    floorT1.settingVertices(FloorV1, FloorV2, FloorV3);
    floorT1.normal = calcTriNormal(floorT1);
    triArray[12] = floorT1;
    floorT2.settingVertices(FloorV3, FloorV4, FloorV1);
    floorT2.normal = calcTriNormal(floorT2);
    triArray[13] = floorT2;
}

int main(int argc, char** argv) {
    eyePosition.settingXYZ(0.5, 0.5, -1.0);
    light.settingXYZ(0.5, 0.5, 1.5);
    light.radius = 0.1;
    setupFloor();
    setupCube();
    glutInit(&argc,argv);
    glutCreateWindow("simple");
    glutDisplayFunc(display);
    glutMainLoop();
}
