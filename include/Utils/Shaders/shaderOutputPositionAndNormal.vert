// ShaderOutputPositionAndNormal::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec3 VertexNormal;
uniform mat4 ModelViewProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform mat4 NormalMatrix;
VARYING_VERT vec3 position;
VARYING_VERT vec3 normal;
INVARIANT_POS;
void main ()
{
	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);
	
	position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
	normal = (NormalMatrix * vec4 (VertexNormal, 0.0)).xyz;
}

