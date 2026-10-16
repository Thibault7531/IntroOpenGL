#version 460 core

struct Material
{
    vec3 ambient;
    sampler2D diffuseMap;
    int emissiveChannels;
    sampler2D emissiveMap;
    vec3 specular;
    float shininess;
};

struct PointLight
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec4 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;
in vec2 v_EmissiveCoord;

out vec4 color;

uniform Material u_Material;
uniform int u_PointLightCount;
uniform PointLight u_PointLights[16];
uniform DirectionalLight u_DirLight;

uniform vec4 u_BaseColor;
uniform vec3 u_CameraPosition;

vec3 calculatePointLightIntensity(PointLight light)
{
    // Attenuation
    vec3 distanceVector = abs(light.position - v_Position.xyz);
    float distance = sqrt(dot(distanceVector, distanceVector));
    float attenuation = 1/(light.constant + light.linear * distance + light.quadratic * distance * distance);

    // Ambient
    vec3 ambientLight = light.ambient * u_Material.ambient * texture(u_Material.diffuseMap, v_TexCoord).xyz;

    // Diffuse
    vec3 lightDirection = normalize(light.position - v_Position.xyz);
    vec3 normalizedNormal = normalize(v_Normal);
    float intensity = max(dot(lightDirection, normalizedNormal), 0.0);
    vec3 diffusedLight = light.diffuse * texture(u_Material.diffuseMap, v_TexCoord).xyz * intensity;

    // Specular
    vec3 viewDirection = normalize(u_CameraPosition - v_Position.xyz);
    vec3 halfDirection = normalize(lightDirection + viewDirection);
    float HdotN = max(dot(halfDirection, normalizedNormal), 0.0);
    vec3 specular = light.specular * u_Material.specular * pow(HdotN, u_Material.shininess);

    return ambientLight + (diffusedLight + specular) * attenuation;
}

vec3 calculateDirectionalLightIntensity(DirectionalLight light) {
    // Ambient
    vec3 ambientLight = light.ambient * u_Material.ambient * texture(u_Material.diffuseMap, v_TexCoord).xyz;

    // Diffuse
    vec3 lightDirection = normalize(-light.direction);
    vec3 normalizedNormal = normalize(v_Normal);
    float intensity = max(dot(lightDirection, normalizedNormal), 0.0);
    vec3 diffusedLight = light.diffuse * texture(u_Material.diffuseMap, v_TexCoord).xyz * intensity;

    // Specular
    vec3 viewDirection = normalize(u_CameraPosition - v_Position.xyz);
    vec3 halfDirection = normalize(lightDirection + viewDirection);
    float HdotN = max(dot(halfDirection, normalizedNormal), 0.0);
    vec3 specular = light.specular * u_Material.specular * pow(HdotN, u_Material.shininess);

    return ambientLight + diffusedLight + specular;
}

void main() {
    vec3 calculatedLight = vec3(0.0);
    for (int i = 0; i < u_PointLightCount; i++) {
        calculatedLight += calculatePointLightIntensity(u_PointLights[i]);
    }
    calculatedLight += calculateDirectionalLightIntensity(u_DirLight);

    // Emission
    vec3 emission;
    if (u_Material.emissiveChannels == 1) {
        emission = vec3(texture(u_Material.emissiveMap, v_EmissiveCoord).x);
    } else if (u_Material.emissiveChannels == 2) {
        vec2 emissionData = texture(u_Material.emissiveMap, v_EmissiveCoord).xy;
        emission = vec3(emissionData.x, emissionData.y, 0.0);
    } else {
        emission = texture(u_Material.emissiveMap, v_EmissiveCoord).xyz;
    }

    float alpha = min(texture(u_Material.diffuseMap, v_TexCoord).a, u_BaseColor.a);
    vec3 lighting = max(calculatedLight, emission);

    color = vec4(lighting, alpha);
}