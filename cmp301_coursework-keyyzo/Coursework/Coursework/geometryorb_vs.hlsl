
// Geometry Orb Generation Vertex Shader
// Simply passes through an input type with specific variables
// No calculations done in this shader

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

InputType main(InputType input)
{
    return input;
}
