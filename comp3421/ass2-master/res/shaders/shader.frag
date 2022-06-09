#version 330 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;
out vec4 fFragColor;

uniform sampler2D uDiffuseMap;
uniform float uDiffuseMapFactor;

uniform sampler2D uSpecularMap;
uniform float uSpecularMapFactor;

struct Material {
    vec3 ambient;
    vec4 diffuse;
    vec3 specular;
    float phongExp;
};

struct DirLight {
    vec3 direction;
    vec3 diffuse;
    vec3 ambient;
};

struct SpotLight {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

uniform Material uMat;
uniform DirLight uSun;
// the mutiple spot light
uniform SpotLight uSpot[NR_POINT_LIGHTS];
uniform vec3 uCameraPos;

vec3 sRGB_to_linear(vec3 col) {
    return pow(col, vec3(2.2));
}

vec3 linear_to_sRGB(vec3 col) {
    return pow(col, vec3(1/2.2));
}

vec3 calc_dir_light(DirLight light, vec3 mat_diffuse) {
    vec3 ambient = light.ambient * uMat.ambient;
    vec3 diffuse = light.diffuse * mat_diffuse * max(0,dot(-light.direction, vNormal));
    return ambient + diffuse;
}

vec3 calc_spot_light(SpotLight light, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 ambient = light.ambient * mat_diffuse * uMat.ambient;
    vec3 light_direction = normalize(vPosition - light.position);
    vec3 diffuse = light.diffuse * mat_diffuse * max(0,dot(-light_direction, vNormal));
    vec3 reflected = reflect(light_direction, vNormal);
    vec3 view = normalize(uCameraPos - vPosition);
    //vec3 H = normalize(-light_direction + view);
    vec3 specular = light.specular * mat_specular * pow(max(0, dot(reflected, view)), uMat.phongExp);
    //vec3 specular = light.specular * uMat.specular * pow(max(0, dot(H, vNormal)), uMat.phongExp*3);

    float distance = distance(light.position, vPosition);
    float attenuation = 1.0f / (1.0f + (0.2 * distance * distance));

    return (ambient + diffuse + specular) * attenuation;
}

void main() {
    vec4 mat_diffuse = mix(uMat.diffuse, texture(uDiffuseMap, vTexCoord), uDiffuseMapFactor);
    mat_diffuse.rgb = sRGB_to_linear(mat_diffuse.rgb);

    vec3 mat_specular = mix(uMat.specular, texture(uSpecularMap, vTexCoord).rgb, uSpecularMapFactor);
    mat_specular = sRGB_to_linear(mat_specular);

    DirLight sun = uSun;
    sun.ambient = sRGB_to_linear(sun.ambient);
    sun.diffuse = sRGB_to_linear(sun.diffuse);

    vec3 shade = calc_dir_light(sun, mat_diffuse.rgb);

    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        SpotLight spot = uSpot[i];
        spot.ambient = sRGB_to_linear(spot.ambient);
        spot.diffuse = sRGB_to_linear(spot.diffuse);

        shade += calc_spot_light(spot, mat_diffuse.rgb, mat_specular);
    }
    shade = linear_to_sRGB(shade);
    fFragColor = vec4(shade, mat_diffuse.a);
}
