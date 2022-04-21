#version 120

attribute vec3 inColor;
attribute vec3 inPosition;
uniform mat4 matrix;

varying vec3 outColor;

void main()
{
    outColor = inColor;
    gl_Position = vec4( inPosition, 1 ) * matrix;
}