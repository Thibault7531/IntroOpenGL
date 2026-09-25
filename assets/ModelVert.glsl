#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

out vec4 v_Position;
out vec3 v_Normal;
out vec2 v_TexCoord;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjection;

void main() {
    v_Position = u_Transform * vec4(a_Position, 1.0);
    gl_Position = u_ViewProjection * v_Position;
    v_Normal = (u_Transform * vec4(a_Normal, 1.0)).xyz;
    v_TexCoord = a_TexCoord;
}