#version 460 core

in vec4 v_Position;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Texture;
uniform vec4 u_BaseColor;

uniform vec3 u_LightPosition;

void main() {
    float ambientFactor = 0.2;
    vec3 ambientLight = vec3(ambientFactor);

    vec3 lightDirection = normalize(v_Position.xyz - u_LightPosition);
    vec3 normalizedNormal = normalize(v_Normal);
    float intensity = abs(min(dot(lightDirection, normalizedNormal), 0.0));

    vec4 texColor = texture(u_Texture, v_TexCoord);

    color = vec4(texColor.rgb * u_BaseColor.rgb * (ambientLight + intensity), u_BaseColor.a);
}