#version 330 core

out vec4 FragColor;

in vec2 TextCoord;
 uniform sampler2D atlas;

void main()
{
   FragColor = texture(atlas, TextCoord);
}
