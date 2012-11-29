// ShaderExplodeVolumesAlpha::vertexShaderText
ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE vec4 VertexColor;
VARYING_VERT vec4 colorVertex;
void main()
{
	colorVertex = VertexColor;
	gl_Position = vec4(VertexPosition, 1.0);
}
