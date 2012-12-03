// ShaderSimpleNormal::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec3 VertexNormal;
uniform mat4 ModelViewProjectionMatrix;
VARYING_VERT vec3 normal;
INVARIANT_POS;
void main ()
{
	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);
	normal = VertexNormal;
}

