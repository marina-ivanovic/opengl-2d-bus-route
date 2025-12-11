#include "../Kostur/Rendering.h"

void configureTexture(unsigned int texture) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void drawUIElement(unsigned int texture, float x, float y, unsigned int shader, unsigned int VAO, int uPosLoc, bool withBackground) {
    if (withBackground) {
        glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 0);
        glUniform4f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f, 0.7f);
        glBindVertexArray(VAO);
        glUniform2f(uPosLoc, x, y);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

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
        drawUIElement(numTextures[tens], x, y, shader, VAO, uPosLoc, false);
    }
    drawUIElement(numTextures[units], x + 0.06f, y, shader, VAO, uPosLoc, false);
}