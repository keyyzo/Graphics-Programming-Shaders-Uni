#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class DepthOfFieldShader : public BaseShader
{
private:


public:
	
	/// Depth of Field Shader
	/// Uses the Circle of Confusion technique to calculate 
	/// which areas are in focus or not

	struct CicleOfConfusionBufferType
	{
		float nearPlaneCoC;
		float farPlaneCoC;
		float apertureCoC;
		float focalLengthCoC;
		float planeInFocusCoC;
		float blurEnabledCoC;
		XMFLOAT2 padding;


	};

	DepthOfFieldShader(ID3D11Device* device, HWND hwnd);
	~DepthOfFieldShader();

	// takes in a blurred and non-blurred texture view of the scene
	// with a depth pass from the camera
	// alongside CoC values that control which areas of the scene are blurred or not
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* nonBlurredTex, ID3D11ShaderResourceView* blurredTex, ID3D11ShaderResourceView* cameraDepth,
		float nearPlane, float farPlane, float aperture, float focalLength, float planeInFocus, float blurEnabled); 

private:

	void initShader(const wchar_t* vs, const wchar_t* ps);

private:

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* shadowMapSampler;
	ID3D11Buffer* CoCBuffer;



};

