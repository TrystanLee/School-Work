
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

// Global things are here:
glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);
cVAOManager     gVAOManager;
cShaderManager  gShaderManager;
std::vector<cMesh> g_vecMeshes;

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

    sModelDrawInfo modelBunny;
//    if (gVAOManager.LoadModelIntoVAO("bun_zipper_res2 (justXYZ).ply", modelBunny, program))
    if (gVAOManager.LoadModelIntoVAO("bun_zipper_res2_xyz_n_rgb.ply", modelBunny, program))
    {
        std::cout << "Loaded the model: " << modelBunny.meshName << std::endl;
        std::cout << modelBunny.numberOfVertices << " vertices loaded" << std::endl;
        std::cout << modelBunny.numberOfTriangles << " triangles loaded" << std::endl;
    }
    else
    {
        std::cout << "Error: Didn't load the model OK" << std::endl;
    }

    sModelDrawInfo modelISS;
    if (gVAOManager.LoadModelIntoVAO("Assembled_ISS_xyz_rgba_n.ply", modelISS, program))
    {
        std::cout << "Loaded the model OK" << std::endl;
    }
    else
    {
        std::cout << "Error: Didn't load the model OK" << std::endl;
    }

    sModelDrawInfo hogwartsModel;
    if ( ! gVAOManager.LoadModelIntoVAO("Hogwarts_xyz_n_rgba (x0.1 scale).ply", hogwartsModel, program) ) 
    {
        std::cout << "Didn't load hogwards." << std::endl;
    }

    // Add the models I'd like to draw in the scene

    cMesh bunny1;
    bunny1.meshName = "bun_zipper_res2_xyz_n_rgb.ply";
    bunny1.positionXYZ.x = 1.0f;
    bunny1.scale = 5.0f;
    bunny1.bOverriveVertexColourHACK = true;
    bunny1.vertexColourOverrideHACK = glm::vec3(1.0f, 0.0f, 0.0f);

    cMesh bunny2;
    bunny2.meshName = "bun_zipper_res2_xyz_n_rgb.ply";
    bunny2.positionXYZ.y = -1.0f;
    bunny2.orientationXYZ.z = 0.75f;    // Radians

    cMesh TheISS;
    TheISS.meshName = "Assembled_ISS_xyz_rgba_n.ply";
    TheISS.orientationXYZ.z = 0.5f; // Radians

    cMesh hogwarts;
    hogwarts.meshName = "Hogwarts_xyz_n_rgba (x0.1 scale).ply";
    hogwarts.bIsWireframe = false;
    hogwarts.scale = 0.01f;
    hogwarts.positionXYZ.y = -2.0f;
    hogwarts.bIsWireframe = true;
    hogwarts.bOverriveVertexColourHACK = true;
    hogwarts.vertexColourOverrideHACK = glm::vec3(0.0f, 1.0f, 0.0f);

    //
    //TheISS.bIsWireframe = true;

    g_vecMeshes.push_back(bunny1);
    g_vecMeshes.push_back(bunny2);
    g_vecMeshes.push_back(TheISS);
    g_vecMeshes.push_back(hogwarts);


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

            //glm::vec3 cameraEye = glm::vec3(0.0, 0.0, -4.0f);
            glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);

            v = glm::lookAt(cameraEye,     // "eye"
                cameraTarget,  // "at"
                upVector);
            //        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);

            //        mvp = p * v * matModel;
            //        mat4x4_mul(mvp, p, m);

            glUseProgram(program);

            //          glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
                //        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*)mvp);

                        // Don't need this anymore as it's being done inside the shader:
                        // 
                        // mvp = p * v * matModel;
                        //
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
