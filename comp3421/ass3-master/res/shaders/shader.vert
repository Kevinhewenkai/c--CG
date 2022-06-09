#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vPosition;
// vector from the camera to the object
out vec3 vView;
// the texture direction of the cube map
out vec3 vTexDir;
noperspective out vec2 vPortalTexCoord;

uniform mat4 uViewProj;
uniform mat4 uModel;
uniform vec3 uCameraPos;
uniform vec4 uClipPlane;

out float gl_ClipDistance[1];

void main() {
    vTexCoord = aTexCoord;
    vNormal = normalize(mat3(uModel) * aNormal);
    vPosition = (uModel * aPos).xyz;
    vView = normalize(uCameraPos - vPosition);
    gl_Position = uViewProj * uModel * aPos;

    // Part A: calculate texture direction for cubemap
    vTexDir = reflect(-vView, vNormal);

    // Part C: calculate portal tex coords
    vPortalTexCoord = (gl_Position.xy / gl_Position.w) * 0.5 + vec2(0.5);

    // Part D: set gl_ClipDistance[0] to be the distance between vPosition and the clipping plane
    gl_ClipDistance[0] = dot(vec4(vPosition, 1), uClipPlane);
}
