R""(

#version 330 core

in vec4 InColor;
in vec4 InLight;

out vec4 FragColor;
vec3 OutColor;

void main()
{
    OutColor = InColor.xyz * InLight.xyz;
    FragColor = vec4(OutColor.x, OutColor.y, OutColor.z, 1.0);
}

)""