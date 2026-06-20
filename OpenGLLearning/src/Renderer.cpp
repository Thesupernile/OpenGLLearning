#include "Renderer.h"
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line) {
    // Self Note: This works until error is zero (therefore until all errors are cleared)
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] Error Code: " << error << "\nFunction: " << function << " \nLine: " << line << " \nFile: " << file << std::endl;
        return false;
    }
    return true;
}
