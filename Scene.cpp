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

using namespace std;

GLuint vaoID;
GLuint mvMatrixLoc, mvpMatrixLoc, norMatrixLoc, lgtLoc, materialLoc, dLoc;
GLenum mode = GL_FILL;
float cam_angle, look_angle = 0;
glm::mat4 proj, view, projView;
glm::vec4 material;
glm::vec3 eye, lookAt;
float d;
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


void handleKeyboardInput(unsigned char key, int x, int y)
{
    if (key == 'w') {
        if (mode == GL_FILL) mode = GL_LINE;
        else mode = GL_FILL;
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    } else if (key == ' ') material.r += 0.05;
}

void handleSpecialInput(int key, int x, int y)
{
    if (key == GLUT_KEY_UP) {
        eye.x += sin(cam_angle);
        eye.z -= cos(cam_angle);
    } else if (key == GLUT_KEY_DOWN) {
        eye.x -= sin(cam_angle);
        eye.z += cos(cam_angle);
    }

    float deltax = eye.x;
    float deltaz = eye.z;
    d = sqrt(deltax * deltax + deltaz * deltaz);
    cout << d << "\n";
    lookAt = glm::vec3(eye.x + 100 * sin(cam_angle), eye.y, eye.z - 100 * cos(cam_angle));
}


void initialise()
{
    GLuint shaderv = loadShader(GL_VERTEX_SHADER, "shaders/Scene.vert");
    GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "shaders/Scene.tesc");
    GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "shaders/Scene.tese");
    GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "shaders/Scene.frag");
    GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "shaders/Scene.geom");

    GLuint program = glCreateProgram();
    glAttachShader(program, shaderv);
    glAttachShader(program, shaderc);
    glAttachShader(program, shadere);
    glAttachShader(program, shaderf);
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
    mvMatrixLoc = glGetUniformLocation(program, "mvMatrix");
    norMatrixLoc = glGetUniformLocation(program, "norMatrix");
    lgtLoc = glGetUniformLocation(program, "lightPos");
    materialLoc = glGetUniformLocation(program, "material");
    dLoc = glGetUniformLocation(program, "d");

    proj = glm::perspective(20.0f * CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
    eye = glm::vec3(0.0, 10.0, 100.0);
    lookAt = glm::vec3(eye.x + 100 * sin(cam_angle), eye.y, eye.z - 100 * cos(cam_angle));

    //Read coordinates from file
    ifstream infile;
    infile.open("PatchFiles/PatchVerts_Gumbo.txt", ios::in);
    int nvert;
    infile >> nvert;
    float verts[nvert * 3];
    for (int i = 0; i < nvert; i++) {
        infile >> verts[3 * i] >> verts[3 * i + 1] >> verts[3 * i + 2];
    }
    infile.close();

    GLuint vboID[4];

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    glGenVertexArrays(1, vboID);

    glBindBuffer(GL_ARRAY_BUFFER, vboID[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    material = glm::vec4(0, 0.8, 0.8, 1);
    glutKeyboardFunc(handleKeyboardInput);
    glutSpecialFunc(handleSpecialInput);
}

void update(int value)
{
    glutTimerFunc(50, update, 0);
    glutPostRedisplay();
}

void display()
{
    view = glm::lookAt(eye, lookAt, glm::vec3(0.0, 1.0, 0.0)); //view matrix
    projView = proj * view;  //Product matrix

    glm::mat4 mvMatrix = glm::mat4(1.0);
    mvMatrix = glm::rotate(mvMatrix, CDR, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix
    glm::mat4 prodMatrix = projView * mvMatrix;        //Model-view-proj matrix
    glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation
    mvMatrix = view * mvMatrix;

    glm::vec4 light = glm::vec4(20.0, 10.0, 20.0, 1.0);
    glm::vec4 lightEye = view * light;     //Light position in eye coordinates

    glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);
    glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here
    glUniform4fv(lgtLoc, 1, &lightEye[0]);
    glUniform4fv(materialLoc, 1, &material[0]);
    glUniform1f(dLoc, d);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindVertexArray(vaoID);
    glDrawArrays(GL_PATCHES, 0, 2048);
    glPatchParameteri(GL_PATCH_VERTICES, 16);
    glFlush();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("COSC422: Ollie Chick");
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

