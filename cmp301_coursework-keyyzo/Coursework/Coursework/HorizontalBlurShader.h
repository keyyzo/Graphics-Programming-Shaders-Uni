#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class HorizontalBlurShader : public BaseShader
{
private:

	/// Horizontal Blur Shader
	/// Blurs a render texture of the scene
	/// in a horizontal direction

	// Passes through the screen width to apply
	// the correct amount of blur

	struct ScreenSizeBufferType
	{
		float screenWidth;
		XMFLOAT3 padding;
	};

public:

	HorizontalBlurShader(ID3D11Device* device, HWND hwnd);
	~HorizontalBlurShader();

	// takes in the render to texture and the screen width value

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float width);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenSizeBuffer;
};
