//  Created by Nicholas Tofani on 4/18/17.
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
    void settingXYZ(GLfloat newX, GLfloat newY, GLfloat newZ) {
        x = newX;
        y = newY;
        z = newZ;
    }
} eyePosition, screenPosition, normal;

struct triangles {
    vertices one;
    vertices two;
    vertices three;
    void settingVertices(vertices newOne, vertices newTwo, vertices newThree) {
        one = newOne;
        two = newTwo;
        three = newThree;
    }
} mainTriangle;

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

void drawpixel(float x,float y,float r,float g,float b) // assume x,y 0-100
{
	#define SZ  .02
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

void setupTriangle() {
    vertices tempVert1;
    tempVert1.settingXYZ(1.0f, 0.0f, 6.0f);
    vertices tempVert2;
    tempVert2.settingXYZ(0.5f, 1.0f, 6.0f);
    vertices tempVert3;
    tempVert3.settingXYZ(0.0f, 0.0f, 6.0f);
    mainTriangle.settingVertices(tempVert1, tempVert2, tempVert3);
    normal = calcNormal(mainTriangle.one, mainTriangle.two, mainTriangle.three);
}

GLfloat dotProduct(vertices vert1, vertices vert2) {
    return ((vert1.x * vert2.x) + (vert1.y * vert2.y) + (vert1.z * vert2.z));
}

GLfloat calcU(vertices vert1, vertices screenPosition, vertices normal) {
    vertices temp;
    temp.settingXYZ((vert1.x - eyePosition.x), (vert1.y - eyePosition.y), (vert1.z - eyePosition.z));
    GLfloat top = dotProduct(normal, temp);
    vertices temp2;
    temp2.settingXYZ((screenPosition.x - eyePosition.x), (screenPosition.y - eyePosition.y), (screenPosition.z - eyePosition.z));
    GLfloat bottom = dotProduct(normal, temp2);
    return (top/bottom);
}

vertices calcIntersection(GLfloat U, vertices screenPosition) {
    vertices temp;
    temp.settingXYZ( U*(screenPosition.x - eyePosition.x), U*(screenPosition.y - eyePosition.y), U*(screenPosition.z - eyePosition.z));
    vertices willReturn;
    willReturn.settingXYZ((eyePosition.x + temp.x), (eyePosition.y + temp.y), (eyePosition.z + temp.z));
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

color ray(GLfloat x, GLfloat y) {
    vertices screenPosition;
    screenPosition.settingXYZ(x, y, 0);
    GLfloat U = calcU(mainTriangle.three, screenPosition, normal);
    vertices intersection = calcIntersection(U, screenPosition);
    vertices V1 = calcVertex(mainTriangle.one, intersection);
    vertices V2 = calcVertex(mainTriangle.two, intersection);
    vertices V3 = calcVertex(mainTriangle.three, intersection);
    vertices C1 = crossProduct(V1, V2);
    vertices C2 = crossProduct(V2, V3);
    vertices C3 = crossProduct(V3, V1);
    return calcColor(C1, C2, C3);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    for(GLfloat x = 0; x <= 1; x = x + .01) {
        for (GLfloat y = 0; y <= 1; y = y + .01) {
            color newColor = ray(x, y);
            drawpixel((x*100) , (y*100), newColor.r, newColor.g, newColor.b);
        }
    }
    glFlush();
}

int main(int argc, char** argv) {
    eyePosition.settingXYZ(0.5, 0.5, -2);
    setupTriangle();
    glutInit(&argc,argv);
    glutCreateWindow("simple");
    glutDisplayFunc(display);
    glutMainLoop();
}