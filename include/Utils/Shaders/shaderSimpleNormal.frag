// ShaderSimpleNormal::fragmentShaderText

PRECISON;
VARYING_FRAG vec3 normal;
FRAG_OUT_DEF;
void main()
{
	vec3 colorNormal = (normalize(normal) + vec3(1.0)) * 0.5;
	gl_FragData[0] = vec4(colorNormal, 1.0);
	gl_FragData[1] = vec4(vec3(1.0) - colorNormal, 1.0);
}
