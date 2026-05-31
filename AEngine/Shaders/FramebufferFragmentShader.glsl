GLSLPROGRAM
#version 330 core

in vec2 TexCoord;
out vec4 FragColor;
uniform sampler2D ScreenTexture;
uniform sampler2D ExposureTexture;
uniform sampler2D BloomTexture;
uniform bool HDR;
uniform bool Bloom;
uniform bool autoExposure;
uniform float exposure;

void main()
{
	vec3 FinalColor = texture(ScreenTexture, TexCoord).rgb;
	float finalExposure = exposure;
	if(autoExposure)
		finalExposure = texture(ExposureTexture, vec2(0.5)).r;
	if(Bloom)
	{
		vec3 bloomColor = texture(BloomTexture, TexCoord).rgb;
		FinalColor += bloomColor;
	}
	if(HDR)
	{
		vec3 mapped = vec3(1.0) - exp(-FinalColor * finalExposure);
		FinalColor = mapped;
	}
	FinalColor = pow(FinalColor, vec3(1.0 / 2.2));
	FragColor = vec4(FinalColor, 1.0);
}