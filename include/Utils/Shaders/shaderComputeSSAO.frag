// ShaderComputeSSAO::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D normalTextureUnit;
uniform sampler2D depthTextureUnit;
FRAG_OUT_DEF;
void main()
{
	vec4 normal = texture2D(normalTextureUnit, texCoord);
	float depth = texture2D(depthTextureUnit, texCoord).r;
	
	gl_FragColor = normal * (1.0 - depth);
}
