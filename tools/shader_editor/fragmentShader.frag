#version 330 core

in vec4 InColor;

out vec4 FragColor;

void main()
{
    FragColor = vec4(InColor.x, InColor.y, InColor.z, InColor.w);
}
