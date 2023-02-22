// Depth Vertex Shader
// Used for objects that are passing through 
// geometry data straight to the vertex shader
// and not making use of the domain or hull shader
// in order to calculate the object's depth

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;
    
};

OutputType main(InputType input)
{
    OutputType output;

    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.depthPosition = output.position;
	
    return output;
}