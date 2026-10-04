VERTEX_LAYOUT "PARTICLE"
Properties
{
	particleTexture = (Texture2D, white)
}

GLSLPROGRAM

#version 330 core

in vec2 TexCoords;
in vec4 ColorParticle;

out vec4 FragColor;

uniform sampler2D particleTexture;

void main()
{
	vec4 texColor = texture(particleTexture, TexCoords);

	FragColor = texColor * ColorParticle;
}