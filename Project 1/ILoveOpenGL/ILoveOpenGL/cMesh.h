#pragma once

// This represents a single mesh object we are drawing

#include <string>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

class cMesh
{
public:
	cMesh();
	std::string meshName;

	glm::vec3 positionXYZ;
	glm::vec3 orientationXYZ;		// "angle"
	float scale;

	// Sets the overall colour of the object
	//	by overwriting the colour variable
	// HACK: We'll get rid of this once we have textures
	glm::vec3 vertexColourOverrideHACK;
	bool bOverriveVertexColourHACK;


	bool bIsWireframe;

};
