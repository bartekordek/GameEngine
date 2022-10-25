R""(

#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;
uniform vec4 light;

out vec4 InColor;
out vec4 InLight;

void main()
{
    InColor = color;
    InLight = light;
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}

)""