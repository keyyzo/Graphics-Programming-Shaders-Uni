
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class GenericDepthShader : public BaseShader
{
private:

	/// Standard depth shader
	/// Used for objects that don't need specialised depth shaders
	/// Such as objects that don't tessellate and make use of the hull and domain shaders


public:

	GenericDepthShader(ID3D11Device* device, HWND hwnd);
	~GenericDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
private:
	ID3D11Buffer* matrixBuffer;

};