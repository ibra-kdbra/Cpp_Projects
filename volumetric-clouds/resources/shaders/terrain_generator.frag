#version 430 core

#define HALF_PI 1.57079632679

layout(binding = 0) uniform sampler1D sunGradient;

in vec2 vert;
in vec4 sample_norm;
in vec3 lightDir;
in vec2 uv;

uniform sampler2D color_sampler;

// light uniforms
struct LightData {
    int type;
    vec4 pos;
    vec3 dir;  // towards light source
    vec3 color;
    float longitude;
    float latitude;
};
uniform LightData testLight;
uniform vec3 ambientColor = vec3(1.f);
uniform float ka = 0.3;
uniform float kd = 0.7;


out vec4 fragColor;


vec3 dirSph2Cart(float latitudeRadians, float longitudeRadians) {
    float x, y, z;
    x = sin(longitudeRadians) * sin(latitudeRadians);
    y = cos(longitudeRadians);
    z = sin(longitudeRadians) * cos(latitudeRadians);
    return vec3(x, y, z);
}

// query sun color texture based on height of the sun
vec3 getSunColor(float longitudeRadians) {
    float timeOfDay = abs(longitudeRadians) / HALF_PI;  // 0: noon, 1: dusk/dawn
    return texture(sunGradient, timeOfDay).rgb;
}

void main(void)
{
//    // sample color
    vec3 sampleColor = texture(color_sampler, uv).rgb;
    vec3 normal = normalize(sample_norm.xyz);

//    // Phong shading
    float sunLatitudeRadians = radians(testLight.latitude);
    float sunLongitudeRadians = radians(testLight.longitude);
    vec3 lightDir = dirSph2Cart(sunLatitudeRadians, sunLongitudeRadians);  // towards the light
    vec3 lightColor = getSunColor(sunLongitudeRadians);


//    ---------------------------------------------

    fragColor = vec4((clamp(dot(normal, lightDir), 0, 1)*0.7 + 0.3)* sampleColor, 1.0);
}
