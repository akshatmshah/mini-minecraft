#version 330
// ^ Change this to version 130 if you have compatibility issues

// Refer to the lambert shader files for useful comments
uniform sampler2D u_Texture;

in vec4 fs_Col;
in vec2 fs_UV;

out vec4 out_Col;

void main()
{
    // Copy the color; there is no shading.
    out_Col = texture(u_Texture, fs_UV);
}
