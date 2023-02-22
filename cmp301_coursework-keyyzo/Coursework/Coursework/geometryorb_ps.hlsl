// Geometry Orb Pixel Shader
// Takes in the colour of each vertex
// generated from the geometry shader
// and colours each vertex to the appropriate colour


Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 colour : COLOR;
};


float4 main(InputType input) : SV_TARGET
{
	
    return input.colour;

}