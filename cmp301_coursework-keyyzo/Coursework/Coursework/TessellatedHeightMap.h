#pragma once


#include "BaseMesh.h"

using namespace DirectX;

class TessellatedHeightMap : public BaseMesh
{

	// Tessellated Plane Mesh
	// changes the topology to 4-point patchlist
	// in order to allow the plane to be tessellated

public:
	/** \brief Initialises and builds a plane mesh
	*
	* Can specify resolution of plane, this deteremines how many subdivisions of the plane.
	* @param device is the renderer device
	* @param device context is the renderer device context
	* @param resolution is a int for subdivision of the plane. The number of unit quad on each axis. Default is 100.
	*/
	TessellatedHeightMap(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int resolution = 100);
	~TessellatedHeightMap();

	void sendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top = D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST) override;

protected:
	void initBuffers(ID3D11Device* device);
	int resolution;
};