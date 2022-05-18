#version 150

uniform mat4 u_ViewProj;    // We're actually passing the inverse of the viewproj
                            // from our CPU, but it's named u_ViewProj so we don't
                            // have to bother rewriting our ShaderProgram class

uniform ivec2 u_Dimensions; // Screen dimensions

uniform vec3 u_Eye; // Camera pos

uniform int u_Time;

out vec3 outColor;

const float PI = 3.14159265359;
const float TWO_PI = 6.28318530718;

const float SPEED = 1 / 100.f;

// Sunset palette
const vec3 sunset[5] = vec3[](vec3(255, 229, 119) / 255.0,
                               vec3(254, 192, 81) / 255.0,
                               vec3(255, 137, 103) / 255.0,
                               vec3(253, 96, 81) / 255.0,
                               vec3(57, 32, 51) / 255.0);
// Dusk palette
const vec3 dusk[5] = vec3[](vec3(144, 96, 144) / 255.0,
                            vec3(96, 72, 120) / 255.0,
                            vec3(72, 48, 120) / 255.0,
                            vec3(48, 24, 96) / 255.0,
                            vec3(0, 24, 72) / 255.0);

const vec3 day[5] = vec3[](vec3(144, 96, 144) / 255.0,
                           vec3(96, 72, 120) / 255.0,
                           vec3(72, 48, 120) / 255.0,
                           vec3(48, 24, 96) / 255.0,
                           vec3(0, 24, 72) / 255.0);

//make a sun palette for the colors of the sun
const vec3 sun[2] = vec3[](vec3(255, 194, 0) / 255.0,
                          vec3(255, 204, 51) / 255.0);

const vec3 cloudColor = sunset[3];

//This maps the 3d coordinate on the sphere to a 2d UV coord
//interpolates between the two closest colors
vec2 sphereToUV(vec3 p) {
    //horizon angle
    //angle -> 0 to 2 * pi
    float phi = atan(p.z, p.x);
    if(phi < 0) {
        phi += TWO_PI;
    }
    //vert angle
    float theta = acos(p.y);
    //map 0 to 1
    return vec2(1 - phi / TWO_PI, 1 - theta / PI);
}

//takes 2d coordinate and maps to the sunset palette we have
//interpolates between the two closest colors
vec3 uvToSunset(vec2 uv) {
    if(uv.y < 0.5) {
        return day[0];
    }
    else if(uv.y < 0.55) {
        return mix(sunset[0], sunset[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(sunset[1], sunset[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(sunset[2], sunset[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(sunset[3], sunset[4], (uv.y - 0.65) / 0.1);
    }
    return sunset[4];
}

//takes 2d coordinate and maps to the dusk palette we have
//interpolates between the two closest colors
vec3 uvToDusk(vec2 uv) {
    if(uv.y < 0.5) {
        return dusk[0];
    }
    else if(uv.y < 0.55) {
        return mix(dusk[0], dusk[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(dusk[1], dusk[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(dusk[2], dusk[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(dusk[3], dusk[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4];
}

//takes 2d coordinate and maps to the dusk palette we have
//interpolates between the two closest colors
vec3 uvToDay(vec2 uv) {
    if(uv.y < 0.5) {
        return day[0];
    }
    else if(uv.y < 0.55) {
        return mix(day[0], day[1], (uv.y - 0.5) / 0.05);
    }
    else if(uv.y < 0.6) {
        return mix(day[1], day[2], (uv.y - 0.55) / 0.05);
    }
    else if(uv.y < 0.65) {
        return mix(day[2], day[3], (uv.y - 0.6) / 0.05);
    }
    else if(uv.y < 0.75) {
        return mix(day[3], day[4], (uv.y - 0.65) / 0.1);
    }
    return dusk[4];
}


vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3 random3( vec3 p ) {
    return fract(sin(vec3(dot(p,vec3(127.1, 311.7, 191.999)),
                          dot(p,vec3(269.5, 183.3, 765.54)),
                          dot(p, vec3(420.69, 631.2,109.21))))
                 *43758.5453);
}

float WorleyNoise3D(vec3 p)
{
    // Tile the space
    vec3 pointInt = floor(p);
    vec3 pointFract = fract(p);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int z = -1; z <= 1; z++)
    {
        for(int y = -1; y <= 1; y++)
        {
            for(int x = -1; x <= 1; x++)
            {
                vec3 neighbor = vec3(float(x), float(y), float(z));

                // Random point inside current neighboring cell
                vec3 point = random3(pointInt + neighbor);

                // Animate the point
                point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

                // Compute the distance b/t the point and the fragment
                // Store the min dist thus far
                vec3 diff = neighbor + point - pointFract;
                float dist = length(diff);
                minDist = min(minDist, dist);
            }
        }
    }
    return minDist;
}

float WorleyNoise(vec2 uv)
{
    // Tile the space
    vec2 uvInt = floor(uv);
    vec2 uvFract = fract(uv);

    float minDist = 1.0; // Minimum distance initialized to max.

    // Search all neighboring cells and this cell for their point
    for(int y = -1; y <= 1; y++)
    {
        for(int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));

            // Random point inside current neighboring cell
            vec2 point = random2(uvInt + neighbor);

            // Animate the point
            point = 0.5 + 0.5 * sin(u_Time * 0.01 + 6.2831 * point); // 0 to 1 range

            // Compute the distance b/t the point and the fragment
            // Store the min dist thus far
            vec2 diff = neighbor + point - uvFract;
            float dist = length(diff);
            minDist = min(minDist, dist);
        }
    }
    return minDist;
}

float worleyFBM(vec3 uv) {
    float sum = 0;
    float freq = 4;
    float amp = 0.5;
    for(int i = 0; i < 8; i++) {
        sum += WorleyNoise3D(uv * freq) * amp;
        freq *= 2;
        amp *= 0.5;
    }
    return sum;
}

vec3 rotate(vec3 light, float t){
    return vec3(light.x, cos(t) * light.y + -sin(t) * light.z, sin(t) * light.y + cos(t) * light.z);
}

void main()
{
    //first need to go from pixel space to world space
    vec2 ndc = (gl_FragCoord.xy / vec2(u_Dimensions)) * 2.0 - 1.0; // -1 to 1 NDC
    vec4 p = vec4(ndc.xy, 1, 1); // Pixel at the far clip plane
    p *= 1000.0; // Times far clip plane value
    p = /*Inverse of*/ u_ViewProj * p; // Convert from unhomogenized screen to world

    //camera to point
    vec3 rayDir = normalize(p.xyz - u_Eye);
    vec2 uv = sphereToUV(rayDir);
    vec2 offset = vec2(0.0);
    // Get a noise value in the range [-1, 1]
    // by using Worley noise as the noise basis of FBM (for clouds)
    offset = vec2(worleyFBM(rayDir));
    offset *= 2.0;
    offset -= vec2(1.0);

    // Compute a gradient from the bottom of the sky-sphere to the top
    vec3 sunsetColor = uvToSunset(uv + offset * 0.1);
    vec3 duskColor = uvToDusk(uv + offset * 0.1);
    vec3 dayColor = uvToDay(uv + offset * 0.1);

    float timeZone = sin(u_Time * SPEED);
    vec3 sunCol;
    vec3 skyCol;

    // Add a glowing sun in the sky
    //direction for sun moving (going about x with u_Time)
    vec3 sunDir = normalize(rotate(normalize(vec3(0, 0, -1.0)), u_Time * SPEED));

    if(timeZone <= 0.55 || timeZone >= -0.55){
        //we need to slowly mix from the color we came from to the sunset or sunrise
        skyCol = mix(dayColor, duskColor, (timeZone + 0.55) / (0.55 * 2.f));
        sunCol = mix(sun[0], sun[1], smoothstep(0.f, 1.f, abs(timeZone) / 0.55));
    }else{
        //if day or night we have the same sun color
        skyCol = dayColor;
        sunCol = sun[0];
    }
    float sunSize = 30;
    float angle = acos(dot(rayDir, sunDir)) * 360.0 / PI;
    float raySunDot = dot(rayDir, sunDir);
    float t = (raySunDot - 0.75) / (-0.1 - 0.75);

    //if our angle (which is ray direction and the center of sun direction) is less than this threshold that means we are looking towards the sun
    if(angle < sunSize) {
            // Full center of sun
            if(angle < 7.5) {
                outColor = sunCol;
            }
            // Corona of sun, mix with sky color
            else {
                outColor = mix(sunCol, skyCol, (angle - 7.5) / 22.5);
                //if we are sunrise of sunset
                if (timeZone <= 0.55 && timeZone >= -0.55) {
                    //mix the color from where we came from
                    //didn't look good enough so smooth stepped
                    outColor = mix(outColor, mix(sunCol, sunsetColor, (angle - 7.5) / 22.5), smoothstep(0.f, 1.f, 1.f - abs(timeZone) / 0.55));
                }
            }
        }else {
            // Otherwise our ray is looking into just the sky
            outColor = skyCol;
            // again, if we are at sunrise or sunset
            if (timeZone <= 0.55 && timeZone >= -0.55) {
                if(raySunDot > 0.75) {
                    outColor = mix(outColor, sunsetColor, smoothstep(0.f, 1.f, 1.f - abs(timeZone) / 0.55));
                }else if(raySunDot > -0.1) {
                    // Any dot product between 0.75 and -0.1 is a LERP b/t sunset and dusk color
                    //need to linearly interpolate as we got dot product within the threshold
                    outColor = mix(outColor, mix(sunsetColor, skyCol, t), smoothstep(0.f, 1.f, 1.f - abs(timeZone) / 0.55));
                }else {
                    // Any dot product <= -0.1 are pure dusk color
                    //if it isn't (less than 0.1) then the sky only
                    outColor = mix(outColor, skyCol, smoothstep(0.f, 1.f, 1.f - abs(timeZone) / 0.55));
                }

            }
        }
}
