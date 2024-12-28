#version 330 core

out vec4 FragColor;
in vec3 ourColor;
in vec4 vertexPosition;

void main()
{
    // float red = (sin(vertexPosition.x) + 1.f ) / 2.f;
    // float green = (sin(vertexPosition.x) + 1.f ) / 2.f;
    // float blue = (sin(vertexPosition.x) + 1.f ) / 2.f;

    FragColor = vec4( ourColor.xyz, 1.0 );
}