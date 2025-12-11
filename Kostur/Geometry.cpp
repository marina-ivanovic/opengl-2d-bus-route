#include "../Kostur/Geometry.h"

extern float stationPos[10][2];
extern float pathControlPoints[10][2];

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
    createRect(VAO, VBO, size, size);
}

void createRect(unsigned int& VAO, unsigned int& VBO, float w, float h) {
    float vertices[] = {
        -w / 2, -h / 2, 0.0f, 0.0f,
         w / 2, -h / 2, 1.0f, 0.0f,
         w / 2,  h / 2, 1.0f, 1.0f,
        -w / 2,  h / 2, 0.0f, 1.0f
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