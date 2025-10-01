#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;      // позиция источника света
uniform vec3 viewPos;       // позиция камеры
uniform vec3 objectColor;   // цвет объекта
uniform int isTriad;        // 1 = триада, 0 = модель

void main()
{
    if (isTriad == 1) {
        // Для триады просто используем цвет без освещения
        FragColor = vec4(objectColor, 1.0);
    } else {
        // Ambient — базовое освещение
        float ambientStrength = 0.2;
        vec3 ambient = ambientStrength * objectColor;

        // Diffuse — освещение от направления света
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * objectColor;

        // Specular — бликующий компонент
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // shininess = 32
        vec3 specular = specularStrength * spec * vec3(1.0); // белый блик

        vec3 result = ambient + diffuse + specular;
        FragColor = vec4(result, 1.0);
    }
}
