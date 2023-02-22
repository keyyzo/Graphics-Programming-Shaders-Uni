// Shadow Vertex Shader
// calculates the shadow matrices
// into screen space for objects
// which wont make use of the hull or
// domain shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Directional Light Shadow buffer

cbuffer ShadowsBuffer : register(b1)
{
    matrix lightView;
    matrix lightProjection;

};

// Point Light Shadow buffer

cbuffer PointShadowsBuffer : register(b2)
{
    matrix pointLightView[6];
    matrix pointLightProjection[6];
};

// Spotlight Shadow buffer

cbuffer SpotlightShadowsBuffer : register(b3)
{
    matrix spotlightView;
    matrix spotlightProjection;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
    float4 pointLightViewPos[6] : TEXCOORD3;
    float4 spotlightViewPos : TEXCOORD9;
    float4 depthPosition : TEXCOORD10;
};


OutputType main(InputType input)
{
    OutputType output;
    
     // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.depthPosition = output.position;
    
   // Calculate the position of the vertice as viewed by the light source.
    
    // directional light
    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightView);
    output.lightViewPos = mul(output.lightViewPos, lightProjection);
    
    // point light
    for (int i = 0; i < 6; i++)
    {
        output.pointLightViewPos[i] = mul(input.position, worldMatrix);
        output.pointLightViewPos[i] = mul(output.pointLightViewPos[i], pointLightView[i]);
        output.pointLightViewPos[i] = mul(output.pointLightViewPos[i], pointLightProjection[i]);
    }
    
    // spot light
    output.spotlightViewPos = mul(input.position, worldMatrix);
    output.spotlightViewPos = mul(output.spotlightViewPos, spotlightView);
    output.spotlightViewPos = mul(output.spotlightViewPos, spotlightProjection);
    
   // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // calculates world position to be used for the point and spot lights within the pixel shader
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    
	return output;
}