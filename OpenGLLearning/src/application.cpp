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
    const float WINDOW_ASPECT_WIDTH = 16;
    const float WINDOW_ASPECT_HEIGHT = 9;
    const float WINDOW_SCALE_FACTOR = 4;

    const float WINDOW_WIDTH = 1280;
    const float WINDOW_HEIGHT = (WINDOW_WIDTH / WINDOW_ASPECT_WIDTH) * WINDOW_ASPECT_HEIGHT;

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
        float circleRadius = 0.7;
        float circleCentreX = 0.0;
        float circleCentreY = 0.0;
        const unsigned int numVerticiesPerCircle = 128;
        const float PI = 3.141592;

        // Plus one is for the centre and the two is for the two coordinates
        float circleVertexPositions[4 * (numVerticiesPerCircle + 1)] = {
            circleCentreX, circleCentreY, 0.5, 0.5
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
        glm::mat4 proj = glm::ortho(-WINDOW_ASPECT_WIDTH/WINDOW_SCALE_FACTOR, WINDOW_ASPECT_WIDTH/WINDOW_SCALE_FACTOR, -WINDOW_ASPECT_HEIGHT/WINDOW_SCALE_FACTOR, WINDOW_ASPECT_HEIGHT/WINDOW_SCALE_FACTOR, -1.0f, 1.0f);

        Shader shader("res/shaders/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Colour", 1.0f, 0.6f, 0.0f, 1.0f);
        shader.SetUniformMat4f("u_MVP", proj);

        Texture texture("res/textures/AshensignWithBackground.png");
        texture.Bind(0);
        shader.SetUniform1i("u_Texture", 0);

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;
        // Loop until the user closes the window
        while (!glfwWindowShouldClose(window))
        {
            // Render here 
            renderer.Clear();

            // Change colour
            shader.Bind();
            shader.SetUniform4f("u_Colour", r, 0.6f, 0.0f, 1.0f);
            texture.Unbind();

            renderer.Draw(va, ib, shader);

            shader.SetUniform4f("u_Colour", 0.0f, 0.0f, 0.0f, 0.0f);
            texture.Bind(0);
            renderer.Draw(va, ib, shader);

            if (r > 1.0f) {
                increment = -0.05f;
            }
            else if (r < 0.0f) {
                increment = 0.05f;
            }
            r += increment;

            // Swap front and back buffers
            glfwSwapBuffers(window);

            // Poll for and process events
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}