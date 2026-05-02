R""(

#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

out vec4 InColor;
out vec3 FragPos;   // world-space position
out vec3 Normal;    // transformed normal

void main()
{
    InColor = color;

    // World position
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Proper normal transform (important if model is scaled)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}

)""