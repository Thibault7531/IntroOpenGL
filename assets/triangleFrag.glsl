#version 460 core

in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Texture;
uniform vec3 u_Color;

void main() {
    color = texture(u_Texture, v_TexCoord);
    color *= vec4(u_Color, 1.0);
}