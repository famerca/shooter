#version 410

layout(location = 0) in vec3 pos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = pos;
    mat4 view_no_translation = mat4(mat3(view));
    vec4 pos_clip = projection * view_no_translation * vec4(pos, 1.0);
    gl_Position = pos_clip.xyww;
}

