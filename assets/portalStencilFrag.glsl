#version 460

in vec2 v_TexCoord;

out vec4 color;

uniform vec3 u_Color;
uniform sampler2D u_Texture;

void main() {
    const vec2 k_MaskScale = vec2(512.0 / 844.0, 512.0 / 823.0);
    const vec2 k_MaskOffset = vec2(0.0, 54.0 / 823.0);

    vec2 maskUV = v_TexCoord * k_MaskScale + k_MaskOffset;
    float portalMask = texture(u_Texture, maskUV).r;

    if (portalMask.r < 0.1) discard;

    color = vec4(u_Color, 1.0);
}
