// ShaderTextureBlurV::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D textureUnit;
uniform float blurSize;
FRAG_OUT_DEF;
void main()
{
	vec4 blurSum = vec4(0.0);
	
	blurSum += texture2D(textureUnit, texCoord - vec2(0.0, 4.0*blurSize)) * 0.06;
	blurSum += texture2D(textureUnit, texCoord - vec2(0.0, 3.0*blurSize)) * 0.09;
	blurSum += texture2D(textureUnit, texCoord - vec2(0.0, 2.0*blurSize)) * 0.12;
	blurSum += texture2D(textureUnit, texCoord - vec2(0.0, blurSize)) * 0.15;
	blurSum += texture2D(textureUnit, texCoord) * 0.16;
	blurSum += texture2D(textureUnit, texCoord + vec2(0.0, blurSize)) * 0.15;
	blurSum += texture2D(textureUnit, texCoord + vec2(0.0, 2.0*blurSize)) * 0.12;
	blurSum += texture2D(textureUnit, texCoord + vec2(0.0, 3.0*blurSize)) * 0.09;
	blurSum += texture2D(textureUnit, texCoord + vec2(0.0, 4.0*blurSize)) * 0.06;
	
	gl_FragColor = blurSum;
}
