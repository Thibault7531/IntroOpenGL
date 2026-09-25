#version 460 core

in vec3 v_Color;
in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Texture;

void main() {
    color = texture(u_Texture, v_TexCoord);
    color *= vec4(v_Color, 1.0);
}