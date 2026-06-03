#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};


static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader!\n";
        std::cout << message << std::endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

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

    // Include the openGL functions using GLEW
    if (glewInit() != GLEW_OK) {
        return -1;
    }

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


    // Create a vertex buffer
    unsigned int bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertexPositions), &circleVertexPositions, GL_STATIC_DRAW);

    // Define and enable the vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (const void*)0);

    // Create an index buffer
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(circleIndicies), &circleIndicies, GL_STATIC_DRAW);

    ShaderProgramSource source = ParseShader("res/shaders/basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Render here 
        glClear(GL_COLOR_BUFFER_BIT);

        // Nullptr allowed since index buffer has already been bound
        glDrawElements(GL_TRIANGLES, numVerticiesPerCircle * 3, GL_UNSIGNED_INT, nullptr);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}