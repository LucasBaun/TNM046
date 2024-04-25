/*
 * A C++ framework for OpenGL programming in TNM046 for MT1
 *
 * This is a small, limited framework, designed to be easy to use for students in an introductory
 * computer graphics course in the first year of a M Sc curriculum. It uses custom code for some
 * things that are better solved by external libraries like GLM, but the emphasis is on simplicity
 * andreadability, not generality.
 *
 * For the window management, GLFW 3.x is used for convenience.
 * The framework should work in Windows, MacOS X and Linux.
 * GLEW is used for handling platform specific OpenGL extensions.
 * This code depends only on GLFW, GLEW, and OpenGL libraries.
 * OpenGL 3.3 or higher is required.
 *
 * Authors: Stefan Gustavson (stegu@itn.liu.se) 2013-2015
 *          Martin Falk (martin.falk@liu.se) 2021
 *
 * This code is in the public domain.
 */
#if defined(WIN32) && !defined(_USE_MATH_DEFINES)
#define _USE_MATH_DEFINES
#endif

// File and console I/O for logging and error reporting
#include <iostream>
// Math header for trigonometric functions
#include <cmath>

#include "Utilities.hpp"

// glew provides easy access to advanced OpenGL functions and extensions
#include <GL/glew.h>

// GLFW 3.x, to handle the OpenGL window
#include <GLFW/glfw3.h>

#include "Utilities.hpp"
#include "Shader.hpp"
#include <vector>
#include <array>
#include "TriangleSoup.hpp"


Shader myShader;

GLuint createVertexBuffer(int location, int dimensions, const std::vector<float>& vertices) {
    GLuint bufferID;
    // Generate buffer, activate it and copy the data
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    // Tell OpenGL how the data is stored in our buffer
    // Attribute location (must match layout(location=#) statement in shader)
    // Number of dimensions (3 -> vec3 in the shader, 2-> vec2 in the shader),
    // type GL_FLOAT, not normalized, stride 0, start at element 0
    glVertexAttribPointer(location, dimensions, GL_FLOAT, GL_FALSE, 0, nullptr);
    // Enable the attribute in the currently bound VAO
    glEnableVertexAttribArray(location);

    return bufferID;
}

GLuint createIndexBuffer(const std::vector<unsigned int>& indices) {
    GLuint bufferID;
    // Generate buffer, activate it and copy the data
    glGenBuffers(1, &bufferID);
    // Activate (bind) the index buffer and copy data to it.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
    // Present our vertex indices to OpenGL
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(),
                 GL_STATIC_DRAW);

    return bufferID;
}

std::array<GLfloat, 16> rotX(float theta) {
    return {1.0f, 0.0f,        0.0f,       0.0f, 0.0f, cos(theta), sin(theta), 0.0f,
            0.0f, -sin(theta), cos(theta), 0.0f, 0.0f, 0.0f,       0.0f,       1.0f};
}

std::array<GLfloat, 16> rotY(float theta) {
    return {cos(theta), 0.0f, -sin(theta), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            sin(theta), 0.0f, cos(theta),  0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

std::array<GLfloat, 16> rotZ(float theta) {
    return {cos(theta), sin(theta), 0.0f, 0.0f, -sin(theta), cos(theta), 0.0f, 0.0f,
            0.0f,       0.0f,       1.0f, 0.0f, 0.0f,        0.0f,       0.0f, 1.0f};
}

std::array<GLfloat, 16> translation(float x, float y, float z) {
    return {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, x, y, z, 1.0f};
}

std::array<GLfloat, 16> scale(float x, float y, float z) {
    return {x, 0.0f, 0.0f, 0.0f, 0.0f, y, 0.0f, 0.0f, 0.0f, 0.0f, z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
}

std::array<float, 16> matrisMultiplikation(const std::array<float, 16>& a,
                                           const std::array<float, 16>& b) {
    std::array<float, 16> result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    return result;
}

/*
 * main(int argc, char* argv[]) - the standard C++ entry point for the program
 */
int main(int, char*[]) {

    // Vertex coordinates (x,y,z) for three vertices
    // const std::vector<GLfloat> vertexArrayData = {
    //     -1.0f, -1.0f, 0.0f,  // First vertex, xyz
    //     1.0f,  -1.0f, 0.0f,  // Second vertex, xyz
    //     0.0f,  1.0f,  0.0f   // Third vertex, xyz
    // };
    // const std::vector<GLuint> indexArrayData = {0, 1, 2};

    const std::vector<GLfloat> vertexArrayData = {
        -1.0f, -1.0f, -1.0f,  // First vertex, xyz
        -1.0f, -1.0f, -1.0f,  // Second vertex, xyz
        -1.0f, -1.0f, -1.0f,  // Third vertex, xyz

        -1.0f, -1.0f, 1.0f,  // First vertex, xyz
        -1.0f, -1.0f, 1.0f,  // Second vertex, xyz
        -1.0f, -1.0f, 1.0f,  // Third vertex, xyz

        -1.0f, 1.0f,  1.0f,  // First vertex, xyz
        -1.0f, 1.0f,  1.0f,  // Second vertex, xyz
        -1.0f, 1.0f,  1.0f,  // Third vertex, xyz

        -1.0f, 1.0f,  -1.0f,  // First vertex, xyz
        -1.0f, 1.0f,  -1.0f,  // Second vertex, xyz
        -1.0f, 1.0f,  -1.0f,  // Third vertex, xyz

        1.0f,  -1.0f, -1.0f,  // First vertex, xyz
        1.0f,  -1.0f, -1.0f,  // Second vertex, xyz
        1.0f,  -1.0f, -1.0f,  // Third vertex, xyz

        1.0f,  -1.0f, 1.0f,  // First vertex, xyz
        1.0f,  -1.0f, 1.0f,  // Second vertex, xyz
        1.0f,  -1.0f, 1.0f,  // Third vertex, xyz

        1.0f,  1.0f,  1.0f,  // First vertex, xyz
        1.0f,  1.0f,  1.0f,  // Second vertex, xyz
        1.0f,  1.0f,  1.0f,  // Third vertex, xyz

        1.0f,  1.0f,  -1.0f,  // First vertex, xyz
        1.0f,  1.0f,  -1.0f,  // Second vertex, xyz
        1.0f,  1.0f,  -1.0f   // Third vertex, xyz
    };
    // const std::vector<GLuint> indexArrayData = {
    //     0, 3, 9,
    //     3, 6, 9,
    //     4, 15, 7,
    //     15, 20, 7,
    //     8, 18, 11,
    //     18, 22, 11,
    //     23, 19, 17,
    //     23, 17, 13,
    //     12, 10, 21,
    //     12, 1, 10,
    //     2, 16, 5,
    //     2, 14, 16
    // };

    const std::vector<GLuint> indexArrayData = {0,  3,  9,  3,  6,  9,  4,  17, 7,  17, 20, 7,
                                                15, 21, 18, 15, 13, 21, 12, 10, 23, 12, 1,  10,
                                                8,  22, 11, 8,  19, 22, 2,  16, 5,  2,  14, 16};
    // const std::vector<GLfloat> colorArrayData = {
    //     1.0f, 0.0f, 0.0f,  
    //     0.0f, 1.0f, 0.0f,  
    //     1.0f, 0.0f, 1.0f,  

    //     1.0f,  0.0f,  0.0f,  
    //     0.0f,  1.0f,  1.0f,  
    //     1.0f,  0.0f,  1.0f, 

    //     1.0f,  0.0f,  0.0f,  
    //     0.0f,  1.0f,  1.0f,  
    //     0.0f,  0.0f,  1.0f, 

    //     1.0f,  0.0f,  0.0f,  
    //     0.0f,  1.0f,  0.0f,  
    //     0.0f,  0.0f,  1.0f, 

    //     0.0f,  1.0f,  0.0f,  
    //     1.0f,  1.0f,  0.0f,  
    //     1.0f,  0.0f,  1.0f, 

    //     1.0f,  1.0f,  0.0f,  
    //     1.0f,  0.0f,  1.0f,  
    //     0.0f,  1.0f,  1.0f, 

    //     1.0f,  1.0f,  0.0f,  
    //     0.0f,  0.0f,  1.0f,  
    //     0.0f,  1.0f,  1.0f,  

    //     1.0f,  1.0f,  0.0f,  
    //     0.0f,  0.0f,  1.0f,  
    //     0.0f,  1.0f,  0.0f,
    // };
    const std::vector<GLfloat> colorArrayData = {
        -1.0f, -1.0f, -1.0f,  // First vertex, xyz
        -1.0f, -1.0f, -1.0f,  // Second vertex, xyz
        -1.0f, -1.0f, -1.0f,  // Third vertex, xyz

        -1.0f, -1.0f, 1.0f,  // First vertex, xyz
        -1.0f, -1.0f, 1.0f,  // Second vertex, xyz
        -1.0f, -1.0f, 1.0f,  // Third vertex, xyz

        -1.0f, 1.0f,  1.0f,  // First vertex, xyz
        -1.0f, 1.0f,  1.0f,  // Second vertex, xyz
        -1.0f, 1.0f,  1.0f,  // Third vertex, xyz

        -1.0f, 1.0f,  -1.0f,  // First vertex, xyz
        -1.0f, 1.0f,  -1.0f,  // Second vertex, xyz
        -1.0f, 1.0f,  -1.0f,  // Third vertex, xyz

        1.0f,  -1.0f, -1.0f,  // First vertex, xyz
        1.0f,  -1.0f, -1.0f,  // Second vertex, xyz
        1.0f,  -1.0f, -1.0f,  // Third vertex, xyz

        1.0f,  -1.0f, 1.0f,  // First vertex, xyz
        1.0f,  -1.0f, 1.0f,  // Second vertex, xyz
        1.0f,  -1.0f, 1.0f,  // Third vertex, xyz

        1.0f,  1.0f,  1.0f,  // First vertex, xyz
        1.0f,  1.0f,  1.0f,  // Second vertex, xyz
        1.0f,  1.0f,  1.0f,  // Third vertex, xyz

        1.0f,  1.0f,  -1.0f,  // First vertex, xyz
        1.0f,  1.0f,  -1.0f,  // Second vertex, xyz
        1.0f,  1.0f,  -1.0f   // Third vertex, xyz
    };

    std::array<GLfloat, 16> matT = {1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                                    0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};

    // Initialise GLFW
    glfwInit();

    const GLFWvidmode* vidmode;  // GLFW struct to hold information about the display
    // Determine the desktop size
    vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // Make sure we are getting a GL context of at least version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Enable the OpenGL core profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open a square window (aspect 1:1) to fill half the screen height
    GLFWwindow* window =
        glfwCreateWindow(vidmode->height / 2, vidmode->height / 2, "GLprimer", nullptr, nullptr);
    if (!window) {
        std::cout << "Unable to open window. Terminating.\n";
        glfwTerminate();  // No window was opened, so we can't continue in any useful way
        return -1;
    }

    // Make the newly created window the "current context" for OpenGL
    // (This step is strictly required or things will simply not work)
    glfwMakeContextCurrent(window);

    // Initialize glew
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cerr << "Error: " << glewGetErrorString(err) << "\n";
        glfwTerminate();
        return -1;
    }

    myShader.createShader("vertex.glsl", "fragment.glsl");

    // Generate 1 Vertex array object, put the resulting identifier in vertexArrayID
    GLuint vertexArrayID = 0;
    glGenVertexArrays(1, &vertexArrayID);
    // Activate the vertex array object
    glBindVertexArray(vertexArrayID);

    GLuint vertexBufferID = createVertexBuffer(0, 3, vertexArrayData);

    GLuint colorBufferID = createVertexBuffer(1, 3, colorArrayData);

    // Generate 1 buffer, put the resulting identifier in indexBufferID
    GLuint indexBufferID = createIndexBuffer(indexArrayData);

    glBindVertexArray(0);

    // Show some useful information on the GL context
    std::cout << "GL vendor:       " << glGetString(GL_VENDOR)
              << "\nGL renderer:     " << glGetString(GL_RENDERER)
              << "\nGL version:      " << glGetString(GL_VERSION)
              << "\nDesktop size:    " << vidmode->width << " x " << vidmode->height << "\n";

    // Get window size. It may start out different from the requested size and
    // will change if the user resizes the window
    int width, height;

    glfwSwapInterval(0);  // Do not wait for screen refresh between frames

    GLint locationTime = glGetUniformLocation(myShader.id(), "time");
    if (locationTime == -1) {  // If the variable is not found, -1 is returned
        std::cout << "Unable to locate variable 'time' in shader!\n";
    }

    // TriangleSoup myShape;
    // myShape.createTriangle();

    TriangleSoup mySoup;
    mySoup.createSphere(0.8f, 200);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Set the clear color to a dark gray (RGBA)
        glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

        util::displayFPS(window);

        glfwGetWindowSize(window, &width, &height);
        // Set viewport. This is the pixel rectangle we want to draw into
        glViewport(0, 0, width, height);  // The entire window

        // Clear the color and depth buffers for drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(myShader.id());

        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glCullFace(GL_BACK);

        /* ---- Rendering code should go here ---- */

        // Do this in the rendering loop to update the uniform variable "time"
        float time =
            static_cast<float>(glfwGetTime());  // Number of seconds since the program was started
        glUseProgram(myShader.id());            // Activate the shader to set its variables
        glUniform1f(locationTime, time);        // Copy the value to the shader program
        // Activate the vertex array object we want to draw (we may have several)
        glBindVertexArray(vertexArrayID);
        // Draw our triangle with 3 vertices.
        // When the last argument of glDrawElements is nullptr, it means
        // "use the previously bound index buffer". (This is not obvious.)
        // The index buffer is part of the VAO state and is bound with it.
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

        
        std::array<GLfloat, 16> matR1 = rotY(glfwGetTime());
        std::array<GLfloat, 16> matR2 = rotY(glfwGetTime());
        std::array<GLfloat, 16> cam = rotX(3.14/6);
        std::array<GLfloat, 16> matS = scale(0.9f, 0.9f, 0.0f);
        std::array<GLfloat, 16> matOT = translation(0.5f, 0.5f, 0.0f);

        
        std::array<GLfloat, 16> mat1 = matrisMultiplikation(matR1, cam);
        std::array<GLfloat, 16> mat2 = matrisMultiplikation(mat1, matS);
        std::array<GLfloat, 16> mat3 = matrisMultiplikation(matS, matT);
    
        

        GLint locationT = glGetUniformLocation(myShader.id(), "T");
        glUseProgram(myShader.id());  // Activate the shader to set its variables
        glUniformMatrix4fv(locationT, 1, GL_FALSE, mat1.data());  // Copy the value

        //myShape.render();
        mySoup.render();
        

        // GLint locationR = glGetUniformLocation(myShader.id(), "R");
        // glUseProgram(myShader.id());  // Activate the shader to set its variables
        // glUniformMatrix4fv(locationR, 1, GL_FALSE, matR.data());  // Copy the value

        // Swap buffers, display the image and prepare for next frame
        glfwSwapBuffers(window);

        // Poll events (read keyboard and mouse input)
        glfwPollEvents();

        // Exit if the ESC key is pressed (and also if the window is closed)
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
    glDeleteVertexArrays(1, &vertexArrayID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);

    // Close the OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();
}
