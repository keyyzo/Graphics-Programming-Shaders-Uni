#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class ShadowShader : public BaseShader
{
private:
	
	/// Shadow Shader
	/// used to calculate how shadows and lights affect
	/// standard objects that dont make use of the hull and domain shader

	// Shadow Buffers for each different type of light
	// passes through their respective view and projection matrix to the domain shader
	// to correctly calculate shadows onto the geometry

	// Directional Light Shadow Buffer

	struct ShadowsBufferType
	{
		XMMATRIX lightView;
		XMMATRIX lightProjection;

	};

	// Point Light Shadow Buffer

	struct PointShadowsBufferType
	{
		XMMATRIX pointLightView[6];
		XMMATRIX pointLightProjection[6];
	};

	// Spotlight Shadow Buffer

	struct SpotlightShadowsBufferType
	{
		XMMATRIX spotlightView;
		XMMATRIX spotlightProjection;

	};

	// Light Buffers for each different type of light
	// passes through their respective values to the pixel shader
	// to correctly calculate how much their light affects the geometry

	// Directional Light Buffer

	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT4 ambient;
		XMFLOAT3 direction;
		float padding;
	};

	// Point Light Buffer

	struct PointLightBufferType
	{
		XMFLOAT4 diffusePoint;
		XMFLOAT4 ambientPoint;
		XMFLOAT4 positionPoint;
		XMFLOAT4 attenValuesPoint;
	};

	// Spotlight Buffer

	struct SpotlightBufferType
	{
		XMFLOAT4 diffuseSpot;
		XMFLOAT4 ambientSpot;
		XMFLOAT3 positionSpot;
		float paddingSpot;
		XMFLOAT3 directionSpot;
		float coneSpot;
		XMFLOAT3 attenValuesSpot;
		float rangeSpot;


	};

public:

	ShadowShader(ID3D11Device* device, HWND hwnd);
	~ShadowShader();

	// takes in all the shadow maps and matrixes needed to calculate the correct shadowing
	// from each light onto the geometry
	// also takes in a texture for the object and all the light values use to affect the object

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMMATRIX viewArr[], XMMATRIX projectionArr[], ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* directionalShadowMap, ID3D11ShaderResourceView* pointMapTextures[],
		ID3D11ShaderResourceView* spotShadowMap, XMFLOAT3 attenVal, XMFLOAT3 spotAttenVal, Light* waterLight, Light* pointLight, Light* spotLight, float coneVal, float rangeVal);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* shadowSampler;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* pointLightBuffer;
	ID3D11Buffer* spotlightBuffer;
	ID3D11Buffer* shadowsBuffer;
	ID3D11Buffer* pointShadowsBuffer;
	ID3D11Buffer* spotShadowsBuffer;
};