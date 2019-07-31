#version 400

layout(vertices = 9) out;

void main()
{
    switch(gl_InvocationID) {
        
        // corners, stay in same place
        
        case 0:
            gl_out[gl_InvocationID].gl_Position = gl_in[0].gl_Position;
            break;
        
        case 2:
            gl_out[gl_InvocationID].gl_Position = gl_in[1].gl_Position;
            break;
            
        case 6:
            gl_out[gl_InvocationID].gl_Position = gl_in[3].gl_Position;
            break;
            
        case 8:
            gl_out[gl_InvocationID].gl_Position = gl_in[2].gl_Position;
            break;

        // edge midpoints
    
        case 1:
            gl_out[gl_InvocationID].gl_Position =
                vec4((gl_in[0].gl_Position.xyz+gl_in[1].gl_Position.xyz)*0.7, 1);
            break;
            
        case 3:
            gl_out[gl_InvocationID].gl_Position =
                vec4((gl_in[0].gl_Position.xyz+gl_in[3].gl_Position.xyz)*0.7, 1);
            break;
            
        case 5:
            gl_out[gl_InvocationID].gl_Position =
                vec4((gl_in[1].gl_Position.xyz+gl_in[2].gl_Position.xyz)*0.7, 1);
            break;
            
        case 7:
            gl_out[gl_InvocationID].gl_Position =
                vec4((gl_in[2].gl_Position.xyz+gl_in[3].gl_Position.xyz)*0.7, 1);
            break;

        // center

        case 4:
            gl_out[gl_InvocationID].gl_Position =
                vec4((gl_in[0].gl_Position.xyz+gl_in[1].gl_Position.xyz
                     +gl_in[2].gl_Position.xyz+gl_in[3].gl_Position.xyz)*0.5, 1);
            break;
    }
    
    gl_TessLevelOuter[0] = 8;
    gl_TessLevelOuter[1] = 8;
    gl_TessLevelOuter[2] = 8;
    gl_TessLevelOuter[3] = 8;
    gl_TessLevelInner[0] = 8;
    gl_TessLevelInner[1] = 8;
}
