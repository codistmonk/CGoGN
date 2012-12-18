// ShaderComputeSSAO::fragmentShaderText

PRECISON;
VARYING_FRAG vec2 texCoord;
uniform sampler2D positionTextureUnit;
uniform sampler2D normalTextureUnit;
uniform sampler2D depthTextureUnit;
uniform float SSAOStrength;
FRAG_OUT_DEF;

#define M_PI 3.1415926535897932384626433832795

const float samplingRadius = 1.0 / 50.0;                     // Radius used to get samples at each step i (radius*i)
const int samplingSteps = 2;                                 // Number of sampling steps (<=> number of circles of samples)
const int samplingDirections = 4;                            // Number of samples on each circle
const int totalSamples = samplingSteps * samplingDirections; // Total number of samples used per fragment
const float angleBias = M_PI / 12.0;                         // Angle under which occlusion will not be taken into account

float random(vec2 uv)
{
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

float computeAmbientOcclusion()
{
	float ambientOcclusion = 0.0;

	// Get position, normal and depth of the fragment from samplers
	vec4 position = texture2D(positionTextureUnit, texCoord);
	vec3 normal = texture2D(normalTextureUnit, texCoord).rgb * 2.0 - vec3(1.0);
	float depth = texture2D(depthTextureUnit, texCoord).r;
	
	// The alpha channel of the position sampler works as a mask : if it is zero, it means no fragment was rendered
	if (position.a != 0.0)
	{
		int i, j;
		int occludingSamples = 0;
		for (i = 0; i < samplingSteps; i++)
		{
			// The radius depends on the current step and the depth of the fragment
			float radius = samplingRadius * (float(i) + 1.0) / depth;
			for (j = 0; j < samplingDirections; j++)
			{
				// Find the angle of the sample that will be used : it depends on the direction but also on a random factor (avoid strips effects)
				//float theta = 2.0 * M_PI * (1.0/(1.0 + float(i)) + float(j) / float(samplingDirections));
				float theta = 2.0 * M_PI * (random(texCoord) + float(j) / float(samplingDirections));
				
				// Get the (u, v) of the sample corresponding to this radius and angle
				float x = radius * cos(theta);
				float y = radius * sin(theta);
				
				// Get the position and normal of this sample
				vec4 samplePosition = texture2D(positionTextureUnit, texCoord + vec2(x, y));
				vec3 sampleNormal = texture2D(normalTextureUnit, texCoord + vec2(x, y)).xyz;
				
				// This sample can occlude the fragment only if it was rendered
				if (samplePosition.a != 0.0)
				{
					// Vector from the fragment to the sample
					vec3 vecToSample = samplePosition.xyz - position.xyz;
					vec3 vecToSampleN = normalize(vecToSample);
					
					// This sample is an occluding one only if it is above the fragment relatively to its normal and the angle bias
					if (dot(normal, vecToSampleN) > cos(M_PI/2.0 - angleBias))
						occludingSamples ++;
				}
			}
		}
		
		// The ambient occlusion is equal to the proportion of occluding samples
		ambientOcclusion = float(occludingSamples) / float(totalSamples);
	}
	
	return ambientOcclusion * SSAOStrength;
}

void main()
{	
	gl_FragColor = vec4(vec3(1.0 - computeAmbientOcclusion()), 1.0);
}
