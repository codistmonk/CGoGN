// ShaderExplodeVolumesAlpha::fragmentShaderText
#extension GL_EXT_gpu_shader4 : enable
VARYING_FRAG vec4 ColorFS;
uniform sampler2D previousDepthBuffer;
void main()
{
	ivec2 pixelXY = ivec2(gl_FragCoord.xy);
	float previousDepth = texelFetch2D(previousDepthBuffer, pixelXY, 0).z;
	float depth = gl_FragCoord.z;
	
	if ((previousDepth == 1.0) || (int(depth * 10000.0) > int(previousDepth * 10000.0)))
	{
		gl_FragColor = ColorFS;
	}
	else
	{
		discard;
	}
}

