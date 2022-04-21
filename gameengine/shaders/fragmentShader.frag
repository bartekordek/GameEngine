#version 300 es

precision highp float;
in vec4 vCol;
out vec4 color;

void main()
{
    color = vCol;
}