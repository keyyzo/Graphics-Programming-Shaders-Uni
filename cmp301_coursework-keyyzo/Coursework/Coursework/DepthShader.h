
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{
private:


	/// Height Map Depth Shader
	/// Uses the Height Map vertex data
	/// to build a correct depth map in 
	/// order to cast shadows onto it

	// Calculates level of tessellation achieved on the height map
	// and actively changes this value on the depth map

	struct TessellationValueBufferType
	{
		int tessellationVal;
		XMFLOAT3 padding;
	};

	// Changes the value of the displacement for the height map
	// and actively changes this value on the depth map

	struct HeightMapDisplacementBufferType
	{
		float HMDisplacementVal;
		XMFLOAT3 padding;

	};

public:

	DepthShader(ID3D11Device* device, HWND hwnd);
	~DepthShader();

	// Takes in the change in tessellation factor and the value of displacement on the height map

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, int tessVal, Light* directionalLight, float displacementVal);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* heightMapDVBuffer;
	ID3D11Buffer* tessValBuffer;
};