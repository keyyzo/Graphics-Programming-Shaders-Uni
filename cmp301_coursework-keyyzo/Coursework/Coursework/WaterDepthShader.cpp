#include "WaterDepthShader.h"

WaterDepthShader::WaterDepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{

	initShader(L"water_vs.cso", L"water_hs.cso", L"water_ds.cso", L"depth_ps.cso");
}


WaterDepthShader::~WaterDepthShader()
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

	// release the tessellation value constant buffer
	if (waterManipulationBuffer)
	{
		waterManipulationBuffer->Release();
		waterManipulationBuffer = 0;
	}


	//Release base shader components
	BaseShader::~BaseShader();
}

void WaterDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC tessValBufferDesc;
	D3D11_BUFFER_DESC waterManipulationBufferDesc;

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

	// Setup the description of the water manipulation buffer that is in the domain shader.
	waterManipulationBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterManipulationBufferDesc.ByteWidth = sizeof(WaterManipulationBufferType);
	waterManipulationBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterManipulationBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterManipulationBufferDesc.MiscFlags = 0;
	waterManipulationBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&waterManipulationBufferDesc, NULL, &waterManipulationBuffer);

}


void WaterDepthShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void WaterDepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, int tessVal, float timeV, float amplitudeV, float frequencyV, float speedV)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// Lock the tessellation buffer so it can be written to.
	TessellationValueBufferType* tessPtr;
	deviceContext->Map(tessValBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessellationValueBufferType*)mappedResource.pData;
	tessPtr->tessellationVal = tessVal;
	tessPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);
	deviceContext->Unmap(tessValBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &tessValBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	// Lock the water manipulation buffer so it can be written to.
	WaterManipulationBufferType* waterPtr;
	deviceContext->Map(waterManipulationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	waterPtr = (WaterManipulationBufferType*)mappedResource.pData;
	waterPtr->timeVal = timeV;
	waterPtr->amplitudetVal = amplitudeV;
	waterPtr->frequencyVal = frequencyV;
	waterPtr->speedVal = speedV;
	deviceContext->Unmap(waterManipulationBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &waterManipulationBuffer);

}
