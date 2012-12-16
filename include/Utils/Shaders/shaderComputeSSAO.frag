// ShaderComputeSSAO::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D positionTextureUnit;
uniform sampler2D normalTextureUnit;
uniform sampler2D depthTextureUnit;
FRAG_OUT_DEF;

#define M_PI 3.1415926535897932384626433832795

const float samplingRadius = 1.0 / 50.0;
const int samplingSteps = 2;
const int samplingDirections = 4;
const int totalSamples = samplingSteps * samplingDirections;

float random(vec2 uv)
{
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	vec4 position = texture2D(positionTextureUnit, texCoord);
	vec3 normal = texture2D(normalTextureUnit, texCoord).rgb * 2.0 - vec3(1.0);
	float depth = texture2D(depthTextureUnit, texCoord).r;
	
	vec3 outColor = vec3(1.0);
	
	if (position.a != 0.0)
	{
		int i, j;
		float occlusion = 0.0;
		int occludingSamples = 0;
		for (i = 0; i < samplingSteps; i++)
		{
			float radius = samplingRadius * (i+1) / depth;
			for (j = 0; j < samplingDirections; j++)
			{
				//float theta = 2.0 * M_PI * (1.0/(1.0 + float(i)) + float(j) / float(samplingDirections));
				float theta = 2.0 * M_PI * (random(texCoord) + float(j) / float(samplingDirections));
				
				float x = radius * cos(theta);
				float y = radius * sin(theta);
				
				vec4 samplePosition = texture2D(positionTextureUnit, texCoord + vec2(x, y));
				vec3 sampleNormal = texture2D(normalTextureUnit, texCoord + vec2(x, y));
				
				if (samplePosition.a != 0.0)
				{
					vec3 vecToSample = samplePosition.xyz - position.xyz;
					vec3 vecToSampleN = normalize(vecToSample);
				
					float sampleOcclusion = 1.0 / (1.0 + length(vecToSample));
					//sampleOcclusion *= 1.0 - abs(dot(normal, sampleNormal));
					sampleOcclusion *= max(0.0, dot(normal, vecToSampleN));
				
					occlusion += sampleOcclusion;
				
					if (dot(normal, vecToSampleN) > 0.2)
						occludingSamples ++;
				}
			}
		}
		
		float ambientOcclusion = occludingSamples / float(totalSamples);
		//float ambientOcclusion = occlusion / float(totalSamples);
		outColor = vec3(1.0 - ambientOcclusion);
	}
	
	gl_FragColor = vec4(outColor, 1.0);
}
