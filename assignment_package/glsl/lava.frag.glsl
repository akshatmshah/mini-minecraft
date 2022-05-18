#version 150
in vec2 fs_UV;

out vec4 color;

uniform sampler2D u_RenderedTexture;

uniform int u_Time;


void main()
{
    float time = u_Time * 0.01;

    vec2 p = fs_UV;

    p.x += sin(p.y * 15. + time * 2.) / 800.;
    p.y += cos(p.x * 10. + time * 2.) / 900.;

    p.x += sin((p.y+p.x) * 15. + time * 2.) / (180. + (2. * sin(time)));
    p.y += cos((p.y+p.x) * 15. + time * 2.) / (200. + (2. * sin(time)));

    vec4 diffuseColor = texture(u_RenderedTexture, p);
    color = vec4(2.f * diffuseColor[0], .5f * diffuseColor[1], .5f * diffuseColor[2], diffuseColor[3]);
}
