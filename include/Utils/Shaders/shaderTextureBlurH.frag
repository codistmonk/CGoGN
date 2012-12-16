// ShaderTextureBlurH::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D textureUnit;
uniform float blurSize;
FRAG_OUT_DEF;
void main()
{
	vec4 blurSum = 0.0;
	
	sum += texture2D(textureUnit, texCoord - vec2(4.0*blurSize, 0.0)) * 0.06;
	sum += texture2D(textureUnit, texCoord - vec2(3.0*blurSize, 0.0)) * 0.09;
	sum += texture2D(textureUnit, texCoord - vec2(2.0*blurSize, 0.0)) * 0.12;
	sum += texture2D(textureUnit, texCoord - vec2(blurSize, 0.0)) * 0.15;
	sum += texture2D(textureUnit, texCoord) * 0.16;
	sum += texture2D(textureUnit, texCoord + vec2(blurSize, 0.0)) * 0.15;
	sum += texture2D(textureUnit, texCoord + vec2(2.0*blurSize, 0.0)) * 0.12;
	sum += texture2D(textureUnit, texCoord + vec2(3.0*blurSize, 0.0)) * 0.09;
	sum += texture2D(textureUnit, texCoord + vec2(4.0*blurSize, 0.0)) * 0.06;
	
	gl_FragColor = blurSum;
}
