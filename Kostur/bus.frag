#version 330 core
in vec2 chTex;
out vec4 outCol;
uniform sampler2D uTex;
uniform vec4 uColor;
uniform bool uUseTexture;

void main()
{
    if (uUseTexture)
    {
        vec4 texColor = texture(uTex, chTex);
        if(texColor.a < 0.1) discard; 
        outCol = texColor * uColor;
    }
    else
    {
        outCol = uColor;
    }
}