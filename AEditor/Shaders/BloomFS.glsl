GLSLPROGRAM
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D SceneTexture;

void main()
{	
	vec3 BrightColor = texture(SceneTexture, TexCoord).rgb;
	float lum = dot(BrightColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(lum > 1.0)
		FragColor = vec4(BrightColor, 1.0);
	else
		FragColor = vec4(0.0, 0.0, 0.0, 1.0);
}