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

GLuint vaoID, vaoIDFloor;
GLuint mvMatrixLoc, mvpMatrixLoc, norMatrixLoc, lgtLoc, materialLoc, dLoc, mvpMatrixLoc2;
GLenum mode = GL_FILL;
GLuint program1, program2;
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

void checkProgram(GLuint program)
{
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
}

void handleKeyboardInput(unsigned char key, int x, int y)
{
    if (key == 'w') {
        if (mode == GL_FILL) mode = GL_LINE;
        else mode = GL_FILL;
        glPolygonMode(GL_FRONT_AND_BACK, mode);
    } else if (key == ' ') material.r += 0.05;

    glutPostRedisplay();
}

void handleSpecialInput(int key, int x, int y)
{
    glUseProgram(program1);

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

    glutPostRedisplay();
}

void initialise()
{

    // Model

    program1 = glCreateProgram();

    // Load and attach shaders
    GLuint shaderv = loadShader(GL_VERTEX_SHADER, "shaders/Scene.vert");
    GLuint shaderc = loadShader(GL_TESS_CONTROL_SHADER, "shaders/Scene.tesc");
    GLuint shadere = loadShader(GL_TESS_EVALUATION_SHADER, "shaders/Scene.tese");
    GLuint shaderf = loadShader(GL_FRAGMENT_SHADER, "shaders/Scene.frag");
    GLuint shaderg = loadShader(GL_GEOMETRY_SHADER, "shaders/Scene.geom");
    glAttachShader(program1, shaderv);
    glAttachShader(program1, shaderc);
    glAttachShader(program1, shadere);
    glAttachShader(program1, shaderf);
    glAttachShader(program1, shaderg);
    glLinkProgram(program1);
    checkProgram(program1);
    glUseProgram(program1);

    // Uniform variables
    mvpMatrixLoc = glGetUniformLocation(program1, "mvpMatrix");
    mvMatrixLoc = glGetUniformLocation(program1, "mvMatrix");
    norMatrixLoc = glGetUniformLocation(program1, "norMatrix");
    lgtLoc = glGetUniformLocation(program1, "lightPos");
    materialLoc = glGetUniformLocation(program1, "material");
    dLoc = glGetUniformLocation(program1, "d");

    // Matrices and vectors
    proj = glm::perspective(20.0f * CDR, 1.0f, 10.0f, 1000.0f);  //perspective projection matrix
    eye = glm::vec3(0.0, 10.0, 100.0);
    lookAt = glm::vec3(eye.x + 100 * sin(cam_angle), eye.y, eye.z - 100 * cos(cam_angle));
    material = glm::vec4(0, 0.8, 0.8, 1);

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

    glGenVertexArrays(1, &vaoID);
    glBindVertexArray(vaoID);

    GLuint vboID[4];
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

    // Floor

    program2 = glCreateProgram();

    // Load and attach shaders
    GLuint shaderfloorv = loadShader(GL_VERTEX_SHADER, "shaders/Floor.vert");
    GLuint shaderfloorf = loadShader(GL_FRAGMENT_SHADER, "shaders/Floor.frag");
    glAttachShader(program2, shaderfloorv);
    glAttachShader(program2, shaderfloorf);
    glLinkProgram(program2);
    checkProgram(program2);
    glUseProgram(program2);

    // Uniform variables
    mvpMatrixLoc2 = glGetUniformLocation(program2, "mvpMatrix");

    // Temp data
    static const GLfloat g_vertex_buffer_data[] = {
            20.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
    };

    glGenVertexArrays(1, &vaoIDFloor);
    glBindVertexArray(vaoIDFloor);

    GLuint vboIDFloor[4];
    glGenVertexArrays(1, vboIDFloor);

    glBindBuffer(GL_ARRAY_BUFFER, vboIDFloor[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);  // Vertex position

    glBindVertexArray(0);

    // Handle input

    glutKeyboardFunc(handleKeyboardInput);
    glutSpecialFunc(handleSpecialInput);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Matrices
    view = glm::lookAt(eye, lookAt, glm::vec3(0.0, 1.0, 0.0)); //view matrix
    projView = proj * view;  //Product matrix
    glm::mat4 mvMatrix = glm::mat4(1.0);
    mvMatrix = glm::rotate(mvMatrix, CDR, glm::vec3(0.0, 1.0, 0.0));  //rotation matrix
    glm::mat4 prodMatrix = projView * mvMatrix;        //Model-view-proj matrix
    glm::mat4 invMatrix = glm::inverse(mvMatrix);  //Inverse of model-view matrix for normal transformation
    mvMatrix = view * mvMatrix;
    glm::vec4 light = glm::vec4(20.0, 10.0, 20.0, 1.0);
    glm::vec4 lightEye = view * light;     //Light position in eye coordinates

    // Model

    glUseProgram(program1);

    material = glm::vec4(0, 0.8, 0.8, 1);

    // Uniform variables
    glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, &prodMatrix[0][0]);
    glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, &mvMatrix[0][0]);
    glUniformMatrix4fv(norMatrixLoc, 1, GL_TRUE, &invMatrix[0][0]);  //Use transpose matrix here
    glUniform4fv(lgtLoc, 1, &lightEye[0]);
    glUniform4fv(materialLoc, 1, &material[0]);
    glUniform1f(dLoc, d);

    // Actual drawing
    glBindVertexArray(vaoID);
    glDrawArrays(GL_PATCHES, 0, 2048);
    glPatchParameteri(GL_PATCH_VERTICES, 16);

    // Floor (well, just a little line for now)

    glUseProgram(program2);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindVertexArray(vaoIDFloor);

    glVertexAttribPointer(
            0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            NULL           // array buffer offset
    );

    // Uniform variables
    glUniformMatrix4fv(mvpMatrixLoc2, 1, GL_FALSE, &prodMatrix[0][0]);

    // Draw the line !
    glDrawArrays(GL_LINES, 0, 2); // 2 indices for the 2 end points of 1 line


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
    display();
    glutDisplayFunc(display);
    glutMainLoop();
}

