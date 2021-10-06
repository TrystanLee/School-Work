#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#include "linmath.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>

glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);

static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"uniform mat4 MVP;\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";


// Moved to sVertex_XYZ_RGB.h
struct sVertex_XYZ_RGB
{
    float x, y, z;  /*z*/
    float r, g, b;
};
//sVertex_XYZ_RGB vertices[6] =
//{     // x     y     +Z+   r     g     b
//    { -0.6f, -0.4f, 0.5f, 1.0f, 0.0f, 0.0f },     // Red
//    {  0.6f, -0.4f, 0.3f, 0.0f, 1.0f, 0.0f },     // Green
//    {  0.0f,  0.6f, 0.1f, 0.0f, 0.0f, 1.0f },      // Blue
//
//    { -1.6f, -0.4f, -0.3f, 1.0f, 0.0f, 0.0f },     // Red
//    {  1.6f, -0.4f, -0.5f, 0.0f, 1.0f, 0.0f },     // Green
//    {  1.0f,  0.6f, -0.7f, 0.0f, 0.0f, 1.0f }      // Blue
//};
//sVertex_XYZ_RGB vertices[6]; //STACK
//sVertex_XYZ_RGB* pVertices = new sVertex_XYZ_RGB[6]; //HEAP
sVertex_XYZ_RGB* pVertices = NULL;


struct sVertex {
    float x, y, z, c, i;
};
struct sTriangle {
    unsigned int vertIndex[3];
};

sVertex ourVertices[453];
sTriangle ourTriangles[948];

bool LoadPlyFile(std::string fileName);




static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    float cameraSpeed = 1.0f;

    // Basic camera controls
    if (key == GLFW_KEY_A)
    {
        cameraEye.x -= cameraSpeed;     // Go left
    }
    if (key == GLFW_KEY_D)
    {
        cameraEye.x += cameraSpeed;     // Go right
    }

    if (key == GLFW_KEY_W)
    {
        cameraEye.z += cameraSpeed;     // Go forward
    }
    if (key == GLFW_KEY_S)
    {
        cameraEye.z -= cameraSpeed;     // Go backwards
    }

    if (key == GLFW_KEY_Q)
    {
        cameraEye.y -= cameraSpeed;     // Go "Down"
    }
    if (key == GLFW_KEY_E)
    {
        cameraEye.y += cameraSpeed;     // Go "Up"
    }
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    if (LoadPlyFile("bun_zipper_res2.ply")) {
        std::cout << "File OKAY" <<std::endl;
    }
    else {
        std::cout << "File loading error" << std::endl;
        return -2;
    }



    // NOTE: OpenGL error checks have been omitted for brevity
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    unsigned int howManyBytes = 8171 * sizeof(sVertex_XYZ_RGB);
    glBufferData(GL_ARRAY_BUFFER, howManyBytes, pVertices, GL_STATIC_DRAW);
    
    
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    mvp_location = glGetUniformLocation(program, "MVP");
    vpos_location = glGetAttribLocation(program, "vPos");
    vcol_location = glGetAttribLocation(program, "vCol");

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
        sizeof(vertices[0]), (void*)(sizeof(float) * 2));
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        //       mat4x4 m, p, mvp;
        glm::mat4 m, p, v, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        //         mat4x4_identity(m);
        m = glm::mat4(1.0f);

        //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
            (float)glfwGetTime(),
            glm::vec3(0.0f, 0.0, 1.0f));

        m = m * rotateZ;

        //mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        p = glm::perspective(0.6f,
            ratio,
            0.1f,
            1000.0f);


        //moving the camera
        v = glm::mat4(1.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
        v = glm::lookAt(cameraEye,
            cameraTarget,
            upVector);

        
        mvp = p * v * m;


        glUseProgram(program);


        //glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));

        glDrawArrays(GL_TRIANGLES, 0, 8171*3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

bool LoadPlyFile(std::string fileName) {
    std::ifstream theFile(fileName.c_str());
    if (!theFile.is_open()) {
        std::cout << "ERROR: File name not found" << std::endl;
        return false;
    }

    std::string nextToken;

    while(theFile >> nextToken){
        std::cout << nextToken << std::endl;
        if (nextToken == "vertex") {
            break;
        }
    }
    unsigned int numVertices = 0;
    theFile >> numVertices;

    while (theFile >> nextToken) {
        std::cout << nextToken << std::endl;
        if (nextToken == "face") {
            break;
        }
    }
    unsigned int numTriangles = 0;
    theFile >> numTriangles;

    while (theFile >> nextToken) {
        std::cout << nextToken << std::endl;
        if (nextToken == "end_header") {
            break;
        }
    }
    std::vector<sVertex> vecVertexArray;
    for (unsigned int index = 0; index < numVertices; index++) {
        sVertex tempVertex;
        theFile >> tempVertex.x;
        theFile >> tempVertex.y;
        theFile >> tempVertex.z;
        theFile >> tempVertex.c;
        theFile >> tempVertex.i;
        vecVertexArray.push_back(tempVertex);
    }
    std::vector<sTriangle> vecTriangleArray;
    for (unsigned int index = 0; index < numTriangles; index++) {
        sTriangle tempTri;
        int discardThis;
        theFile >> discardThis;
        theFile >> tempTri.vertIndex[0];
        theFile >> tempTri.vertIndex[1];
        theFile >> tempTri.vertIndex[2];
        vecTriangleArray.push_back(tempTri);
    }
    unsigned int toAllocate = numVertices;
    sVertex_XYZ_RGB* pVertices = new sVertex_XYZ_RGB[toAllocate];
    for (unsigned int index = 0; index < numVertices; index++) {
        pVertices[index].x = vecVertexArray[index].x;
        pVertices[index].y = vecVertexArray[index].y;
        pVertices[index].z = vecVertexArray[index].z;

        pVertices[index].r = 1.0f;
        pVertices[index].g = 1.0f;
        pVertices[index].b = 1.0f;
    }
    return true;
};