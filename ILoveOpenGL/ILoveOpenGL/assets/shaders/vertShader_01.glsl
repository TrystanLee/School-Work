// Vertex shader
#version 420

//uniform mat4 MVP;
uniform mat4 matModel;
uniform mat4 matView;
uniform mat4 matProjection;
uniform mat4 matModelInverseTranspose;	// For normal calculation


uniform bool bUseVertexColour;		// Will default to GL_FALSE, which is zero (0)
uniform vec3 vertexColourOverride;

in vec4 vColour;
in vec4 vPosition;
in vec4 vNormal;				// Vertex normal X,Y,Z (W ignored)
in vec4 vUVx2;					// 2 x Texture coords (vec4) UV0, UV1
in vec4 vTangent;				// For bump mapping X,Y,Z (W ignored)
in vec4 vBiNormal;				// For bump mapping X,Y,Z (W ignored)


out vec4 fColour;		// used to be "out vec3 color"
out vec4 fVertWorldLocation;
out vec4 fNormal;
out vec4 fUVx2;

void main()
{
	// Order of these is important
	//mvp = p * v * matModel; from C++ code
	
	mat4 MVP = matProjection * matView * matModel;
			
    gl_Position = MVP * vPosition; 		// Used to be: vec4(vPosition, 1.0f);	// Used to be vPos
	
	// The location of the vertex in "world" space (not screen space)
	fVertWorldLocation = matModel * vPosition;
	
	if ( bUseVertexColour )
	{
		fColour = vec4(vertexColourOverride, 1.0f);	// Because vertexColourOverride is vec3)
	}
	else
	{
	    fColour = vColour;		// Used to be vCol
	}
	
	
	// Copy the rest of the vertex values:
	// fVertWorldLocation = vPosition;
	fNormal = vNormal;
	// fUVx2 = vUVx2;
};
