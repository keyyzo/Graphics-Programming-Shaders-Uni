#include "HeightMapShader.h"

HeightMapShader::HeightMapShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	
	initShader(L"heightmap_vs.cso", L"heightmap_hs.cso", L"heightmap_ds.cso", L"heightmap_ps.cso");
}


HeightMapShader::~HeightMapShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// release the tessellation value constant buffer
	if (tessValBuffer)
	{
		tessValBuffer->Release();
		tessValBuffer = 0;
	}

	// Release the directional light constant buffer.
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}

	// Release the point light constant buffer.
	if (pointLightBuffer)
	{
		pointLightBuffer->Release();
		pointLightBuffer = 0;
	}

	// Release the spotlight constant buffer.
	if (spotlightBuffer)
	{
		spotlightBuffer->Release();
		spotlightBuffer = 0;
	}

	// Release the directional shadow buffer
	if (shadowsBuffer)
	{
		shadowsBuffer->Release();
		shadowsBuffer = 0;
	}

	// Release the point shadow buffer
	if (pointShadowsBuffer)
	{
		pointShadowsBuffer->Release();
		pointShadowsBuffer = 0;
	}

	// Release the spot shadow buffer
	if (spotShadowsBuffer)
	{
		spotShadowsBuffer->Release();
		spotShadowsBuffer = 0;
	}

	// Release the displacement constant buffer.
	if (heightMapDVBuffer)
	{
		heightMapDVBuffer->Release();
		heightMapDVBuffer = 0;
	}

	// Release the height sampler state.
	if (heightSampler)
	{
		heightSampler->Release();
		heightSampler = 0;
	}

	// Release the shadow sampler state.
	if (shadowSampler)
	{
		shadowSampler->Release();
		shadowSampler = 0;
	}


	

	//Release base shader components
	BaseShader::~BaseShader();
}

void HeightMapShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC shadowSamplerDesc;
	D3D11_BUFFER_DESC tessValBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BUFFER_DESC heightMapDVBufferDesc;
	D3D11_BUFFER_DESC pointLightBufferDesc;
	D3D11_BUFFER_DESC spotlightBufferDesc;
	D3D11_BUFFER_DESC shadowsBufferDesc;
	D3D11_BUFFER_DESC pointShadowsBufferDesc;
	D3D11_BUFFER_DESC spotShadowsBufferDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the domain shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Setup the description of the dynamic tessellation constant buffer that is in the hull shader.
	tessValBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessValBufferDesc.ByteWidth = sizeof(TessellationValueBufferType);
	tessValBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessValBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessValBufferDesc.MiscFlags = 0;
	tessValBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessValBufferDesc, NULL, &tessValBuffer);

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

	//renderer->CreateSamplerState(&samplerDesc, &heightSampler);

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

	// Setup the description of the displacement constant buffer that is in the domain shader.
	heightMapDVBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightMapDVBufferDesc.ByteWidth = sizeof(HeightMapDisplacementBufferType);
	heightMapDVBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightMapDVBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightMapDVBufferDesc.MiscFlags = 0;
	heightMapDVBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightMapDVBufferDesc, NULL, &heightMapDVBuffer);

	// Setup directional shadow buffer 
	shadowsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	shadowsBufferDesc.ByteWidth = sizeof(ShadowsBufferType);
	shadowsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	shadowsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	shadowsBufferDesc.MiscFlags = 0;
	shadowsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&shadowsBufferDesc, NULL, &shadowsBuffer);

	// Setup point shadow buffer 
	pointShadowsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	pointShadowsBufferDesc.ByteWidth = sizeof(PointShadowsBufferType);
	pointShadowsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pointShadowsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pointShadowsBufferDesc.MiscFlags = 0;
	pointShadowsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&pointShadowsBufferDesc, NULL, &pointShadowsBuffer);

	// Setup spot shadow buffer 
	spotShadowsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	spotShadowsBufferDesc.ByteWidth = sizeof(SpotlightShadowsBufferType);
	spotShadowsBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	spotShadowsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	spotShadowsBufferDesc.MiscFlags = 0;
	spotShadowsBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&spotShadowsBufferDesc, NULL, &spotShadowsBuffer);

}


void HeightMapShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void HeightMapShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, XMMATRIX viewArr[], XMMATRIX projectionArr[], ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* textureHeight, ID3D11ShaderResourceView* directionalShadowMap, ID3D11ShaderResourceView* pointMapTextures[], ID3D11ShaderResourceView* spotShadowMap, int tessVal, XMFLOAT3 attenVal,  XMFLOAT3 spotAttenVal,Light* directionalLight, Light* pointLight, Light* spotLight, float coneVal, float rangeVal, float displacementVal)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Setup the description of the tessellation constant buffer that is in the hull shader.
	TessellationValueBufferType* tessPtr;
	deviceContext->Map(tessValBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationValueBufferType*)mappedResource.pData;
	tessPtr->tessellationVal = tessVal;
	tessPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(tessValBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessValBuffer);

	// Setup the description of the dynamic matrix constant buffer that is in the domain shader.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Setup the description of the displacement constant buffer that is in the domain shader.
	HeightMapDisplacementBufferType* disPtr;
	deviceContext->Map(heightMapDVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	disPtr = (HeightMapDisplacementBufferType*)mappedResource.pData;
	disPtr->HMDisplacementVal = displacementVal;
	disPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(heightMapDVBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &heightMapDVBuffer);
	
	// Setup the description of the directional shadow buffer that is in the domain shader.
	XMMATRIX directionalLightView = XMMatrixTranspose(directionalLight->getViewMatrix());
	XMMATRIX directionalProjectionView = XMMatrixTranspose(directionalLight->getOrthoMatrix());

	ShadowsBufferType* shadowPtr;
	deviceContext->Map(shadowsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	shadowPtr = (ShadowsBufferType*)mappedResource.pData;
	shadowPtr->lightView = directionalLightView;
	shadowPtr->lightProjection = directionalProjectionView;
	/*for (int i = 0; i < 6; i++)
	{
		shadowPtr->pointLightView[i] = viewArr[i];
		shadowPtr->pointLightProjection[i] = projectionArr[i];
	}*/
	deviceContext->Unmap(shadowsBuffer, 0);
	deviceContext->DSSetConstantBuffers(2, 1, &shadowsBuffer);

	// Setup the description of the point shadow buffer that is in the domain shader.
	PointShadowsBufferType* pointShadowPtr;
	deviceContext->Map(pointShadowsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	pointShadowPtr = (PointShadowsBufferType*)mappedResource.pData;
	for (int i = 0; i < 6; i++)
	{
		pointShadowPtr->pointLightView[i] = XMMatrixTranspose(viewArr[i]);
		pointShadowPtr->pointLightProjection[i] = XMMatrixTranspose(projectionArr[i]);
	}
	deviceContext->Unmap(pointShadowsBuffer, 0);
	deviceContext->DSSetConstantBuffers(3, 1, &pointShadowsBuffer);

	// Setup the description of the spot shadow buffer that is in the domain shader.
	SpotlightShadowsBufferType* spotShadowPtr;
	deviceContext->Map(spotShadowsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	spotShadowPtr = (SpotlightShadowsBufferType*)mappedResource.pData;
	spotShadowPtr->spotlightView = XMMatrixTranspose(spotLight->getViewMatrix());
	spotShadowPtr->spotlightProjection = XMMatrixTranspose(spotLight->getProjectionMatrix());
	deviceContext->Unmap(spotShadowsBuffer, 0);
	deviceContext->DSSetConstantBuffers(4, 1, &spotShadowsBuffer);

	//Additional
	// Send light data to pixel shader
	// directional light
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse = directionalLight->getDiffuseColour();
	lightPtr->ambient = directionalLight->getAmbientColour();
	lightPtr->direction = directionalLight->getDirection();
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

	// set height map texture to domain shader
	deviceContext->DSSetShaderResources(0, 1, &textureHeight);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &directionalShadowMap);
	deviceContext->PSSetShaderResources(2, 6, pointMapTextures);
	deviceContext->PSSetShaderResources(9, 1, &spotShadowMap);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &shadowSampler);


}
