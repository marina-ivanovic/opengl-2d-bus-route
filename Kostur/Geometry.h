#pragma once
#include <GL/glew.h>
#include <vector>
#include "Constants.h"
#include "MathUtils.h"

void createPathVAO(unsigned int& VAO, unsigned int& VBO, int slices);
void createCircle(unsigned int& VAO, unsigned int& VBO, float radius);
void createQuad(unsigned int& VAO, unsigned int& VBO, float size);
void createRect(unsigned int& VAO, unsigned int& VBO, float w, float h);