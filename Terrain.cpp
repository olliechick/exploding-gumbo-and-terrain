//  ========================================================================
//  COSC422: Computer Graphics (2018);  University of Canterbury.
//
//  FILE NAME: Terrain.cpp
//  This is part of Assignment1 files.
//
//	The program generates and loads the mesh data for a terrain floor (100 verts, 81 elems).
//  Required files:  Terrain.vert (vertex shader), Terrain.frag (fragment shader), HeightMap1.tga  (height map)
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "res/loadTGA.h"

#define PI 3.14159265358979
#define ANGLE_INCREMENT PI / 90
#define SCALE_STEP 0.01

using namespace std;

GLuint vaoID;
GLuint mvpMatrixLoc, eyeLoc, heightMapLoc, lightLoc, grassLoc, waterLoc, snowLoc, scaleLoc;

float angle = 0;
bool autoRotate = false;

float waterScale, grassScale, grassSnowScale;

float CDR = 3.14159265 / 180.0;     //Conversion from degrees to rad (required in GLM 0.9.6)

float verts[100 * 3];       //10x10 grid (100 vertices)
GLushort elems[81 * 4];       //Element array for 81 quad patches

glm::mat4 proj, view, projView;
glm::vec3 eye, lookAt;
float cam_angle = 0, d;
GLenum mode = GL_FILL;

float defaultWaterScale = 0.25;
float defaultGrassScale = 0.6;
float defaultGrassSnowScale = 0.7;

//Generate vertex and element data for the terrain floor
void generateData()
{
    int indx, start;
    //verts array
    for (int i = 0; i < 10; i++)   //100 vertices on a 10x10 grid
    {
        for (int j = 0; j < 10; j++) {
            indx = 10 * i + j;
            verts[3 * indx] = 10 * i - 45;        //x  varies from -45 to +45
            verts[3 * indx + 1] = 0;            //y  is set to 0 (ground plane)
            verts[3 * indx + 2] = -10 * j;        //z  varies from 0 to -100
        }
    }

    //elems array
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            indx = 9 * i + j;
            start = 10 * i + j;
            elems[4 * indx] = start;
            elems[4 * indx + 1] = start + 10;
            elems[4 * indx + 2] = start + 11;
            elems[4 * indx + 3] = start + 1;
        }
    }
}

//Loads terrain texture
void loadTextures()
{
    GLuint texID[5];
    glGenTextures(5, texID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID[0]);
    loadTGA("res/HeightMap1.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texID[1]);
    loadTGA("res/HeightMap2.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texID[2]);
    loadTGA("res/grass.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texID[3]);
    loadTGA("res/water.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texID[4]);
    loadTGA("res/snow.tga");

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

//Loads a shader file and returns the reference to a shader object
GLuint loadShader(GLenum shaderType, string filename)
{
    ifstream shaderFile(filename.c_str());
    if (!shaderFile.good()) cout << "Error opening shader file." << endl;
    stringstream shaderData;
    shaderData << shaderFile.rdbuf();
    shaderFile.close();
    string shaderStr = shaderData.str();
    const char *shaderTxt = shaderStr.c_str();

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderTxt, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);
        cerr << "Compile failure in shader: " << strInfoLog << endl;
        delete[] strInfoLog;
    }
    return shader;
}

void calculate_d()
{
    float deltax = eye.x;
    float deltaz = eye.z;
    d = sqrt(deltax * deltax + deltaz * deltaz);
}

float keep_in_0_to_1_range(float in)
{
    if (in < 0) in = 0;
    if (in > 1) in = 1;
    return in;
}

void handleKeyboardInput(unsigned char key, int x, int y)
{
    if (key == 'w') {
        if (mode == GL_FILL) mode = GL_LINE;
        else mode = GL_FILL;
    } else if (key == 'r') { // Totally reset scene
        cam_angle = 0;
        eye = glm::vec3(0.0, 20.0, 30.0);
        calculate_d();
        glUniform1i(heightMapLoc, 0);
        mode = GL_FILL;
        angle = 0;
        autoRotate = false;
        waterScale = defaultWaterScale;
        grassScale = defaultGrassScale;
        grassSnowScale = defaultGrassSnowScale;
    } else if (key == '1') {
        glUniform1i(heightMapLoc, 0);
    } else if (key == '2') {
        glUniform1i(heightMapLoc, 1);
    } else if (key == 'a') {
        angle -= ANGLE_INCREMENT;
    } else if (key == 'd') {
        angle += ANGLE_INCREMENT;
    } else if (key == 's') {
        autoRotate = !autoRotate;
    } else if (key == 'o') {
        waterScale -= SCALE_STEP;
        if (grassScale > grassSnowScale - 0.1) {
            if (grassSnowScale - 0.1 < waterScale) grassScale = waterScale;
            else grassScale = grassSnowScale - 0.1;
        }
    } else if (key == 'p') {
        waterScale += SCALE_STEP;
        if (grassScale < waterScale) grassScale = waterScale;
        if (grassSnowScale < waterScale) grassSnowScale = waterScale;
    } else if (key == 'k') {
        grassSnowScale -= SCALE_STEP;
        if (waterScale > grassSnowScale) waterScale = grassSnowScale;
        if (grassScale > grassSnowScale) grassScale = grassSnowScale;
        grassScale = grassSnowScale - 0.1;
    } else if (key == 'l') {
        grassSnowScale += SCALE_STEP;
        grassScale = grassSnowScale - 0.1;
    }

    waterScale = keep_in_0_to_1_range(waterScale);
    grassScale = keep_in_0_to_1_range(grassScale);
    grassSnowScale = keep_in_0_to_1_range(grassSnowScale);

    if (grassScale < waterScale) waterScale;


    glutPostRedisplay();
}

void handleSpecialInput(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        eye.x += sin(cam_angle);
        eye.z -= cos(cam_angle);
    } else if (key == GLUT_KEY_DOWN) {
        eye.x -= sin(cam_angle);
        eye.z += cos(cam_angle);
    } else if (key == GLUT_KEY_PAGE_UP) {
        eye.x += 10 * sin(cam_angle);
        eye.z -= 10 * cos(cam_angle);
    } else if (key == GLUT_KEY_PAGE_DOWN) {
        eye.x -= 10 * sin(cam_angle);
        eye.z += 10 * cos(cam_angle);
    }

    calculate_d();
    glutPostRedisplay();
}

//Initialise the shader program, create and load buffer data
void initialise()
{
//--------Load terrain height map-----------
    loadTextures();
//--------Load shaders----------------------
    GLuint shaderv = loadShader(GL_VERTEX_SHADER, "shaders/Terrain.vert");
    GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "shaders/Terrain.frag");
    GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "shaders/Terrain.tesc");
    GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "shaders/Terrain.tese");
    GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "shaders/Terrain.geom");

    GLuint program = glCreateProgram();
    glAttachShader(program, shaderv);
    glAttachShader(program, shaderf);
    glAttachShader(program, shaderc);
    glAttachShader(program, shadere);
    glAttachShader(program, shaderg);
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
    }
    glUseProgram(program);

    mvpMatrixLoc = glGetUniformLocation(program, "mvpMatrix");
    lightLoc = glGetUniformLocation(program, "light");

    heightMapLoc = glGetUniformLocation(program, "heightMap");
    glUniform1i(heightMapLoc, 0);

    grassLoc = glGetUniformLocation(program, "grass");
    glUniform1i(grassLoc, 2);
    waterLoc = glGetUniformLocation(program, "water");
    glUniform1i(waterLoc, 3);
    snowLoc = glGetUniformLocation(program, "snow");
    glUniform1i(snowLoc, 4);

    scaleLoc = glGetUniformLocation(program, "scale");

//--------Compute matrices----------------------
    proj = glm::perspective(30.0f * CDR, 1.25f, 20.0f, 500.0f);  //perspective projection matrix
    eye = glm::vec3(0.0, 20.0, 30.0);

    waterScale = defaultWaterScale;
    grassScale = defaultGrassScale;
    grassSnowScale = defaultGrassSnowScale;

//---------Load buffer data-----------------------
    generateData();

    GLuint vboID[2];
    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenBuffers(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    glutKeyboardFunc(handleKeyboardInput);
    glutSpecialFunc(handleSpecialInput);
}

void update(int _)
{
    if (autoRotate) {
        angle += ANGLE_INCREMENT;
        glutPostRedisplay();
    }

    glutTimerFunc(50, update, 0);
}

//Display function to compute uniform values based on transformation parameters and to draw the scene
void display()
{
    lookAt = glm::vec3(eye.x + 100 * sin(cam_angle), eye.y - 20, eye.z - 100 * cos(cam_angle) + 30);
    view = glm::lookAt(eye, lookAt, glm::vec3(0.0, 1.0, 0.0)); //view matrix
    projView = proj * view;  //Product (mvp) matrix
    glm::vec4 light = normalize(glm::vec4(50 * sin(angle), 50 * cos(angle), -50, 0));

    glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &projView[0][0]);
    glUniform3fv(eyeLoc, 1, &eye[0]);
    glUniform4fv(lightLoc, 1, &light[0]);

    glm::vec3 scale = glm::vec3(waterScale, grassScale, grassSnowScale);
    glUniform3fv(scaleLoc, 1, &scale[0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, mode);
    glBindVertexArray(vaoID);
    glDrawElements(GL_PATCHES, 81 * 4, GL_UNSIGNED_SHORT, NULL);
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_DEPTH);
    glutInitWindowSize(1000, 800);
    glutCreateWindow("COSC422 assignment 1 part 2: Ollie Chick");
    glutInitContextVersion(4, 2);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    if (glewInit() == GLEW_OK) {
        cout << "GLEW initialization successful! " << endl;
        cout << " Using GLEW version " << glewGetString(GLEW_VERSION) << endl;
    } else {
        cerr << "Unable to initialize GLEW  ...exiting." << endl;
        exit(EXIT_FAILURE);
    }

    initialise();
    display();
    glutDisplayFunc(display);
    glutTimerFunc(50, update, 0);
    glutMainLoop();
}

