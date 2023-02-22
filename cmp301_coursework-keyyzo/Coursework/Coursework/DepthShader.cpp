#include "DepthShader.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"heightmap_vs.cso", L"heightmap_hs.cso", L"heightmap_ds.cso",L"depth_ps.cso");
}

DepthShader::~DepthShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// release the height map buffer
	if (heightMapDVBuffer)
	{
		heightMapDVBuffer->Release();
		heightMapDVBuffer = 0;
	}

	// release the tessellation buffer
	if (tessValBuffer)
	{
		tessValBuffer->Release();
		tessValBuffer = 0;
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

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC heightMapDVBufferDesc;
	D3D11_BUFFER_DESC tessValBufferDesc;

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

	// Setup the description of the dynamic tessellation constant buffer that is in the hull shader.
	tessValBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessValBufferDesc.ByteWidth = sizeof(TessellationValueBufferType);
	tessValBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessValBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessValBufferDesc.MiscFlags = 0;
	tessValBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&tessValBufferDesc, NULL, &tessValBuffer);

	// Setup the description of the displacement constant buffer that is in the domain shader.
	heightMapDVBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	heightMapDVBufferDesc.ByteWidth = sizeof(HeightMapDisplacementBufferType);
	heightMapDVBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	heightMapDVBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	heightMapDVBufferDesc.MiscFlags = 0;
	heightMapDVBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&heightMapDVBufferDesc, NULL, &heightMapDVBuffer);

}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, int tessVal, Light* directionalLight, float displacementVal)
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
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lock the tessellation buffer so it can be written to.
	TessellationValueBufferType* tessPtr;
	deviceContext->Map(tessValBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationValueBufferType*)mappedResource.pData;
	tessPtr->tessellationVal = tessVal;
	tessPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(tessValBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessValBuffer);

	// Lock the displacement buffer so it can be written to.
	HeightMapDisplacementBufferType* disPtr;
	deviceContext->Map(heightMapDVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	disPtr = (HeightMapDisplacementBufferType*)mappedResource.pData;
	disPtr->HMDisplacementVal = displacementVal;
	disPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(heightMapDVBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &heightMapDVBuffer);

}
