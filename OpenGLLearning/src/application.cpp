#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main(void)
{
    const float WINDOW_SCALE_FACTOR = 1;

    const float WINDOW_WIDTH = 1280;
    const float WINDOW_HEIGHT = 720;

    GLFWwindow* window;

    // Initialize glfw 
    if (!glfwInit())
        return -1;

    // Enable Anti Aliasing (via MSAA)
    glfwWindowHint(GLFW_SAMPLES, 8);
    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Program", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    // Make the openGL context
    glfwMakeContextCurrent(window);
    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(1);

    // Include the openGL functions using GLEW
    if (glewInit() != GLEW_OK) {
        return -1;
    }
    {
        glm::vec3 cameraPos = glm::vec3(100, 0, 0);
        glm::vec3 objectPos = glm::vec3(0, 0, 0);

        float vertexPositions[] = {
            -0.5f, -0.5f,   // 0
             0.5f, -0.5f,   // 1
             0.5f,  0.5f,   // 2
            -0.5f,  0.5f    // 3
        };

        // Index Buffer Data
        // Note for future: The type for indicies can be any UNSIGNED type (eg. char, short, unsigned int, etc.
        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        // Circling
        float circleRadius = 200;
        float circleCentreX = WINDOW_WIDTH/2;
        float circleCentreY = WINDOW_HEIGHT/2;
        const unsigned int numVerticiesPerCircle = 128;
        const float PI = 3.141592;
        objectPos.x = circleCentreX;
        objectPos.y = circleCentreY;

        // Plus one is for the centre and the two is for the two coordinates
        float circleVertexPositions[4 * (numVerticiesPerCircle + 1)] = {
            0, 0, 0.5, 0.5
        };
        // 3 since there is a triangle for every vertex
        unsigned int circleIndicies[3 * numVerticiesPerCircle];
        const float anglePerVertex = (2 * PI) / (numVerticiesPerCircle);

        for (int i = 0; i < numVerticiesPerCircle; i++) {
            // Add the vertex at the correct x and y pos
            circleVertexPositions[4 * (i + 1)] = (circleRadius * cos(anglePerVertex * i));
            circleVertexPositions[4 * (i + 1) + 1] = (circleRadius * sin(anglePerVertex * i));

            // Add the texture coordinates (+ 1 then / 2 is to convert the range -1 to 1 to 0 to 1)
            circleVertexPositions[4 * (i + 1) + 2] = ((cos(anglePerVertex * i) + 1) / 2);
            circleVertexPositions[4 * (i + 1) + 3] = ((sin(anglePerVertex * i) + 1) / 2);

            // Multiply by three since there are three numbers per index
            circleIndicies[3 * i] = 0;
            circleIndicies[(3 * i) + 1] = i + 1;
            if (i != numVerticiesPerCircle - 1) {
                circleIndicies[(3 * i) + 2] = i + 2;
            }
            // Else condition necessary for wrapping the cirle back around
            else {
                circleIndicies[(3 * i) + 2] = 1;
            }

        }

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;

        // Create a vertex buffer
        VertexBuffer vb(circleVertexPositions, sizeof(circleVertexPositions));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        // Create an index buffer
        IndexBuffer ib(circleIndicies, sizeof(circleIndicies) / sizeof(circleIndicies[0]));

        // Create projection matrix
        glm::mat4 proj = glm::ortho(0.0f, WINDOW_WIDTH/WINDOW_SCALE_FACTOR, 0.0f, WINDOW_HEIGHT/WINDOW_SCALE_FACTOR, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), -cameraPos);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), objectPos);

        glm::mat4 MVP = proj * view * model;

        Shader shader("res/shaders/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Colour", 1.0f, 0.6f, 0.0f, 1.0f);
        shader.SetUniformMat4f("u_MVP", MVP);

        Texture texture("res/textures/AshensignWithBackground.png");
        texture.Bind(0);
        shader.SetUniform1i("u_Texture", 0);

        Renderer renderer;

        float r = 0.0f;
        float g = 0.5f;
        float b = 0.2f;
        float increment_r = 0.05f;
        float increment_g = 0.07f;
        float increment_b = 0.03f;

        float cameraPosIncrement = 2.0f;
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Render here 
            renderer.Clear();

            // Change colour
            //texture.Unbind();
            //shader.SetUniform4f("u_Colour", r, g, b, 1.0f);
            //renderer.Draw(va, ib, shader);

            view = glm::translate(glm::mat4(1.0f), -cameraPos);
            glm::mat4 MVP = proj * view * model;
            shader.SetUniform4f("u_Colour", 0.0f, 0.0f, 0.0f, 0.0f);
            shader.SetUniformMat4f("u_MVP", MVP);
            texture.Bind(0);
            renderer.Draw(va, ib, shader);

            // Colour changing code
            if (r > 1.0f || r < 0.0f) {
                increment_r = -increment_r;
            }
            if (g > 1.0f || g < 0.0f) {
                increment_g = -increment_g;
            }
            if (b > 1.0f || b < 0.0f) {
                increment_b = -increment_b;
            }
            r += increment_r;
            g += increment_g;
            b += increment_b;

            // Movement code
            if (cameraPos.x < -WINDOW_HEIGHT / 2 || cameraPos.x > WINDOW_HEIGHT / 2) {
                cameraPosIncrement = -cameraPosIncrement;
            }

            cameraPos.x += cameraPosIncrement;

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}