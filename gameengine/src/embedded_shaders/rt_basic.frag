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

    // Direction from fragment -> light
    vec3 lightDir = normalize(lightPos - FragPos);

    // --- Diffuse ---
    float normToLightDir = dot(norm, lightDir);
    float diff = max(normToLightDir, 0.0); // Max - so if direction is -1 that means light is behind object.

    // --- Specular ---
    float specularStrength = 0.5;
    vec3 viewDir = normalize(eyePos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specular = specularStrength * spec;

    // --- Ambient ---
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * InColor.rgb;

    // Common attenuation formula
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float distance = length(lightPos - FragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * distance * distance);

    // Final color
    vec3 lighting = ambient + diff * InColor.rgb + specular * vec3(1.0);
    vec3 result = lighting * attenuation;

    FragColor = vec4(result, InColor.a);
}

)""