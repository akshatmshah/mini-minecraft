#version 150
in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_RenderedTexture;

uniform int u_Time;

void main()
{       
    float time = u_Time * 0.01;

    vec2 uv = fs_UV;

    uv.x += sin(uv.y * 15. + time * 2.) / 800.;
    uv.y += cos(uv.x * 10. + time * 2.) / 900.;

    uv.x += sin((uv.y+uv.x) * 15. + time * 2.) / (180. + (2. * sin(time)));
    uv.y += cos((uv.y+uv.x) * 15. + time * 2.) / (200. + (2. * sin(time)));

    vec4 diffuseColor = texture(u_RenderedTexture, uv);

    color =  vec4(.9f * diffuseColor[0], .5f * diffuseColor[1], 2.f * diffuseColor[2], diffuseColor[3]);
}
