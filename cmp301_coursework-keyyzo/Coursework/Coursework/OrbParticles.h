
#pragma once

#include "BaseMesh.h"

using namespace DirectX;

class OrbParticles : public BaseMesh
{

	// Object class based on the sphere mesh class
	// was originally used to generate particles on a custom mesh
	// now set to calculate a standard sphere mesh

public:
	OrbParticles(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~OrbParticles();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override; 
	
protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};