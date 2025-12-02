#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "../Header/Util.h" 
int screenWidth = 0;
int screenHeight = 0;

GLFWcursor* cursor = NULL;

int main()
{
    if (!glfwInit()) {
        std::cout << "Error initializing GLFW-a!" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);

    screenWidth = mode->width;
    screenHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Bus Simulator", primaryMonitor, NULL);

    if (window == NULL) {
        std::cout << "Error creating a window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW could not be initialized." << std::endl;
        return -1;
    }

    cursor = loadImageToCursor("Resources/cursor.png");
    if (cursor != NULL) {
        glfwSetCursor(window, cursor);
    }
    else {
        std::cout << "Cursor could not be loaded" << std::endl;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.2f, 0.8f, 0.6f, 1.0f);

    // --- FPS LIMITER ---
    const double targetFPS = 75.0;
    const double targetFrameTime = 1.0 / targetFPS;
    double lastTime = glfwGetTime();

    // --- MAIN LOOP ---
    while (!glfwWindowShouldClose(window))
    {
        // FRAMERATE
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastTime;

        if (deltaTime < targetFrameTime) {
            continue;
        }
        lastTime = currentTime;

        // EXIT ON ESC
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // OVDE CE ICI TVOJ KOD ZA CRTANJE (Stanice, Bus, Overlay...)

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (cursor != NULL) glfwDestroyCursor(cursor);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}