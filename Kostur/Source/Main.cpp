#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

#include "../Header/Util.h"
#include "../BusState.h"
#include "../Constants.h"
#include "../MathUtils.h"
#include "../Geometry.h"
#include "../Rendering.h"

int screenWidth = 0;
int screenHeight = 0;
State busState;

float stationPos[10][2] = {
    { -0.6f, -0.5f }, { -0.1f, -0.65f }, {  0.35f, -0.5f }, {  0.65f, -0.2f },
    {  0.45f,  0.1f }, {  0.65f,  0.45f }, {  0.2f,   0.65f }, { -0.25f,  0.6f },
    { -0.7f,   0.3f }, { -0.55f, -0.1f }
};
float pathControlPoints[10][2];

// Textures
unsigned int busTexture, circleTexture, controlTexture, openTexture, closedTexture, nameTexture;
unsigned int stationsTexture[10];
unsigned int numbersTexture[10];
unsigned int backgroundTexture;

// Callbacks
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (busState.mode == WAITING && busState.doorsOpen) {
        if (action == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if ((busState.passengers < 50 && !busState.hasControl) || (busState.passengers < 49 && busState.hasControl)) busState.passengers++;
            }
            if (button == GLFW_MOUSE_BUTTON_RIGHT) {
                if ((busState.passengers > 0 && !busState.hasControl) || (busState.passengers > 1 && busState.hasControl)) busState.passengers--;
            }
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        if (busState.mode == WAITING && busState.doorsOpen && !busState.hasControl && busState.passengers < 50) {
            busState.hasControl = true;
            busState.passengers++;
        }
    }
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    screenWidth = mode->width;
    screenHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Bus Projekat", primaryMonitor, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) return -1;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    // Init State
    busState.mode = MOVING; busState.currentStation = 0; busState.nextStation = 1;
    busState.t = 0.0f; busState.waitTimer = 0.0f; busState.passengers = 0;
    busState.fines = 0; busState.hasControl = false; busState.doorsOpen = true;

    GLFWcursor* cursor = loadImageToCursor("Resources/cursor.png");
    if (cursor) glfwSetCursor(window, cursor);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    busTexture = loadImageToTexture("Resources/bus.png"); configureTexture(busTexture);
    controlTexture = loadImageToTexture("Resources/control.png"); configureTexture(controlTexture);
    openTexture = loadImageToTexture("Resources/open.png"); configureTexture(openTexture);
    closedTexture = loadImageToTexture("Resources/closed.png"); configureTexture(closedTexture);
    nameTexture = loadImageToTexture("Resources/name.png"); configureTexture(nameTexture);
    backgroundTexture = loadImageToTexture("Resources/novisad.png"); configureTexture(backgroundTexture);

    for (int i = 0; i < 10; ++i) {
        std::string path = "Resources/station" + std::to_string(i) + ".png";
        stationsTexture[i] = loadImageToTexture(path.c_str()); configureTexture(stationsTexture[i]);

        path = "Resources/num" + std::to_string(i) + ".png";
        numbersTexture[i] = loadImageToTexture(path.c_str()); configureTexture(numbersTexture[i]);
    }

    unsigned int VAO_Background, VBO_Background;
    createQuad(VAO_Background, VBO_Background, 2.0f);

    unsigned int VAO_Name, VBO_Name;
    createRect(VAO_Name, VBO_Name, 0.5f, 0.2f);

    unsigned int VAO_Station, VBO_Station;
    createCircle(VAO_Station, VBO_Station, 0.06f);

    unsigned int VAO_Bus, VBO_Bus;
    createQuad(VAO_Bus, VBO_Bus, 0.1f);

    unsigned int VAO_UI, VBO_UI;
    createQuad(VAO_UI, VBO_UI, 0.17f);

    unsigned int VAO_Path, VBO_Path;
    initRandomPaths();
    createPathVAO(VAO_Path, VBO_Path, 30);

    unsigned int shader = createShader("bus.vert", "bus.frag");
    int uPosLoc = glGetUniformLocation(shader, "uPos");
    int uTexLoc = glGetUniformLocation(shader, "uTex");
    int uColorLoc = glGetUniformLocation(shader, "uColor");
    int uAspectLoc = glGetUniformLocation(shader, "uAspectRatio");
    int uUseTexLoc = glGetUniformLocation(shader, "uUseTexture");
    float aspectRatio = (float)screenWidth / (float)screenHeight;

    glLineWidth(5.0f);
    double lastTime = glfwGetTime();

    // --- MAIN LOOP ---
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        if (currentTime - lastTime < targetFrameTime) continue;
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 1. UPDATE
        if (busState.mode == MOVING) {
            busState.doorsOpen = false;
            busState.t += deltaTime * 0.3f;
            if (busState.t >= 1.0f) {
                busState.currentStation = busState.nextStation;
                busState.nextStation = (busState.currentStation + 1) % NUM_STATIONS;
                busState.mode = WAITING; busState.t = 0.0f; busState.waitTimer = 0.0f; busState.doorsOpen = true;
                if (busState.hasControl) {
                    busState.hasControl = false; busState.passengers--;
                    if (busState.passengers > 0) {
                        busState.fines += rand() % busState.passengers;
                    }
                }
            }
        }
        else if (busState.mode == WAITING) {
            busState.waitTimer += deltaTime;
            if (busState.waitTimer >= 10.0f) { busState.mode = MOVING; busState.doorsOpen = false; }
        }

        // 2. RENDER
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader);

        // A) Background
        glUniform1i(uUseTexLoc, 1);
        glBindVertexArray(VAO_Background);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);
        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 1.0);
        glUniform2f(uPosLoc, 0.0, 0.0);
        glUniform1f(uAspectLoc, 1.0f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUniform1f(uAspectLoc, aspectRatio); // Restore Aspect

        // B) Path
        glUniform1i(uUseTexLoc, 0);
        glBindVertexArray(VAO_Path);
        glUniform4f(uColorLoc, 1.0, 0.0, 0.0, 1.0);
        glUniform2f(uPosLoc, 0.0, 0.0);
        glDrawArrays(GL_LINE_STRIP, 0, NUM_STATIONS * (30 + 1));

        // C) Stations
        glUniform1i(uUseTexLoc, 1);
        glBindVertexArray(VAO_Station);
        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 1.0);
        for (int i = 0; i < NUM_STATIONS; i++) {
            glBindTexture(GL_TEXTURE_2D, stationsTexture[i]);
            glUniform2f(uPosLoc, stationPos[i][0], stationPos[i][1]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 42);
        }

        // D) Bus
        glBindVertexArray(VAO_Bus);
        glBindTexture(GL_TEXTURE_2D, busTexture);
        float busX, busY;
        if (busState.mode == WAITING) {
            busX = stationPos[busState.currentStation][0]; busY = stationPos[busState.currentStation][1];
        }
        else {
            getBezierPoint(busState.t, stationPos[busState.currentStation][0], stationPos[busState.currentStation][1],
                pathControlPoints[busState.currentStation][0], pathControlPoints[busState.currentStation][1],
                stationPos[busState.nextStation][0], stationPos[busState.nextStation][1], busX, busY);
        }
        glUniform2f(uPosLoc, busX, busY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // E) UI
        float m = 0.17f;
        unsigned int doorTex = busState.doorsOpen ? openTexture : closedTexture;
        drawUIElement(doorTex, (-1.0f * aspectRatio) + m, -1.0f + m, shader, VAO_UI, uPosLoc, true); // Doors

        if (busState.hasControl)
            drawUIElement(controlTexture, (1.0f * aspectRatio) - m, 1.0f - m, shader, VAO_UI, uPosLoc, true); // Control

        drawNumber(busState.passengers, (-1.0f * aspectRatio) + m, 1.0f - m, 0.0f, numbersTexture, shader, VAO_Bus, uPosLoc); // Pass
        drawNumber(busState.fines, (-1.0f * aspectRatio) + m, 1.0f - m - 0.15f, 0.0f, numbersTexture, shader, VAO_Bus, uPosLoc); // Fines

        // F) Name Tag
        glUniform1i(uUseTexLoc, 1);
        glBindVertexArray(VAO_Name);
        glBindTexture(GL_TEXTURE_2D, nameTexture);
        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 0.7f);
        glUniform2f(uPosLoc, (1.0f * aspectRatio) - 0.25f, -1.0f + 0.1f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 1.0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}