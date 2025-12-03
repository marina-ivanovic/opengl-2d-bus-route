#version 330 core
layout(location = 0) in vec2 inPos;
layout(location = 1) in vec2 inTex;
out vec2 chTex;
uniform vec2 uPos;
uniform float uAspectRatio;

void main()
{
    float newX = (inPos.x + uPos.x) / uAspectRatio;
    float newY = inPos.y + uPos.y;
    gl_Position = vec4(newX, newY, 0.0, 1.0);
    chTex = inTex;
}