//  Created by Nicholas Tofani on 3/23/17.
//  Copyright © 2017 Nicholas Tofani. All rights reserved.
//
#include <iostream>
#include <gl/glew.h>
#include <GLUT/GLUT.h>
#include <stdio.h>
#include <complex>
#include "textureGraphics.c"

double eyex, eyey, eyez, theta;
GLuint texture;

struct vertices {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    void settingXYZ(GLfloat newX, GLfloat newY, GLfloat newZ) {
        x = newX;
        y = newY;
        z = newZ;
    }
} v0, v1, v2, v3, v4, v5, v6, v7;

struct triangles {
    vertices one;
    vertices two;
    vertices three;
    GLfloat textureOneX = 1.0f;
    GLfloat textureOneY = 1.0f;
    
    GLfloat textureTwoX = 1.0f;
    GLfloat textureTwoY = 0.0f;
    
    GLfloat textureThreeX = 0.0f;
    GLfloat textureThreeY = 0.0f;
    
    void textureTopTriangle() {
        textureOneX = 0.0f;
        textureOneY = 0.0f;
        
        textureTwoX = 0.0f;
        textureTwoY = 1.0f;
        
        textureThreeX = 1.0f;
        textureThreeY = 1.0f;
    }

    void settingVertices(vertices newOne, vertices newTwo, vertices newThree) {
        one = newOne;
        two = newTwo;
        three = newThree;
    }

} t0, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11;

triangles triArray[12] = {t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11};

void settingUpVertices(void) {
	//front
    v0.settingXYZ(.50f, .50f, .50f); 
	v1.settingXYZ(-.50f, .50f, .50f);
	v2.settingXYZ(-.50f, -.50f, .50f); 
	v3.settingXYZ(.50f, -.50f, .50f);

	//back
    v4.settingXYZ(.50f, -.50f, -.50f); 
	v5.settingXYZ(.50f, .50f, -.50f); 
	v6.settingXYZ(-.50f, .50f, -.50f); 
	v7.settingXYZ(-.50f, -.50f, -.50f); 
}

void settingUpTriangles(void) {

	//front
    triArray[0].settingVertices(v1, v2, v3); 
	triArray[1].settingVertices(v3, v0, v1);
    triArray[0].textureTopTriangle();
    
	//right
    triArray[2].settingVertices(v0, v3, v4); 
	triArray[3].settingVertices(v4, v5, v0);
    triArray[2].textureTopTriangle();
    
	//top
    triArray[4].settingVertices(v0, v5, v6);
	triArray[5].settingVertices(v6, v1, v0); 
    triArray[4].textureTopTriangle();
    
	//left
    triArray[6].settingVertices(v6, v7, v2);
	triArray[7].settingVertices(v2, v1, v6);
    triArray[6].textureTopTriangle();
    
	//bottom
    triArray[8].settingVertices(v7, v4, v3);
	triArray[9].settingVertices(v3, v2, v7); 
    triArray[8].textureTopTriangle();
    
	//back
    triArray[10].settingVertices(v5, v4, v7); 
	triArray[11].settingVertices(v7, v6, v5); 
    triArray[10].textureTopTriangle();
}

// Only works for rotating in Y
void transform_eye(void) {
    eyex = 5*cos(theta) + 5*sin(theta);
    eyey = 5;
    eyez = -5*sin(theta) + 5*cos(theta);
    theta = theta + .01;
    //Reset at full rotation to avoid a forever growing double(memory Leak)
    if(theta >= 360.0) {
        theta = 0.0;
    }
}

void draw_triangles(void) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_TRIANGLES);
        for(int i=0; i <= 11; i++) {
            glTexCoord2f(triArray[i].textureOneX, triArray[i].textureOneY);
            glVertex3f(triArray[i].one.x, triArray[i].one.y, triArray[i].one.z);
            
            glTexCoord2f(triArray[i].textureTwoX, triArray[i].textureTwoY);
            glVertex3f(triArray[i].two.x, triArray[i].two.y, triArray[i].two.z);

            glTexCoord2f(triArray[i].textureThreeX, triArray[i].textureThreeY);
            glVertex3f(triArray[i].three.x, triArray[i].three.y, triArray[i].three.z);

        }
    glEnd();
}

void display(void) {
    // rotate eye about y
    transform_eye();
    // set up new view
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 1.0, 1.0, 10000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyex, eyey, eyez, //eye
              0.0, 0.0, 0.0, //target
              0.0, 1.0, 0.0  //up
              );
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    draw_triangles();
    glBindTexture(GL_TEXTURE_2D, 0);
    glutSwapBuffers();
}

void init_mod(void) {
    eyex = 5.0;
    eyey = 5.0;
    eyez = 5.0;
    theta = 0.0;
    settingUpVertices();
    settingUpTriangles();
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, gimp_image.bytes_per_pixel, gimp_image.width, gimp_image.height, 0, GL_RGB, GL_UNSIGNED_BYTE, gimp_image.pixel_data);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, gimp_image.width, gimp_image.height, GL_RGB, GL_UNSIGNED_BYTE, gimp_image.pixel_data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutCreateWindow("simple");
    glutDisplayFunc(display);
    glutIdleFunc(glutPostRedisplay);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    init_mod();
    glutMainLoop();
}
