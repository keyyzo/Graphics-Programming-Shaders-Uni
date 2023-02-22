// Depth of Field Vertex Shader
// Takes in a matrix buffer 
// and recalculates each vertex position 
// using the passed through matrices

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex1 : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex1 : TEXCOORD0;
    float4 depthPosition : TEXCOORD3;
};

OutputType main(InputType input)
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.depthPosition = output.position;
    
	// Store the texture coordinates for the pixel shader.
    output.tex1 = input.tex1;
 

    return output;
}