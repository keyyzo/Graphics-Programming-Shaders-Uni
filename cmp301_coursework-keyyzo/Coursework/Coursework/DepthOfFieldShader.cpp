#include "DepthOfFieldShader.h"

DepthOfFieldShader::DepthOfFieldShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"depthoffield_vs.cso", L"depthoffield_ps.cso");
}

DepthOfFieldShader::~DepthOfFieldShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// release the main sample state
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// release the shadow sampler
	if (shadowMapSampler)
	{
		shadowMapSampler->Release();
		shadowMapSampler = 0;
	}
	
	// release the circle of confusion buffer
	if (CoCBuffer)
	{
		CoCBuffer->Release();
		CoCBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthOfFieldShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_SAMPLER_DESC shadowMapSamplerDesc;
	D3D11_BUFFER_DESC CoCBufferDesc;


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

	// setup the description of the circle of confusion buffer thats sent to the pixel shader
	CoCBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	CoCBufferDesc.ByteWidth = sizeof(CicleOfConfusionBufferType);
	CoCBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CoCBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CoCBufferDesc.MiscFlags = 0;
	CoCBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&CoCBufferDesc, NULL, &CoCBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// sampler for shadow map sampling
	shadowMapSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	shadowMapSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowMapSamplerDesc.BorderColor[0] = 1.0f;
	shadowMapSamplerDesc.BorderColor[1] = 1.0f;
	shadowMapSamplerDesc.BorderColor[2] = 1.0f;
	shadowMapSamplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&shadowMapSamplerDesc, &shadowMapSampler);


}

void DepthOfFieldShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* nonBlurredTex, ID3D11ShaderResourceView* blurredTex, ID3D11ShaderResourceView* cameraDepth, float nearPlane, float farPlane, float aperture, float focalLength, float planeInFocus, float blurEnabled)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lock the CoC buffer so it can be written to.
	CicleOfConfusionBufferType* cocPtr;
	deviceContext->Map(CoCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cocPtr = (CicleOfConfusionBufferType*)mappedResource.pData;
	cocPtr->nearPlaneCoC = nearPlane;
	cocPtr->farPlaneCoC = farPlane;
	cocPtr->apertureCoC = aperture;
	cocPtr->focalLengthCoC = focalLength;
	cocPtr->planeInFocusCoC = planeInFocus;
	cocPtr->blurEnabledCoC = blurEnabled;
	cocPtr->padding = XMFLOAT2(0.0f, 0.0f);
	deviceContext->Unmap(CoCBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &CoCBuffer);

	// sending the samplers and textures passed in to the pixel shader
	deviceContext->PSSetShaderResources(0, 1, &nonBlurredTex);
	deviceContext->PSSetShaderResources(1, 1, &blurredTex);
	deviceContext->PSSetShaderResources(2, 1, &cameraDepth);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &shadowMapSampler);
}