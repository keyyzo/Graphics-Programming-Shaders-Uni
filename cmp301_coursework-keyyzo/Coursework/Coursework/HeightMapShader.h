#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class HeightMapShader : public BaseShader
{
private:

	/// Height Map Shader
	/// Takes in all the data used for the height map
	/// and generates the shadows, lights, displacement
	/// and tessellation on the height map

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

	// Calculates level of tessellation achieved on the height map

	struct TessellationValueBufferType
	{
		int tessellationVal;
		XMFLOAT3 padding;
	};

	// Changes the value of the displacement for the height map

	struct HeightMapDisplacementBufferType
	{
		float HMDisplacementVal;
		XMFLOAT3 padding;

	};

public:
	HeightMapShader(ID3D11Device* device, HWND hwnd);
	~HeightMapShader();

	// takes in all the shadow maps and matrixes needed to calculate the correct shadowing
	// from each light onto the geometry
	// passes through a texture to control the height map from the domain shader
	// and a texture created on top of that to show how it looks from the pixel shader
	// also passes in values that control the tessellation and all the light values 

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, XMMATRIX viewArr[], XMMATRIX projectionArr[],
		ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureHeight, ID3D11ShaderResourceView* directionalShadowMap, ID3D11ShaderResourceView* pointMapTextures[], ID3D11ShaderResourceView* spotShadowMap, 
		int tessVal, XMFLOAT3 attenVal, XMFLOAT3 spotAttenVal, Light* directionalLight, Light* pointLight , Light* spotLight, float coneVal, float rangeVal, float displacementVal);
private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* heightSampler;
	ID3D11SamplerState* shadowSampler;
	ID3D11Buffer* tessValBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* heightMapDVBuffer;
	ID3D11Buffer* pointLightBuffer;
	ID3D11Buffer* spotlightBuffer;
	ID3D11Buffer* shadowsBuffer;
	ID3D11Buffer* pointShadowsBuffer;
	ID3D11Buffer* spotShadowsBuffer;
};