R""(

#version 330 core

in vec4 InColor;
in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform vec3 eyePos;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);

    // Direction from fragment → light
    vec3 lightDir = normalize(lightPos - FragPos);

    // --- Diffuse ---
    float diff = max(dot(norm, lightDir), 0.0);

    // --- Specular ---
    float specularStrength = 0.5;
    vec3 viewDir = normalize(eyePos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specular = specularStrength * spec;

    // --- Ambient ---
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * InColor.rgb;

    // Final color
    vec3 result = (ambient + diff * InColor.rgb + specular * vec3(1.0));

    FragColor = vec4(result, InColor.a);
}

)""