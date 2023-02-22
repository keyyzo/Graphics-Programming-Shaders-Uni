#include "ShadowShader.h"

ShadowShader::ShadowShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"shadow_vs.cso", L"shadow_ps.cso");
}


ShadowShader::~ShadowShader()
{
	// release the sampler state
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// release the shadow sampler state
	if (shadowSampler)
	{
		shadowSampler->Release();
		shadowSampler = 0;
	}

	// release the matrix buffer
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// release the layout
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// release the directional light buffer
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// release the point light buffer
	if (pointLightBuffer)
	{
		pointLightBuffer->Release();
		pointLightBuffer = 0;
	}

	// release the spotlight buffer
	if (spotlightBuffer)
	{
		spotlightBuffer->Release();
		spotlightBuffer = 0;
	}

	// release the directional shadow buffer
	if (shadowsBuffer)
	{
		shadowsBuffer->Release();
		shadowsBuffer = 0;
	}

	// release the point shadows buffer
	if (pointShadowsBuffer)
	{
		pointShadowsBuffer->Release();
		pointShadowsBuffer = 0;
	}

	// release the spotlight buffer
	if (spotShadowsBuffer)
	{
		spotShadowsBuffer->Release();
		spotShadowsBuffer = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void ShadowShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC shadowSamplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC pointLightBufferDesc;
	D3D11_BUFFER_DESC spotlightBufferDesc;
	D3D11_BUFFER_DESC shadowsBufferDesc;
	D3D11_BUFFER_DESC pointShadowsBufferDesc;
	D3D11_BUFFER_DESC spotShadowsBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// sampler for shadow map sampling
	shadowSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 1.0f;
	shadowSamplerDesc.BorderColor[1] = 1.0f;
	shadowSamplerDesc.BorderColor[2] = 1.0f;
	shadowSamplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&shadowSamplerDesc, &shadowSampler);

	// Setup directional light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

	// Setup point light buffer
	pointLightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointLightBufferDesc.ByteWidth = sizeof(PointLightBufferType);
	pointLightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointLightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointLightBufferDesc.MiscFlags = 0;
	pointLightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&pointLightBufferDesc, NULL, &pointLightBuffer);

	// Setup spotlight buffer
	spotlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	spotlightBufferDesc.ByteWidth = sizeof(SpotlightBufferType);
	spotlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	spotlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	spotlightBufferDesc.MiscFlags = 0;
	spotlightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&spotlightBufferDesc, NULL, &spotlightBuffer);

	// Setup directional shadows buffer
	shadowsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	shadowsBufferDesc.ByteWidth = sizeof(ShadowsBufferType);
	shadowsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	shadowsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	shadowsBufferDesc.MiscFlags = 0;
	shadowsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&shadowsBufferDesc, NULL, &shadowsBuffer);

	// Setup point shadows buffer
	pointShadowsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointShadowsBufferDesc.ByteWidth = sizeof(PointShadowsBufferType);
	pointShadowsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointShadowsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointShadowsBufferDesc.MiscFlags = 0;
	pointShadowsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&pointShadowsBufferDesc, NULL, &pointShadowsBuffer);

	// Setup spot shadows buffer
	spotShadowsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	spotShadowsBufferDesc.ByteWidth = sizeof(SpotlightShadowsBufferType);
	spotShadowsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	spotShadowsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	spotShadowsBufferDesc.MiscFlags = 0;
	spotShadowsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&spotShadowsBufferDesc, NULL, &spotShadowsBuffer);

}


void ShadowShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMMATRIX viewArr[], XMMATRIX projectionArr[], ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* directionalShadowMap, ID3D11ShaderResourceView* pointMapTextures[],
	ID3D11ShaderResourceView* spotShadowMap, XMFLOAT3 attenVal, XMFLOAT3 spotAttenVal, Light* waterLight, Light* pointLight, Light* spotLight, float coneVal, float rangeVal)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lock the directional shadows buffer so it can be written to.
	XMMATRIX directionalLightView = XMMatrixTranspose(waterLight->getViewMatrix());
	XMMATRIX directionalProjectionView = XMMatrixTranspose(waterLight->getOrthoMatrix());

	ShadowsBufferType* shadowPtr;
	deviceContext->Map(shadowsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	shadowPtr = (ShadowsBufferType*)mappedResource.pData;
	shadowPtr->lightView = directionalLightView;
	shadowPtr->lightProjection = directionalProjectionView;
	deviceContext->Unmap(shadowsBuffer, 0);
	deviceContext->VSSetConstantBuffers(1, 1, &shadowsBuffer);

	// Lock the point shadows buffer so it can be written to.
	PointShadowsBufferType* pointShadowPtr;
	deviceContext->Map(pointShadowsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pointShadowPtr = (PointShadowsBufferType*)mappedResource.pData;
	for (int i = 0; i < 6; i++)
	{
		pointShadowPtr->pointLightView[i] = XMMatrixTranspose(viewArr[i]);
		pointShadowPtr->pointLightProjection[i] = XMMatrixTranspose(projectionArr[i]);
	}
	deviceContext->Unmap(pointShadowsBuffer, 0);
	deviceContext->VSSetConstantBuffers(2, 1, &pointShadowsBuffer);

	// Lock the spot shadows buffer so it can be written to.
	SpotlightShadowsBufferType* spotShadowPtr;
	deviceContext->Map(spotShadowsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotShadowPtr = (SpotlightShadowsBufferType*)mappedResource.pData;
	spotShadowPtr->spotlightView = XMMatrixTranspose(spotLight->getViewMatrix());
	spotShadowPtr->spotlightProjection = XMMatrixTranspose(spotLight->getProjectionMatrix());
	deviceContext->Unmap(spotShadowsBuffer, 0);
	deviceContext->VSSetConstantBuffers(3, 1, &spotShadowsBuffer);

	//Additional
	// Send light data to pixel shader

	// directional light
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse = waterLight->getDiffuseColour();
	lightPtr->ambient = waterLight->getAmbientColour();
	lightPtr->direction = waterLight->getDirection();
	lightPtr->padding = 0.0f;
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	// point light
	PointLightBufferType* pointLightPtr;
	deviceContext->Map(pointLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pointLightPtr = (PointLightBufferType*)mappedResource.pData;
	pointLightPtr->diffusePoint = pointLight->getDiffuseColour();
	pointLightPtr->ambientPoint = pointLight->getAmbientColour();
	pointLightPtr->positionPoint = XMFLOAT4(pointLight->getPosition().x, pointLight->getPosition().y, pointLight->getPosition().z, 0.0f);
	pointLightPtr->attenValuesPoint = XMFLOAT4(attenVal.x, attenVal.y, attenVal.z, 0.0f);
	deviceContext->Unmap(pointLightBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &pointLightBuffer);

	// spotlight
	SpotlightBufferType* spotlightPtr;
	deviceContext->Map(spotlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotlightPtr = (SpotlightBufferType*)mappedResource.pData;
	spotlightPtr->diffuseSpot = spotLight->getDiffuseColour();
	spotlightPtr->ambientSpot = spotLight->getAmbientColour();
	spotlightPtr->positionSpot = spotLight->getPosition();
	spotlightPtr->directionSpot = spotLight->getDirection();
	spotlightPtr->attenValuesSpot = spotAttenVal;
	spotlightPtr->coneSpot = coneVal;
	spotlightPtr->rangeSpot = rangeVal;
	spotlightPtr->paddingSpot = 0.0f;
	deviceContext->Unmap(spotlightBuffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &spotlightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &directionalShadowMap);
	deviceContext->PSSetShaderResources(2, 6, pointMapTextures);
	deviceContext->PSSetShaderResources(9, 1, &spotShadowMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &shadowSampler);
}

