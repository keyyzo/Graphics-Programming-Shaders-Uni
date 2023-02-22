#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class GeometryOrbShader : public BaseShader
{
private:

	/// Geometry Shader built to generate line primitives
	/// from an object using a trianglelist topology
	/// The line primitives generated show the vertex normals
	/// and the face normals from the object

	// buffer that allows the generated vertices to change 
	// in length and decide whether or not they will generate 
	// based on a passed in value

	struct VisualNormalsBuffer
	{
		float vertexNormalSize;
		float faceNormalSize;
		float vertexNormalActive;
		float faceNormalActive;
	};


public:

	GeometryOrbShader(ID3D11Device* device, HWND hwnd);
	~GeometryOrbShader();

	// Takes in the size values of each type of normal generated
	// and a value that controls whether they are active or not

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture,
		float vNormalSize, float fNormalSize, float vNormalActive, float fNormalActive);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* normalsBuffer;
	ID3D11SamplerState* sampleState;
};