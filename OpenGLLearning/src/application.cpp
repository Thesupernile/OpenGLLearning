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

int main(void)
{
    GLFWwindow* window;

    // Initialize glfw 
    if (!glfwInit())
        return -1;

    // Enable Anti Aliasing (via MSAA)
    glfwWindowHint(GLFW_SAMPLES, 8);
    // Create a windowed mode window and its OpenGL context 
    window = glfwCreateWindow(720, 720, "Program", NULL, NULL);
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
        unsigned int circleRadius = 0.5;
        float circleCentreX = 0.0;
        float circleCentreY = 0.0;
        const unsigned int numVerticiesPerCircle = 128;
        const float PI = 3.141592;

        // Plus one is for the centre and the two is for the two coordinates
        float circleVertexPositions[2 * (numVerticiesPerCircle + 1)] = {
            circleCentreX, circleCentreY
        };
        // 3 since there is a triangle for every vertex
        unsigned int circleIndicies[3 * numVerticiesPerCircle];
        const float anglePerVertex = (2 * PI) / (numVerticiesPerCircle);

        for (int i = 0; i < numVerticiesPerCircle; i++) {
            // Add the vertex at the correct x and y pos
            circleVertexPositions[2 * (i + 1)] = (0.5 * cos(anglePerVertex * i));
            circleVertexPositions[2 * (i + 1) + 1] = (0.5 * sin(anglePerVertex * i));

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

        VertexArray va;

        // Create a vertex buffer
        VertexBuffer vb(circleVertexPositions, sizeof(circleVertexPositions));

        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        // Create an index buffer
        IndexBuffer ib(circleIndicies, sizeof(circleIndicies) / sizeof(circleIndicies[0]));

        Shader shader("res/shaders/basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Colour", 1.0f, 0.6f, 0.0f, 1.0f);

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