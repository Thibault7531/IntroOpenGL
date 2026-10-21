#version 460

out vec4 color;

uniform sampler2D u_AccumTexture;
uniform sampler2D u_RevealTexture;

const float EPSILON = 0.00001;

bool isAproximatelyEqual(float a, float b) {
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float maxComponent(vec3 v) {
    return max(max(v.x, v.y), v.z);
}

void main() {
    ivec2 coords = ivec2(gl_FragCoord.xy);
    float reveal = texelFetch(u_RevealTexture, coords, 0).r;

    if (reveal >= 0.99999) {
        discard;
    }

    vec4 accum = texelFetch(u_AccumTexture, coords, 0);

    if (any(isnan(accum)) || any(isinf(accum))) {
        accum = vec4(0.0);
    }

    if (isinf(maxComponent(accum.rgb))) {
        accum.rgb = vec3(accum.a);
    }

    vec3 averageColor = accum.rgb / max(accum.a, 0.00001);
    averageColor = clamp(averageColor, vec3(0.0), vec3(65504.0));

    color = vec4(averageColor, 1.0 - reveal);
}
