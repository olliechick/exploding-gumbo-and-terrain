//  ========================================================================
//  COSC422: Advanced Computer Graphics;  University of Canterbury.
//
//  Assignment 1
//
//  Written by Ollie Chick
//
//  Adapted from COSC422 labs
//  ========================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Scene.h"

using namespace std;

GLuint vaoID;
GLuint theProgram;
GLuint matrixLoc;
float angle = 0.0;
glm::mat4 projView;
float CDR = 3.14159265 / 180.0;   //Conversion from degrees to radians (required in GLM 0.9.6 and later versions)

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


void initialise()
{
    glm::mat4 proj, view;
    GLuint shaderv = loadShader(GL_VERTEX_SHADER, "shaders/Scene.vert");
    GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "shaders/Scene.tesc");
    GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "shaders/Scene.tese");
    GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "shaders/Scene.frag");

    GLuint program = glCreateProgram();
    glAttachShader(program, shaderv);
    glAttachShader(program, shaderc);
    glAttachShader(program, shadere);
    glAttachShader(program, shaderf);
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

    matrixLoc = glGetUniformLocation(program, "mvpMatrix");

    proj = glm::perspective(20.0f * CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
    view = glm::lookAt(glm::vec3(0.0, 5.0, 12.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)); //view matrix
    projView = proj * view;  //Product matrix

    GLuint vboID[4];

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenVertexArrays(2, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void update(int value)
{
    angle++;
    glutTimerFunc(50, update, 0);
    glutPostRedisplay();
}

void display()
{
    glm::mat4 matrix = glm::mat4(1.0);
    matrix = glm::rotate(matrix, angle * CDR, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix
    glm::mat4 prodMatrix = projView * matrix;        //Model-view-proj matrix

    glUniformMatrix4fv(matrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vaoID);
    glDrawElements(GL_PATCHES, 24, GL_UNSIGNED_SHORT, NULL);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Cube with Bezier patches");
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
    glutDisplayFunc(display);
    glutTimerFunc(50, update, 0);
    glutMainLoop();
}

