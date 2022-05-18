#version 330
// ^ Change this to version 130 if you have compatibility issues

// This is a fragment shader. If you've opened this file first, please
// open and read lambert.vert.glsl before reading on.
// Unlike the vertex shader, the fragment shader actually does compute
// the shading of geometry. For every pixel in your program's output
// screen, the fragment shader is run for every bit of geometry that
// particular pixel overlaps. By implicitly interpolating the position
// data passed into the fragment shader by the vertex shader, the fragment shader
// can compute what color to apply to its pixel based on things like vertex
// position, light position, and vertex color.

uniform sampler2D u_Texture; // The color with which to render this instance of geometry.
uniform int u_Time;
uniform vec3 u_Eye;
uniform vec4 u_Player;
const float SPEED = 1 / 100.f;

// These are the interpolated values out of the rasterizer, so you can't know
// their specific values without knowing the vertices that contributed to them
in vec4 fs_Pos;
in vec4 fs_Nor;
in vec4 fs_LightVec;
in vec4 fs_UV;
in vec4 fs_CameraPos;

out vec4 out_Col; // This is the final output color that you will see on your
                  // screen for the pixel that is currently being processed.

float random1(vec3 p) {
    return fract(sin(dot(p,vec3(127.1, 311.7, 191.999)))
                 *43758.5453);
}

float mySmoothStep(float a, float b, float t) {
    t = smoothstep(0, 1, t);
    return mix(a, b, t);
}

float cubicTriMix(vec3 p) {
    vec3 pFract = fract(p);
    float llb = random1(floor(p) + vec3(0,0,0));
    float lrb = random1(floor(p) + vec3(1,0,0));
    float ulb = random1(floor(p) + vec3(0,1,0));
    float urb = random1(floor(p) + vec3(1,1,0));

    float llf = random1(floor(p) + vec3(0,0,1));
    float lrf = random1(floor(p) + vec3(1,0,1));
    float ulf = random1(floor(p) + vec3(0,1,1));
    float urf = random1(floor(p) + vec3(1,1,1));

    float mixLoBack = mySmoothStep(llb, lrb, pFract.x);
    float mixHiBack = mySmoothStep(ulb, urb, pFract.x);
    float mixLoFront = mySmoothStep(llf, lrf, pFract.x);
    float mixHiFront = mySmoothStep(ulf, urf, pFract.x);

    float mixLo = mySmoothStep(mixLoBack, mixLoFront, pFract.z);
    float mixHi = mySmoothStep(mixHiBack, mixHiFront, pFract.z);

    return mySmoothStep(mixLo, mixHi, pFract.y);
}

float fbm(vec3 p) {
    float amp = 0.5;
    float freq = 4.0;
    float sum = 0.0;
    for(int i = 0; i < 8; i++) {
        sum += cubicTriMix(p * freq) * amp;
        amp *= 0.5;
        freq *= 2.0;
    }
    return sum;
}

//rotate about x using time
vec3 rotate(vec3 light, float t){
    return vec3(light.x, cos(t) * light.y + -sin(t) * light.z, sin(t) * light.y + cos(t) * light.z);
}

void main()
{
    vec2 uv = vec2(fs_UV);

        // animated water / lava
        if (fs_UV.z != 0) {
            float inc = (u_Time % 100) * 0.01 / 16;
            uv.x += fract(inc);
        }

        vec4 diffuseColor = texture(u_Texture, uv);

        // procedural grass
        if (fs_UV.x > 8.f / 16.f &&
            fs_UV.y > 13.f / 16.f &&
            fs_UV.x < 9.f / 16.f &&
            fs_UV.y < 14.f / 16.f) {

            vec3 p = vec3((fs_Pos.xz / 16.f), 1);

            vec3 grass_1 = vec3(0.95, 0.65, 0.35);
            vec3 grass_2 = vec3(0.5, 0.5, 0.25);

            vec3 col = mix(grass_2 * diffuseColor.rgb, grass_1 * diffuseColor.rgb, fbm(p));
            vec3 color = mix(col * diffuseColor.rgb, diffuseColor.rgb, fbm(p));

            diffuseColor = vec4(color, diffuseColor.a);
        }

        //mountains
        if (fs_UV.x > 1.f / 16.f &&
            fs_UV.y > 15.f / 16.f &&
            fs_UV.x < 2.f / 16.f &&
            fs_UV.y < 16.f / 16.f) {

            vec3 p = vec3((fs_Pos.xz / 16.f), 1);

            vec3 mtn_1 = vec3(0.99, 0.99, 0.99);
            vec3 mtn_2 = vec3(0.9, 0.9, 0.9);

            vec3 col = mix(mtn_2 * diffuseColor.rgb, mtn_1 * diffuseColor.rgb, fbm(p));
            vec3 color = mix(col * diffuseColor.rgb, diffuseColor.rgb, fbm(p));

            diffuseColor = vec4(color, diffuseColor.a);
        }


       vec3 sunDir = normalize(rotate(normalize(vec3(0, 0, -1.0)), u_Time * SPEED));
       //determine which time we are in
       float timeZone = sin(u_Time * SPEED);
       vec3 light;

       if(timeZone > 0.55 || timeZone < -0.55){
           //sunrise sunset
            light = vec3(255, 194, 0) / 255.0;
       }else{
           //ease into midday
            light = mix(vec3(255, 204, 51) / 255.0, vec3(255, 194, 0) / 255.0, smoothstep(0.f, 1.f, abs(timeZone) / 0.4));
       }

        // Calculate the diffuse term for Lambert shading
        float diffuseTerm = dot(normalize(fs_Nor), normalize(fs_LightVec));
        // Avoid negative lighting values
        diffuseTerm = clamp(diffuseTerm, 0, 1);
        float spec = 0;

        //get vectors based off lecture
        vec3 view_dir = u_Eye - vec3(fs_Pos);
        vec3 halfVec = (normalize(view_dir) + normalize(sunDir)) / 2;
        //dot of h, n to power of k
        spec = clamp(pow(dot(normalize(halfVec), (normalize(vec3(fs_Nor)))), 10), 0.f, 1.f);

        if (timeZone >= -0.55 && timeZone < 0.55) {
            //smoove transition when in the proper timezone
            diffuseTerm = mix(0.f, diffuseTerm, (timeZone + 0.55) / (2.f * 0.55));
            spec = mix(0, spec, (timeZone + 0.55) / (2.f * 0.55));
        }else if (timeZone < -0.55){
            diffuseTerm = 0;
            spec = 0;
         }


        float lightIntensity = diffuseTerm + 0.2 + spec;   //Add a small float value to the color multiplier
                                                            //to simulate ambient lighting. This ensures that faces that are not
                                                            //lit by our point light are not completely black.

        // Compute final shaded color

//        out_Col = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        out_Col = vec4(diffuseColor.rgb * lightIntensity * light, diffuseColor.a);

//        // fog
        vec4 fog_color = vec4(0.5, 0.5, 0.55, 1);
        float dist = length(fs_Pos.xz - u_Player.xz) * 0.007;
        vec4 color = vec4(diffuseColor.rgb * lightIntensity, diffuseColor.a);
        color = mix(color, fog_color, pow(smoothstep(0, 1, min(1, dist)), 2));
        out_Col = vec4(color.rgb, diffuseColor.a);

}
