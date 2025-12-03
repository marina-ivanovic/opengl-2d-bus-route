#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "../Header/Util.h"
#include "../BusState.h"

// --- CONSTANTS ---
const int NUM_STATIONS = 10;
const float PI = 3.14159265359;
const double targetFPS = 75.0;
const double targetFrameTime = 1.0 / targetFPS;

// --- GLOBAL VARIABLES ---
int screenWidth = 0;
int screenHeight = 0;
State busState;

float stationPos[10][2] = {
    { -0.6f, -0.5f }, { -0.1f, -0.65f }, {  0.35f, -0.5f }, {  0.65f, -0.2f },
    {  0.45f,  0.1f }, {  0.65f,  0.45f }, {  0.2f,   0.65f }, { -0.25f,  0.6f },
    { -0.7f,   0.3f }, { -0.55f, -0.1f }
};

// --- TEXTURES ---
unsigned int busTexture, circleTexture, controlTexture, openTexture, closedTexture, nameTexture;
unsigned int stationsTexture[10];
unsigned int numbersTexture[10];
float pathControlPoints[10][2];
unsigned int backgroundTexture;

// --- MATHEMATICS ---

void initRandomPaths() {
    for (int i = 0; i < NUM_STATIONS; i++) {
        int next = (i + 1) % NUM_STATIONS;
        float p0x = stationPos[i][0]; float p0y = stationPos[i][1];
        float p2x = stationPos[next][0]; float p2y = stationPos[next][1];

        float midX = (p0x + p2x) / 2.0f;
        float midY = (p0y + p2y) / 2.0f;
        float dx = p2x - p0x;
        float dy = p2y - p0y;

        // Sine wave curvature
        float wave = sin((float)i * 2.5f);
        float curvature = 0.35f * wave;

        pathControlPoints[i][0] = midX - dy * curvature;
        pathControlPoints[i][1] = midY + dx * curvature;
    }
}

void getBezierPoint(float t, float p0x, float p0y, float p1x, float p1y, float p2x, float p2y, float& outX, float& outY) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    outX = uu * p0x + 2 * u * t * p1x + tt * p2x;
    outY = uu * p0y + 2 * u * t * p1y + tt * p2y;
}

// --- OPENGL HELPERS ---

void configureTexture(unsigned int texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void createPathVAO(unsigned int& VAO, unsigned int& VBO, int slices) {
    std::vector<float> vertices;
    for (int i = 0; i < NUM_STATIONS; i++) {
        int next = (i + 1) % NUM_STATIONS;
        float p0x = stationPos[i][0]; float p0y = stationPos[i][1];
        float p2x = stationPos[next][0]; float p2y = stationPos[next][1];
        float p1x = pathControlPoints[i][0]; float p1y = pathControlPoints[i][1];

        for (int j = 0; j <= slices; j++) {
            float t = (float)j / (float)slices;
            float x, y;
            getBezierPoint(t, p0x, p0y, p1x, p1y, p2x, p2y, x, y);
            vertices.push_back(x); vertices.push_back(y);
            vertices.push_back(0.0f); vertices.push_back(0.0f);
        }
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void drawUIElement(unsigned int texture, float x, float y, unsigned int shader, unsigned int VAO, int uPosLoc) {

    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);
    glUniform4f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform2f(uPosLoc, x, y);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawNumber(int number, float x, float y, float size, unsigned int* numTextures, unsigned int shader, unsigned int VAO, int uPosLoc) {
    int tens = (number / 10) % 10;
    int units = number % 10;

    if (number >= 10) {
        drawUIElement(numTextures[tens], x, y, shader, VAO, uPosLoc);
    }
    drawUIElement(numTextures[units], x + 0.06f, y, shader, VAO, uPosLoc);
}

void createCircle(unsigned int& VAO, unsigned int& VBO, float radius) {
    float circleVertices[(40 + 2) * 4];
    float r = radius;
    circleVertices[0] = 0.0; circleVertices[1] = 0.0;
    circleVertices[2] = 0.5; circleVertices[3] = 0.5;
    for (int i = 0; i <= 40; ++i) {
        float angle = i * 2 * PI / 40;
        int idx = (i + 1) * 4;
        circleVertices[idx] = cos(angle) * r;
        circleVertices[idx + 1] = sin(angle) * r;
        circleVertices[idx + 2] = cos(angle) * 0.5 + 0.5;
        circleVertices[idx + 3] = sin(angle) * 0.5 + 0.5;
    }
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void createQuad(unsigned int& VAO, unsigned int& VBO, float size) {
    float vertices[] = {
        -size, -size,  0.0, 0.0,
         size, -size,  1.0, 0.0,
         size,  size,  1.0, 1.0,
        -size,  size,  0.0, 1.0
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

// --- CALLBACKS ---
void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (busState.mode == WAITING && busState.doorsOpen) {
        if (action == GLFW_PRESS) {
            if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (busState.passengers < 50) busState.passengers++;
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

    // TICKET CONTROL
    if (key == GLFW_KEY_K && action == GLFW_PRESS) {
        if (busState.mode == WAITING && busState.doorsOpen && !busState.hasControl) {
            busState.hasControl = true;
            busState.passengers++; // Controller enters
        }
    }
}

// --- MAIN ---
int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    screenWidth = mode->width;
    screenHeight = mode->height;

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Bus Route Tracker", primaryMonitor, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) return -1;

    glfwSetMouseButtonCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    // Init State
    busState.mode = MOVING;
    busState.currentStation = 0;
    busState.nextStation = 1;
    busState.t = 0.0f;
    busState.waitTimer = 0.0f;
    busState.passengers = 0;
    busState.fines = 0;
    busState.hasControl = false;
    busState.doorsOpen = true;

    // Cursor
    GLFWcursor* cursor = loadImageToCursor("Resources/cursor.png");
    if (cursor) glfwSetCursor(window, cursor);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Texture Loading
    busTexture = loadImageToTexture("Resources/bus.png"); configureTexture(busTexture);
    controlTexture = loadImageToTexture("Resources/control.png"); configureTexture(controlTexture);
    openTexture = loadImageToTexture("Resources/open.png"); configureTexture(openTexture);
    closedTexture = loadImageToTexture("Resources/closed.png"); configureTexture(closedTexture);
    nameTexture = loadImageToTexture("Resources/name.png"); configureTexture(nameTexture);

    for (int i = 0; i < 10; ++i) {
        std::string path = "Resources/station" + std::to_string(i) + ".png";
        stationsTexture[i] = loadImageToTexture(path.c_str());
        configureTexture(stationsTexture[i]);
    }

    for (int i = 0; i < 10; ++i) {
        std::string path = "Resources/num" + std::to_string(i) + ".png";
        numbersTexture[i] = loadImageToTexture(path.c_str());
        configureTexture(numbersTexture[i]);
    }

    backgroundTexture = loadImageToTexture("Resources/novisad.png"); configureTexture(backgroundTexture);

    // VAO Creation
    unsigned int VAO_Background, VBO_Background;

    float bgVertices[] = {
        // X      Y      U     V
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f,
        -1.0f,  1.0f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO_Background);
    glGenBuffers(1, &VBO_Background);

    glBindVertexArray(VAO_Background);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Background);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), bgVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 2. NAME TAG
    unsigned int VAO_Name, VBO_Name;
    float w = 0.5f;
    float h = 0.2f;
    float nameVertices[] = {
        -w / 2, -h / 2, 0.0f, 0.0f,
         w / 2, -h / 2, 1.0f, 0.0f,
         w / 2,  h / 2, 1.0f, 1.0f,
        -w / 2,  h / 2, 0.0f, 1.0f
    };
    glGenVertexArrays(1, &VAO_Name);
    glGenBuffers(1, &VBO_Name);
    glBindVertexArray(VAO_Name);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Name);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nameVertices), nameVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VAO_Station, VBO_Station;
    createCircle(VAO_Station, VBO_Station, 0.06f);

    unsigned int VAO_Bus, VBO_Bus;
    createQuad(VAO_Bus, VBO_Bus, 0.05f);

    unsigned int VAO_Path, VBO_Path;
    int slicesPerCurve = 30;
    initRandomPaths();
    createPathVAO(VAO_Path, VBO_Path, slicesPerCurve);

    unsigned int VAO_UI, VBO_UI;
    createQuad(VAO_UI, VBO_UI, 0.17f);
    
    // ---------------------------------------

    // Shaders and Uniforms
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
        if (currentTime - lastTime < targetFrameTime) {
            continue;
        }
        double deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // 1. UPDATE LOGIC
        if (busState.mode == MOVING) {
            busState.doorsOpen = false;
            busState.t += deltaTime * 0.3f; // Speed

            if (busState.t >= 1.0f) {
                busState.currentStation = busState.nextStation;
                busState.nextStation = (busState.currentStation + 1) % NUM_STATIONS;
                busState.mode = WAITING;
                busState.t = 0.0f;
                busState.waitTimer = 0.0f;
                busState.doorsOpen = true;

                if (busState.hasControl) {
                    busState.hasControl = false;
                    busState.passengers--; // Control exits
                    if (busState.passengers > 0) {
                        int caught = rand() % busState.passengers;
                        busState.fines += caught;
                    }
                }
            }
        }
        else if (busState.mode == WAITING) {
            busState.waitTimer += deltaTime;
            if (busState.waitTimer >= 10.0f) {
                busState.mode = MOVING;
                busState.doorsOpen = false;
            }
        }

        // 2. RENDER
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader);

        // A) BACKGROUND
        glUniform1i(uUseTexLoc, 1);
        glBindVertexArray(VAO_Background);
        glBindTexture(GL_TEXTURE_2D, backgroundTexture);

        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 1.0); // White color = Original image
        glUniform2f(uPosLoc, 0.0, 0.0);
        glUniform1f(uAspectLoc, 1.0f); // Stretch across full screen (ignore aspect ratio)

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // -- Restore correct aspect ratio for the rest of the scene --
        glUniform1f(uAspectLoc, aspectRatio);

        // B) PATH
        glUniform1i(uUseTexLoc, 0); // Disable texture
        glBindVertexArray(VAO_Path);
        glUniform4f(uColorLoc, 1.0, 0.0, 0.0, 1.0); // Red
        glUniform2f(uPosLoc, 0.0, 0.0);
        glDrawArrays(GL_LINE_STRIP, 0, NUM_STATIONS * (slicesPerCurve + 1));

        // C) STATIONS
        glUniform1i(uUseTexLoc, 1); // Enable texture
        glBindVertexArray(VAO_Station);
        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 1.0);

        for (int i = 0; i < NUM_STATIONS; i++) {
            glBindTexture(GL_TEXTURE_2D, stationsTexture[i]);
            glUniform2f(uPosLoc, stationPos[i][0], stationPos[i][1]);
            glDrawArrays(GL_TRIANGLE_FAN, 0, 42);
        }

        // D) BUS
        glBindVertexArray(VAO_Bus);
        glBindTexture(GL_TEXTURE_2D, busTexture);

        float busX, busY;
        if (busState.mode == WAITING) {
            busX = stationPos[busState.currentStation][0];
            busY = stationPos[busState.currentStation][1];
        }
        else {
            float p0x = stationPos[busState.currentStation][0];
            float p0y = stationPos[busState.currentStation][1];
            float p2x = stationPos[busState.nextStation][0];
            float p2y = stationPos[busState.nextStation][1];
            float p1x = pathControlPoints[busState.currentStation][0];
            float p1y = pathControlPoints[busState.currentStation][1];
            getBezierPoint(busState.t, p0x, p0y, p1x, p1y, p2x, p2y, busX, busY);
        }

        glUniform2f(uPosLoc, busX, busY);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        // E) UI ELEMENTS (Corner Icons)

        float marginX = 0.17f;
        float marginY = 0.17f;

        // 1. DOORS (Bottom-Left)
        unsigned int doorTex = busState.doorsOpen ? openTexture : closedTexture;
        drawUIElement(doorTex, (-1.0f * aspectRatio) + marginX, -1.0f + marginY, shader, VAO_UI, uPosLoc);

        // 2. CONTROL (Top-Right)
        if (busState.hasControl) {
            drawUIElement(controlTexture, (1.0f * aspectRatio) - marginX, 1.0f - marginY, shader, VAO_UI, uPosLoc);
        }

        // 3. PASSENGERS & FINES (Top-Left)

        // Passengers (Top Left)
        drawNumber(busState.passengers, (-1.0f * aspectRatio) + marginX, 1.0f - marginY, 0.0f, numbersTexture, shader, VAO_Bus, uPosLoc);

        // Fines (Below Passengers)
        drawNumber(busState.fines, (-1.0f * aspectRatio) + marginX, 1.0f - marginY - 0.15f, 0.0f, numbersTexture, shader, VAO_Bus, uPosLoc);

        // 4. NAME TAG (Bottom-Right)

        glUniform1i(uUseTexLoc, 1);
        glBindVertexArray(VAO_Name);
        glBindTexture(GL_TEXTURE_2D, nameTexture);

        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 0.7f);

        glUniform2f(uPosLoc, (1.0f * aspectRatio) - 0.22f , -0.9f);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glUniform4f(uColorLoc, 1.0, 1.0, 1.0, 1.0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}