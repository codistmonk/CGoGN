// ShaderComputeSSAO::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D normalTextureUnit;
uniform sampler2D depthTextureUnit;
FRAG_OUT_DEF;

#define M_PI 3.1415926535897932384626433832795

const float samplingRadius = 1.0 / 50.0;
const int samplingSteps = 4;
const int samplingDirections = 4;
const int totalSamples = samplingSteps * samplingDirections;

float random(vec2 uv)
{
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 normalFromDepth(float depth, vec2 texcoords)
{  
	const vec2 offset1 = vec2(0.0, 0.001);
	const vec2 offset2 = vec2(0.001, 0.0);

	float depth1 = texture2D(depthTextureUnit, texcoords + offset1).r;
	float depth2 = texture2D(depthTextureUnit, texcoords + offset2).r;

	vec3 p1 = vec3(offset1, depth1 - depth);
	vec3 p2 = vec3(offset2, depth2 - depth);

	vec3 normal = cross(p1, p2);
	normal.z = -normal.z;

	return normalize(normal);
}

void main()
{
	float depth = texture2D(depthTextureUnit, texCoord).r;
	vec3 normal = texture2D(normalTextureUnit, texCoord).rgb * 2.0 - vec3(1.0);
	//vec3 normal = normalFromDepth(depth, texCoord);
	
	vec3 outColor = vec3(1.0);
	
	if (depth < 1.0)
	{
		int i, j;
		float occlusion = 0.0;
		int occludingSamples = 0;
		for (i = 0; i < samplingSteps; i++)
		{
			float radius = samplingRadius * (i+1);
			for (j = 0; j < samplingDirections; j++)
			{
				//float theta = 2.0 * M_PI * (1.0/(1.0 + float(i)) + float(j) / float(samplingDirections));
				float theta = 2.0 * M_PI * (random(texCoord) + float(j) / float(samplingDirections));
				
				float x = radius * cos(theta);
				float y = radius * sin(theta);
				
				float sampleDepth = texture2D(depthTextureUnit, texCoord + vec2(x, y)).r;
				
				/*vec3 vecToSample = vec3(x, y, -(sampleDepth - depth));
				vec3 vecToSampleN = normalize(vecToSample);

				float coverage = dot(normal, vecToSampleN);
				
				if (coverage > 0.2)
				{
					occlusion += coverage;
					occludingSamples ++;
				}*/
				if (depth > sampleDepth + 0.001)
					occludingSamples ++;
			}
		}
		
		float ambientOcclusion = occludingSamples / float(totalSamples);
		//float ambientOcclusion = occlusion / float(totalSamples);
		outColor = vec3(1.0 - ambientOcclusion);
	}
	
	gl_FragColor = vec4(outColor, 1.0);
}
