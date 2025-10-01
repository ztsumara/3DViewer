#version 330 core
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPosKey;    // основной источник света
uniform vec3 lightPosFill;   // второстепенный источник света
uniform vec3 viewPos;        // позиция камеры
uniform vec3 objectColor;    // цвет модели
uniform int isTriad;         // 1 = триада, 0 = модель

void main()
{
    if(isTriad == 1) {
        FragColor = vec4(objectColor, 1.0);
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // --- Ambient ---
    vec3 ambient = 0.15 * objectColor;

    // --- Key Light (основной свет) ---
    vec3 lightDirKey = normalize(lightPosKey - FragPos);
    float diffKey = max(dot(norm, lightDirKey), 0.0);
    vec3 diffuseKey = diffKey * objectColor;
    vec3 reflectDirKey = reflect(-lightDirKey, norm);
    float specKey = pow(max(dot(viewDir, reflectDirKey), 0.0), 32.0);
    vec3 specularKey = 0.6 * specKey * vec3(1.0);

    // --- Fill Light (второстепенный свет) ---
    vec3 lightDirFill = normalize(lightPosFill - FragPos);
    float diffFill = max(dot(norm, lightDirFill), 0.0);
    vec3 diffuseFill = 0.5 * diffFill * objectColor; // немного слабее
    vec3 reflectDirFill = reflect(-lightDirFill, norm);
    float specFill = pow(max(dot(viewDir, reflectDirFill), 0.0), 16.0);
    vec3 specularFill = 0.3 * specFill * vec3(1.0);

    // --- Итоговый цвет ---
    vec3 result = ambient + diffuseKey + specularKey + diffuseFill + specularFill;
    FragColor = vec4(result, 1.0);
}
