Properties
{
	SpriteImage = (Texture2D, white)
	SpriteColor = (Color, (1.0, 1.0, 1.0, 1.0))
}
GLSLPROGRAM
#version 330 core

uniform sampler2D SpriteImage;
uniform vec4 SpriteColor;

in vec2 TexCoords;
out vec4 fragColor;

void main()
{
	vec4 textureColor =  texture(SpriteImage, TexCoords);
	if(textureColor.a < 0.1)
		discard;
	fragColor = vec4(SpriteColor.rgb, 1.0) * textureColor;
}