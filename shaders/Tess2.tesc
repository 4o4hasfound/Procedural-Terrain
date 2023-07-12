#version 410 core

// Quads
layout (vertices = 4) out;

in vec2 uvs[];
out vec2 uvsCoord[];

uniform vec3 cameraPos;
uniform vec2 chunkPos;
uniform float size;
uniform bool doTess;

const int MIN_TES = 8;
const int MAX_TES = 32;
const float MIN_DIST = 0.0;
const float MAX_DIST = 1000.0;
const float tessLev = 4.0;

int SmoothTessLevel(int MinTess, int MaxTess, float MinDis, float MaxDis, float dis){
    return int(mix(MaxTess, MinTess, clamp((dis - MinDis) / MaxDis, 0.0, 1.0)));
}


int getTessLevel(float dis){
    if (dis <= size * 4) return SmoothTessLevel(25, 30, 0.0, size * 4, dis);
    else if (dis <= size * 6) return SmoothTessLevel(25, 15, size * 4, size * 6, dis);
    else return SmoothTessLevel(3, 15, size * 6, MAX_DIST, dis);
}

void main()
{
	//gl_out[0].gl_ClipDistance[0] = 1.0;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    uvsCoord[gl_InvocationID] = uvs[gl_InvocationID];
    if (gl_InvocationID == 0)
    {
        if (doTess){
            vec4 center0 = gl_in[0].gl_Position + (gl_in[3].gl_Position - gl_in[0].gl_Position) / 2.0; // left side
            vec4 center1 = gl_in[1].gl_Position + (gl_in[0].gl_Position - gl_in[1].gl_Position) / 2.0; // bot side
            vec4 center2 = gl_in[2].gl_Position + (gl_in[1].gl_Position - gl_in[2].gl_Position) / 2.0; // right side
            vec4 center3 = gl_in[3].gl_Position + (gl_in[2].gl_Position - gl_in[3].gl_Position) / 2.0; // top side
        
            float dist0 = length(vec4(0.0, cameraPos.y, 0.0, 0.0) - center0);
            float dist1 = length(vec4(0.0, cameraPos.y, 0.0, 0.0) - center1);
            float dist2 = length(vec4(0.0, cameraPos.y, 0.0, 0.0) - center2);
            float dist3 = length(vec4(0.0, cameraPos.y, 0.0, 0.0) - center3);
            int tes0 = getTessLevel(dist0);
            int tes1 = getTessLevel(dist1);
            int tes2 = getTessLevel(dist2);
            int tes3 = getTessLevel(dist3);
    //        int tes0 = int(mix(MAX_TES, MIN_TES, clamp(dist0 / MAX_DIST, 0.0, 1.0)));
    //        int tes1 = int(mix(MAX_TES, MIN_TES, clamp(dist1 / MAX_DIST, 0.0, 1.0)));
    //        int tes2 = int(mix(MAX_TES, MIN_TES, clamp(dist2 / MAX_DIST, 0.0, 1.0)));
    //        int tes3 = int(mix(MAX_TES, MIN_TES, clamp(dist3 / MAX_DIST, 0.0, 1.0)));
    //        int tes0 = int(mix(MAX_TES, MIN_TES, tanh(2*(clamp(dist0 / MAX_DIST, 0.0, 1.0)))));
    //        int tes1 = int(mix(MAX_TES, MIN_TES, tanh(2*(clamp(dist1 / MAX_DIST, 0.0, 1.0)))));
    //        int tes2 = int(mix(MAX_TES, MIN_TES, tanh(2*(clamp(dist2 / MAX_DIST, 0.0, 1.0)))));
    //        int tes3 = int(mix(MAX_TES, MIN_TES, tanh(2*(clamp(dist3 / MAX_DIST, 0.0, 1.0)))));

            gl_TessLevelOuter[0] = tes0; // left for quads
            gl_TessLevelOuter[1] = tes1; // bot for quads
            gl_TessLevelOuter[2] = tes2; // right for quads
            gl_TessLevelOuter[3] = tes3; // top for quads
        
            gl_TessLevelInner[0] = max(tes1, tes3); // top bot for quads
            gl_TessLevelInner[1] = max(tes0, tes2); // left right for quads
        }
        else{
            gl_TessLevelOuter[0] = tessLev; // left for quads
            gl_TessLevelOuter[1] = tessLev; // bot for quads
            gl_TessLevelOuter[2] = tessLev; // right for quads
            gl_TessLevelOuter[3] = tessLev; // top for quads
        
            gl_TessLevelInner[0] = tessLev; // top bot for quads
            gl_TessLevelInner[1] = tessLev; // left right for quads
        }
    }
    
	//gl_out[0].gl_ClipDistance[0] = 12;
}
