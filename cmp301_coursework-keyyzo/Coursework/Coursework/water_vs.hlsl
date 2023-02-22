
// Water Vertex Shader
// Simple pass through vertex shader, taking the input
// vertices and passing them to the hull shader 

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

OutputType main(InputType input)
{
    OutputType output;

	// Pass the vertex position, normal and tex coords into the hull shader
    output.position = input.position;
    output.normal = input.normal;
    output.tex = input.tex;
    
    // pass through world position to hull shader
    input.worldPosition = output.worldPosition;
    
    
    return output;
    
}