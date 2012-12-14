// ShaderOutputNormal::fragmentShaderText

PRECISON;
VARYING_FRAG vec3 normal;
FRAG_OUT_DEF;
void main()
{
	gl_FragColor = vec4((normalize(normal) + vec3(1.0)) * 0.5, 1.0);
}
