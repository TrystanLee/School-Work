
#include "GLCommon.h"
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 
// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>       // "smart array"
#include <fstream>      // C++ file I-O library (look like any other stream)
#include "cVAOManager.h"
#include "cMesh.h"
#include "cShaderManager.h"
#include <sstream>

// Global things are here:
//TODO: camera eye to be read from file
glm::vec3 cameraEye;
cVAOManager     gVAOManager;
cShaderManager  gShaderManager;
std::vector<cMesh> g_vecMeshes;
float r = 0; //floats to determine if colouring with hack
float b = 0;
float g = 0;


bool loadCamera(std::string fileName) { //these two method COULD be combined but kept seperate for mutual exclusion
    std::ifstream theFile(fileName.c_str());
    if (!theFile.is_open()) {
        std::cout << "Error: " << fileName << " not found" << std::endl;
        return false;
    }
    std::string line,token;
    std::getline(theFile, line);
    std::stringstream ss(line);
    std::getline(ss,token,','); //reads in camer-eye
    
    float camX = 0;
    float camY = 0;
    float camZ = 0;
    std::getline(ss, token, ','); //reads in camera position x
    camX = std::stof(token);
    std::getline(ss, token, ','); //reads in camera position y
    camY = std::stof(token);
    std::getline(ss, token, ',');// reads in camera position z
    camZ = std::stof(token);

    //std::cout << camX << camY << camZ << std::endl;
    cameraEye = glm::vec3(camX, camY, camZ);
    ss.clear();
    ss.str("");
    theFile.close();
};
void loadModel(std::string fileName, GLuint program) {
    sModelDrawInfo currentModel; //model information
    
    
    //when new model is made just push back onto g_vecMeshes;
    std::ifstream theFile(fileName.c_str());
    if (!theFile.is_open()) {
        std::cout << "Error: " << fileName << " not found" << std::endl;
        exit(-3);
    }
    std::string line, token;
    std::getline(theFile, line); //reads camera data
    std::getline(theFile, line); // reads column names
    

    while (std::getline(theFile, line)) {
        std::stringstream ss(line);
        std::getline(ss, token, ',');// reads in camera position z
        if (gVAOManager.LoadModelIntoVAO(token, currentModel, program))
        {
            std::cout << "Loaded the model: " << currentModel.meshName << std::endl;
        }
        else
        {
            std::cout << "Error: " <<currentModel.meshName << " did not load" << std::endl;
        }

        cMesh currMod;
        currMod.meshName = token; //give the mesh a name

        //position in the world
        std::getline(ss, token, ',');
        currMod.positionXYZ.x = std::stof(token);
        std::getline(ss, token, ',');
        currMod.positionXYZ.y = std::stof(token);
        std::getline(ss, token, ',');
        currMod.positionXYZ.z = std::stof(token);

        //orientating around an axis
        std::getline(ss, token, ',');
        currMod.orientationXYZ.x = std::stof(token);
        std::getline(ss, token, ',');
        currMod.orientationXYZ.y = std::stof(token);
        std::getline(ss, token, ',');
        currMod.orientationXYZ.z = std::stof(token);
        
        //colours
        std::getline(ss, token, ',');
        r = std::stof(token);
        std::getline(ss, token, ',');
        g = std::stof(token);
        std::getline(ss, token, ',');
        b = std::stof(token);
        if (r + g + b) { //if rgb values not 0 then assume coloring
            currMod.bOverriveVertexColourHACK = true;
            currMod.vertexColourOverrideHACK = glm::vec3(r, g, b);
        }
        else {
            currMod.bOverriveVertexColourHACK = false;
        }
        //scale the model
        std::getline(ss, token, ',');
        currMod.scale = std::stof(token);
        //if it is a wireframe
        std::getline(ss, token, ',');
        currMod.bIsWireframe = std::stof(token);


        g_vecMeshes.push_back(currMod);
        ss.str("");
    }
    theFile.close();
};



static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

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
    if (key == GLFW_KEY_1)
    {
        for (unsigned int index = 0; index != g_vecMeshes.size(); index++) {
            g_vecMeshes[index].bIsWireframe = true;
        }
    }
    if (key == GLFW_KEY_2)
    {
        for (unsigned int index = 0; index != g_vecMeshes.size(); index++) {
            g_vecMeshes[index].bIsWireframe = false;
        }
    }
    /*
    if (key == GLFW_KEY_3)
    {
        for (unsigned int index = 0; index != g_vecMeshes.size(); index++) {
            g_vecMeshes[index].bOverriveVertexColourHACK = true;
            g_vecMeshes[index].vertexColourOverrideHACK = glm::vec3(r, g, b);
        }
    }
    if (key == GLFW_KEY_4)
    {
        for (unsigned int index = 0; index != g_vecMeshes.size(); index++) {
            g_vecMeshes[index].bOverriveVertexColourHACK = false;
        }
    }
    */
}



int main(void)
{
    GLFWwindow* window;

//    GLuint vertex_buffer = 0;     // ** NOW in VAO Manager **

    //GLuint vertex_shader;     // Now in the "Shader Manager"
    //GLuint fragment_shader;   // Now in the "Shader Manager"
    GLuint program = 0;     // 0 means "no shader program"

    GLint mvp_location = -1;        // Because glGetAttribLocation() returns -1 on error
//    GLint vpos_location = -1;       // Because glGetAttribLocation() returns -1 on error
//    GLint vcol_location = -1;       // Because glGetAttribLocation() returns -1 on error

    glfwSetErrorCallback(error_callback);

    if ( ! glfwInit() )
    {
        return -1;
        //exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(1200, 640, "OpenGL is great!", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    cShaderManager::cShader vertShader;
    vertShader.fileName = "assets/shaders/vertShader_01.glsl";
        
    cShaderManager::cShader fragShader;
    fragShader.fileName = "assets/shaders/fragShader_01.glsl";

    if (gShaderManager.createProgramFromFile("Shader#1", vertShader, fragShader))
    {
        std::cout << "Shader compiled OK" << std::endl;
        // 
        // Set the "program" variable to the one the Shader Manager used...
        program = gShaderManager.getIDFromFriendlyName("Shader#1");
    }
    else
    {
        std::cout << "Error making shader program: " << std::endl;
        std::cout << gShaderManager.getLastError() << std::endl;
    }

    //GLint mvp_location = -1;
    mvp_location = glGetUniformLocation(program, "MVP");

    // Get "uniform locations" (aka the registers these are in)
    GLint matModel_Location = glGetUniformLocation(program, "matModel");
    GLint matView_Location = glGetUniformLocation(program, "matView");
    GLint matProjection_Location = glGetUniformLocation(program, "matProjection");

    loadCamera("scene.csv");
    loadModel("scene.csv",program);


    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glm::mat4 matModel;    // used to be "m"; Sometimes it's called "world"
        glm::mat4 p;
        glm::mat4 v;
        glm::mat4 mvp;
        //        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        // Turn on the depth buffer
        glEnable(GL_DEPTH);         // Turns on the depth buffer
        glEnable(GL_DEPTH_TEST);    // Check if the pixel is already closer

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // *******************************************************
        // Screen is cleared and we are ready to draw the scene...
        // *******************************************************



        for (unsigned int index = 0; index != g_vecMeshes.size(); index++)
        {
            // So the code is a little easier...
            cMesh curMesh = g_vecMeshes[index];

            matModel = glm::mat4(1.0f);  // "Identity" ("do nothing", like x1)
            //mat4x4_identity(m);


            // *****************************************************
            // Translate or "move" the object somewhere
            glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
                curMesh.positionXYZ);

            //matModel = matModel * matTranslate;
            // *****************************************************


            // *****************************************************
            // Rotation around the Z axis
            glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f),
                curMesh.orientationXYZ.z,//(float)glfwGetTime(),
                glm::vec3(0.0f, 0.0f, 1.0f));

            //matModel = matModel * rotateZ;
            // *****************************************************

            // *****************************************************
            // Rotation around the Y axis
            glm::mat4 rotateY = glm::rotate(glm::mat4(1.0f),
                curMesh.orientationXYZ.y,
                glm::vec3(0.0f, 1.0f, 0.0f));

            //matModel = matModel * rotateY;
            // *****************************************************

            // *****************************************************
            // Rotation around the X axis
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f),
                curMesh.orientationXYZ.x,
                glm::vec3(1.0f, 0.0f, 0.0f));

            //matModel = matModel * rotateX;
            // *****************************************************


            // *****************************************************
            // Scale the model
            glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
                glm::vec3(curMesh.scale,  // Scale in X
                    curMesh.scale,  // Scale in Y
                    curMesh.scale));// Scale in Z

//matModel = matModel * matScale;
// *****************************************************

// *****************************************************
            matModel = matModel * matTranslate;
            matModel = matModel * rotateZ;
            matModel = matModel * rotateY;
            matModel = matModel * rotateX;
            matModel = matModel * matScale;

            // Now the matModel ("Model" or "World") matrix
            //  represents ALL the transformations we want, in ONE matrix.

            p = glm::perspective(0.6f,
                ratio,
                0.1f,
                1000.0f);

            v = glm::mat4(1.0f);


            glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

            v = glm::lookAt(cameraEye,     // "eye"
                cameraTarget,  // "at"
                upVector);
            glUseProgram(program);
            glUniformMatrix4fv(matModel_Location, 1, GL_FALSE, glm::value_ptr(matModel));
            glUniformMatrix4fv(matView_Location, 1, GL_FALSE, glm::value_ptr(v));
            glUniformMatrix4fv(matProjection_Location, 1, GL_FALSE, glm::value_ptr(p));



            // Michael won't push this until later...
            GLint bUseVertexColour_Location = glGetUniformLocation(program, "bUseVertexColour");
            GLint vertexColourOverride_Location = glGetUniformLocation(program, "vertexColourOverride");

            glUniform3f(vertexColourOverride_Location, 0.0f, 1.0f, 1.0f);
            glUniform1f(bUseVertexColour_Location, (float)GL_TRUE);

            // See if mesh is wanting the vertex colour override (HACK) to be used?
            if (curMesh.bOverriveVertexColourHACK)
            {
                // Override the colour...
                glUniform1f(bUseVertexColour_Location, (float)GL_TRUE);
                glUniform3f(vertexColourOverride_Location,
                    curMesh.vertexColourOverrideHACK.r,
                    curMesh.vertexColourOverrideHACK.g,
                    curMesh.vertexColourOverrideHACK.b);
            }
            else
            {
                // DON'T override the colour
                glUniform1f(bUseVertexColour_Location, (float)GL_FALSE);
            }



            // Wireframe
            if (curMesh.bIsWireframe)                // GL_POINT, GL_LINE, and GL_FILL)
            {
                // Draw everything with only lines
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            else
            {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            sModelDrawInfo modelInfo;
            //        if (gVAOManager.FindDrawInfoByModelName("bun_zipper_res2 (justXYZ).ply", modelInfo))
            //        if (gVAOManager.FindDrawInfoByModelName("Assembled_ISS.ply", modelInfo))

            if (gVAOManager.FindDrawInfoByModelName(g_vecMeshes[index].meshName, modelInfo))
            {
                glBindVertexArray(modelInfo.VAO_ID);

                glDrawElements(GL_TRIANGLES,
                    modelInfo.numberOfIndices,
                    GL_UNSIGNED_INT,
                    (void*)0);

                glBindVertexArray(0);
            }


        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}