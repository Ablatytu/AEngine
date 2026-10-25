GLSLPROGRAM
#version 330 core

const float offset = 1.0 / 300.0;  
float blur_kernel[9] = float[](
    1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0,
    2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0,
    1.0f / 16.0, 2.0 / 16.0, 1.0 / 16.0
);
vec2 offsets[9] = vec2[](
     vec2(-offset,  offset), // top-left
     vec2( 0.0f,    offset), // top-center
     vec2( offset,  offset), // top-right
     vec2(-offset,  0.0f),   // center-left
     vec2( 0.0f,    0.0f),   // center-center
     vec2( offset,  0.0f),   // center-right
     vec2(-offset, -offset), // bottom-left
     vec2( 0.0f,   -offset), // bottom-center
     vec2( offset, -offset)  // bottom-right    
);
int edge_kernel[9] = int[](
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    );
uniform sampler2D ScreenTexture;
uniform bool chaos;
uniform bool confuse;
uniform bool shake;
in vec2 TexCoord;
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0f);
    vec3 sample[9];
    if(chaos || shake)
        for(int i = 0; i < 9; i++)
            sample[i] = vec3(texture(ScreenTexture, TexCoord.st + offsets[i]));

    if(chaos)
    {
        for(int i = 0; i < 9; i++)
            FragColor += vec4(sample[i] * edge_kernel[i], 0.0);
        FragColor.a = 1.0f;
    }
    else if (confuse)
    {
        FragColor = vec4(1.0 - texture(ScreenTexture, TexCoord).rgb, 1.0);
    }
    else if (shake)
    {
        for(int i = 0; i < 9; i++)
            FragColor += vec4(sample[i] * blur_kernel[i], 0.0);
        FragColor.a = 1.0;
    }
    else
    {
        FragColor = texture(ScreenTexture, TexCoord);
    }
}