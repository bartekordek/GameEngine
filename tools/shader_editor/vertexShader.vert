#version 330 core

layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
uniform vec3 off;

out vec3 ourColor; // output a color to the fragment shader
out vec4 vertexPosition;

void main()
{
    gl_Position = vec4(aPos.x + off.x, aPos.y + off.y, aPos.z + off.z, 1.0);
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
    vertexPosition = gl_Position;
}