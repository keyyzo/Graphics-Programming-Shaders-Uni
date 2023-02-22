#pragma once

#include "BaseMesh.h"

using namespace DirectX;

class TessellatedOrb : public BaseMesh
{

public:
	TessellatedOrb(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 20);
	~TessellatedOrb();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) override; //D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};

// D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST