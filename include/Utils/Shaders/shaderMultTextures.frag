// ShaderMultTextures::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D texture1Unit;
uniform sampler2D texture2Unit;
FRAG_OUT_DEF;
void main()
{
	vec3 color1 = texture2D(texture1Unit, texCoord).rgb;
	vec3 color2 = texture2D(texture2Unit, texCoord).rgb;
	gl_FragColor = vec4(color1 * color2, 1.0);
}
