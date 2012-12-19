// ShaderExplodeVolumesAlpha::fragmentShaderText
#extension GL_EXT_gpu_shader4 : enable
VARYING_FRAG vec4 ColorFS;
uniform sampler2D previousDepthBuffer;
uniform int depthPeeling;
void main()
{
	ivec2 pixelXY = ivec2(gl_FragCoord.xy);
	float previousDepth = texelFetch2D(previousDepthBuffer, pixelXY, 0).z;
	float depth = gl_FragCoord.z;
	
	if (depthPeeling == 0)
	{
		gl_FragColor = ColorFS;
	}
	else if (depthPeeling == 1)
	{
		gl_FragColor = vec4(ColorFS.rgb * ColorFS.a, 1.0 - ColorFS.a);
	}
	else if (depth > previousDepth)
	{
		gl_FragColor = vec4(ColorFS.rgb * ColorFS.a, ColorFS.a);
	}
	else
	{
		discard;
	}
}

