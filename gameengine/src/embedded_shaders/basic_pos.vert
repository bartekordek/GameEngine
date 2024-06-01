R""(

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

out vec4 InColor;

void main()
{
    InColor = color;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}

)""