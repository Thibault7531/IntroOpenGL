#version 460

in vec2 v_TexCoord;

out vec4 color;

uniform sampler2D u_Texture;
uniform float u_Time;
uniform int u_IsOrange;

void main() {
    vec2 center = vec2(0.5, 0.5);

    vec2 noiseUV = v_TexCoord * vec2(0.3, 0.3) + vec2(0.0, 0.7);
    vec2 scroll1 = noiseUV + vec2(u_Time * 0.05, u_Time * 0.02);
    vec2 scroll2 = noiseUV + vec2(u_Time * -0.03, u_Time * 0.06);

    vec2 localScroll1 = fract(v_TexCoord * 1.5 + vec2(u_Time * 0.05, u_Time * 0.02));
    vec2 localScroll2 = fract(v_TexCoord * 1.5 + vec2(u_Time * -0.03, u_Time * 0.06));

    const vec2 k_NoiseScale = vec2(256.0 / 844.0, 256.0 / 823.0);
    const vec2 k_NoiseOffset = vec2(0.0, 567.0 / 823.0);

    vec2 noiseUV1 = localScroll1 * k_NoiseScale + k_NoiseOffset;
    vec2 noiseUV2 = localScroll2 * k_NoiseScale + k_NoiseOffset;

    float noiseValue1 = texture(u_Texture, noiseUV1).r;
    float noiseValue2 = texture(u_Texture, noiseUV2).r;
    float combinedNoise = (noiseValue1 + noiseValue2) * 0.5;

    float distFromCenter = distance(v_TexCoord, center);

    const vec2 k_MaskScale = vec2(512.0 / 844.0, 512.0 / 823.0);
    const vec2 k_MaskOffset = vec2(0.0, 54.0 / 823.0);

    vec2 maskUV = v_TexCoord * k_MaskScale + k_MaskOffset;
    float portalMask = texture(u_Texture, maskUV).r;

    float edgeFactor = smoothstep(0.35, 0.5, distFromCenter);
    float dynamicShift = (combinedNoise - 0.5) * 0.12 * edgeFactor;

    float paletteU = (u_IsOrange == 1) ? 0.682 : 0.379;
    float rimGradient = clamp((distFromCenter - 0.32) / 0.16 + (combinedNoise - 0.5) * 0.3, 0.0, 1.0);
    float paletteV = mix(0.87, 0.98, rimGradient);
    vec2 colorUV =vec2(paletteU, paletteV);

    vec4 finalBorderColor = texture(u_Texture, colorUV);

    float ringMask = smoothstep(0.40, 0.48, distFromCenter) * smoothstep(0.5, 0.48, distFromCenter);
    finalBorderColor *= ringMask * clamp(combinedNoise * 2.0, 0.0, 1.0);

    color = finalBorderColor;
}
