// ShaderExplodeVolumesAlpha::fragmentShaderText
#extension GL_EXT_gpu_shader4 : enable
VARYING_FRAG vec4 ColorFS;
uniform sampler2D previousDepthBuffer;
void main()
{
	ivec2 pixelXY = ivec2(gl_FragCoord.xy);
	float previousZ = texelFetch2D(previousDepthBuffer, pixelXY, 0).z;
	float z = gl_FragCoord.z;
	
	if (int(z * 10000.0) != int(previousZ * 10000.0))
	{
		gl_FragColor = ColorFS;
	}
	else
	{
		discard;
	}
}

