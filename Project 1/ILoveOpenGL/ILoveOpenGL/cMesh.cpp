#include "cMesh.h"

cMesh::cMesh()
{
	this->positionXYZ = glm::vec3(0.0f, 0.0f, 0.0f);
	this->orientationXYZ = glm::vec3(0.0f, 0.0f, 0.0f);

	this->scale = 1.0f;

	this->bIsWireframe = false;

	this->vertexColourOverrideHACK = glm::vec3(1.0f, 1.0f, 1.0f);	// White
	this->bOverriveVertexColourHACK = false;

	//Or you could do this
//	this->positionXYZ.x = 0.0f;
//	this->positionXYZ.y = 0.0f;
//	this->positionXYZ.z = 0.0f;
	// etc...
}