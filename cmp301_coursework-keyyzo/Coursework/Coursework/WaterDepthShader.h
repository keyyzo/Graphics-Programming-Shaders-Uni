#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterDepthShader : public BaseShader
{
private:


	/// Water Plane Depth Shader
	/// Uses the water plane vertex data
	/// to build a correct depth map in 
	/// order to cast shadows onto it

	// passes through a timer value
	// that actively affects the overall speed and amplitude
	// at which the water plane moves with
	// and actively changes this value on the depth map

	struct WaterManipulationBufferType
	{
		float timeVal;
		float amplitudetVal;
		float frequencyVal;
		float speedVal;

	};

	// Calculates level of tessellation achieved on the water plane
	// and actively changes this value on the depth map

	struct TessellationValueBufferType
	{
		int tessellationVal;
		XMFLOAT3 padding;
	};

public:
	WaterDepthShader(ID3D11Device* device, HWND hwnd);
	~WaterDepthShader();

	// Takes in the change in tessellation factor and the values that affect the waters movement

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, int tessVal, float timeV, float amplitudeV, float frequencyV, float speedV);
private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* tessValBuffer;
	ID3D11Buffer* waterManipulationBuffer;

};