#version 330 core

in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vPosition;
in vec3 vView;
// the texture direction for the cube map
in vec3 vTexDir;
noperspective in vec2 vPortalTexCoord;

layout (location = 0) out vec4 fFragColor;

uniform sampler2D uDiffuseMap;
uniform float uDiffuseMapFactor;

uniform sampler2D uSpecularMap;
uniform float uSpecularMapFactor;

uniform samplerCube uCubeMap;
uniform float uCubeMapFactor;

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
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 diffuse;
    vec3 ambient;
    vec3 specular;
};

uniform Material uMat;
uniform DirLight uSun;
uniform SpotLight uSpot;
uniform vec3 uCameraPos;
uniform bool uIsPortal;

vec3 fNormal;
vec3 fView;

vec3 sRGB_to_linear(vec3 col) {
    return pow(col, vec3(2.2));
}

vec3 linear_to_sRGB(vec3 col) {
    return pow(col, vec3(1/2.2));
}

vec3 calc_dir_light(DirLight light, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 ambient = light.ambient * mat_diffuse * uMat.ambient;
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light.direction, fNormal));
    vec3 reflected = reflect(light.direction, vNormal);
    vec3 specular = light.specular * mat_specular * pow(max(0, dot(reflected, fView)), uMat.phongExp);
    return ambient + diffuse + specular;
}

vec3 calc_spot_light(SpotLight light, vec3 mat_diffuse, vec3 mat_specular) {
    vec3 ambient = light.ambient * mat_diffuse * uMat.ambient;
    vec3 light_direction = normalize(vPosition - light.position);
    vec3 diffuse = light.diffuse * mat_diffuse * max(0, dot(-light_direction, fNormal));
    vec3 reflected = reflect(light_direction, fNormal);
    // blinn phong lighting
    vec3 view = normalize(uCameraPos - vPosition);
    vec3 halfwayDir = normalize(light_direction + view);
    vec3 specular = light.specular * mat_specular * pow(max(0, dot(vNormal, halfwayDir)), uMat.phongExp);
    float distance = distance(light.position, vPosition);
    return (ambient + diffuse + specular);
}

void main() {
    fNormal = normalize(vNormal);
    fView = normalize(vView);
    vec2 texCoord = vTexCoord;
    if (uIsPortal) {
        texCoord = vPortalTexCoord;
    }

    vec4 mat_diffuse = mix(uMat.diffuse, texture(uDiffuseMap, texCoord), uDiffuseMapFactor);
    // cube map
    mat_diffuse = mix(mat_diffuse, texture(uCubeMap, vTexDir), uCubeMapFactor);
    mat_diffuse.rgb = sRGB_to_linear(mat_diffuse.rgb);

    vec3 mat_specular = mix(uMat.specular, texture(uSpecularMap, texCoord).rgb, uSpecularMapFactor);
    mat_specular = sRGB_to_linear(mat_specular);

    DirLight sun = uSun;
    sun.ambient = sRGB_to_linear(sun.ambient);
    sun.diffuse = sRGB_to_linear(sun.diffuse);
    sun.specular = sRGB_to_linear(sun.specular);

    SpotLight spot = uSpot;
    spot.ambient = sRGB_to_linear(spot.ambient);
    spot.diffuse = sRGB_to_linear(spot.diffuse);
    spot.specular = sRGB_to_linear(spot.specular);

    vec3 shade = calc_dir_light(sun, mat_diffuse.rgb, mat_specular) + calc_spot_light(spot, mat_diffuse.rgb, mat_specular);
    shade = linear_to_sRGB(shade);
    fFragColor = vec4(shade, mat_diffuse.a);
}
