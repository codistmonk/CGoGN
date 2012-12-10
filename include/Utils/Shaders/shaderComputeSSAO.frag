// ShaderComputeSSAO::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D normalTextureUnit;
uniform sampler2D depthTextureUnit;
FRAG_OUT_DEF;

#define M_PI 3.1415926535897932384626433832795

const float samplingRadius = 1.0 / 200.0;
const int samplingSteps = 4;
const int samplingDirections = 4;
const int totalSamples = samplingSteps * samplingDirections;

void main()
{
	vec3 normal = texture2D(normalTextureUnit, texCoord).rgb * 2.0 - vec3(1.0);
	float depth = texture2D(depthTextureUnit, texCoord).r;
	
	vec3 outColor = vec3(1.0);
	
	if (depth < 1.0)
	{
		int i, j;
		float occlusion = 0.0;
		int occludingSamples = 0;
		for (i = 0; i < samplingSteps; i++)
		{
			float radius = samplingRadius * (i+1) / depth;
			for (j = 0; j < 4; j++)
			{
				float theta = 2.0 * M_PI * (1.0 / (float(i) + 1.0) + float(j) / float(samplingDirections));
				
				float x = radius * cos(theta);
				float y = radius * sin(theta);
				
				float sampleDepth = texture2D(depthTextureUnit, texCoord + vec2(x, y)).r;
				
				vec3 vecToSample = vec3(x, y, -(sampleDepth - depth));
				vec3 vecToSampleN = normalize(vecToSample);

				occlusion += max(dot(normal, vecToSampleN), 0.0);
			}
		}
		
		float ambientOcclusion = occlusion / float(totalSamples);
		outColor = vec3(1.0 - ambientOcclusion);
	}
	
	gl_FragColor = vec4(outColor, 1.0);
}
