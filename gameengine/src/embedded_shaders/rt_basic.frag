R""(

#version 330 core

in vec4 InColor;
in vec3 InNormal;

out vec4 FragColor;

void main()
{
    vec3 normal = normalize(InNormal);
    vec3 normalColor = normal * 0.5 + 0.5;
    FragColor = vec4(normalColor, 1.0) * InColor;
}

)""