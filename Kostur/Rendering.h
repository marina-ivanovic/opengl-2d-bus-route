#pragma once
#include <GL/glew.h>

void configureTexture(unsigned int texture);
void drawUIElement(unsigned int texture, float x, float y, unsigned int shader, unsigned int VAO, int uPosLoc, bool withBackground = false);
void drawNumber(int number, float x, float y, float size, unsigned int* numTextures, unsigned int shader, unsigned int VAO, int uPosLoc);