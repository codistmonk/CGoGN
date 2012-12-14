// ShaderOutputPositionAndNormal::fragmentShaderText

PRECISON;
VARYING_VERT vec3 position;
VARYING_FRAG vec3 normal;
FRAG_OUT_DEF;
void main()
{
	gl_FragData[0] = vec4(position, 1.0);
	gl_FragData[1] = vec4((normalize(normal) + vec3(1.0)) * 0.5, 1.0);
}
