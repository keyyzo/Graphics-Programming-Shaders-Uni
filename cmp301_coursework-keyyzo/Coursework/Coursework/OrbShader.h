#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class OrbShader : public BaseShader
{
private:

	/// Orb Shader
	/// originally made to tessellate a sphere
	/// in order to change the shape of the sphere
	/// now repurposed as sending through a texture

	struct TessellationValueBufferType
	{
		XMFLOAT4 cameraPosition;
	};

public:
	OrbShader(ID3D11Device* device, HWND hwnd);
	~OrbShader();

	// sends a texture through to the object

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT4 camPos);
private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	//ID3D11Buffer* tessValBuffer;
};