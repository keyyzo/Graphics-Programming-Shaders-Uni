
// Water Domain Shader
// Takes in a texture which controls
// the overall look of the object
// by using a displacement method
// Also calculates the shadow matrices
// into screen space
Texture2D waterBumpMap : register(t1);
Texture2D waterNormalMap : register(t2);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Controls the overall displacement value of the object,
// changing the look of the final object in real time
// also passes in a time value and speed value
// to constantly update the position of the vector
// to act as flowing water
cbuffer WaterManipulationBuffer : register(b1)
{
    float timeVal;
    float amplitudeVal;
    float frequencyVal;
    float speedVal;
};

// Directional Light Shadow buffer
cbuffer ShadowsBuffer : register(b2)
{
    matrix lightView;
    matrix lightProjection;

};

// Point Light Shadow buffer
cbuffer PointShadowsBuffer : register(b3)
{
    matrix pointLightView[6];
    matrix pointLightProjection[6];
};

// Spotlight Shadow buffer
cbuffer SpotlightShadowsBuffer : register(b4)
{
    matrix spotlightView;
    matrix spotlightProjection;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
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

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float3 normalPosition;
    OutputType output;
 
    // Technique adapted from the 3D Game Programming with DirectX 11 Book: https://files.xray-engine.org/boox/3d_game_programming_with_DirectX11.pdf
    // Chapter 19.2.4: Displacement Mapping
    // Author: Frank Luna
    
    // Determine the position of the new vertex.
	
		
    // Lerp between each vertex position of the patch using the uv coords
    // in order to determine the actual vertex position used within the scene
    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[2].position, patch[3].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    float4 displacementVal;
    float2 textureCoords;
    
    // Lerp between each tex coords of the patch using the uv coords
    // in order to determine the actual tex coords used within the scene
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[2].tex, patch[3].tex, uvwCoord.y);
    textureCoords = lerp(t1, t2, uvwCoord.x);
    
    // Lerp between each normal position of the patch using the uv coords
    // in order to determine the actual normal position used within the scene
    float3 n1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
    float3 n2 = lerp(patch[2].normal, patch[3].normal, uvwCoord.y);
    normalPosition = lerp(n1, n2, uvwCoord.x);
    
    // passing time through to the texture coords
    // this allows for the tetxure displacement to keep
    // moving through the texture and repeating to
    // simulate the moving water in both the X and Z
    // directions of the plane
    
    textureCoords.x += (timeVal * speedVal);
    textureCoords.y += (timeVal * speedVal);
    
     // Technique adapted from the 3D Game Programming with DirectX 11 Book: https://files.xray-engine.org/boox/3d_game_programming_with_DirectX11.pdf
    // Chapter 19.2.5: Tangent and Normal Vector Estimation
    // Author: Frank Luna
    
    // Creates pre-determined cell sizes,
    // two of them deciding the UV size and mapping that
    // to the texture cell
    // with the world space cell being used in calculations to correctly
    // map the normals to each cell
    
    float texelCellSpaceU = 1.0f / 75.0f, texelCellSpaceV = 1.0f / 75.0f;
    float worldCellSpace = 1.0f / 75.0f;
    
    // calculating each part of the tex cell
    float2 leftTex = textureCoords + float2(-texelCellSpaceU, 0.0f);
    float2 rightTex = textureCoords + float2(texelCellSpaceU, 0.0f);
    float2 bottomTex = textureCoords + float2(0.0f, -texelCellSpaceV);
    float2 topTex = textureCoords + float2(0.0f, texelCellSpaceV);
    
    // sampling the texture to map the correct part of the texture 
    // to each cell
    float leftY = waterBumpMap.SampleLevel(sampler0, leftTex, 0.0f).r;
    float rightY = waterBumpMap.SampleLevel(sampler0, rightTex, 0.0f).r;
    float bottomY = waterBumpMap.SampleLevel(sampler0, bottomTex, 0.0f).r;
    float topY = waterBumpMap.SampleLevel(sampler0, topTex, 0.0f).r;
    
    // calculating the tangent and bitangent
    // using the world cell space and creating the two vectors to cross product
    // for the final normal of the cell space
    float3 tangent = normalize(float3(2.0f * worldCellSpace, rightY - leftY, 0.0f));
    float3 bitan = normalize(float3(0.0f, bottomY - topY, -2.0f * worldCellSpace));
    float3 finalNormal = cross(tangent, bitan);
    
    // set normal position to the calculated normals
    // to pass through to the pixel shader
    normalPosition = finalNormal;
    
    // samples the texture in order to build the displacement 
    // of the overall water plane
    displacementVal = waterBumpMap.SampleLevel(sampler0, textureCoords, 0.0f);
    float waterHeight = displacementVal.y;
    
    // change the y position of each vertex based on the value of the amplitude
    // from the texture and multiplying it to build up the overall amplitude
    vertexPosition.y = waterHeight * amplitudeVal;
    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // pass through the depth position
    output.depthPosition = output.position;
    
   // Calculate the position of the vertice as viewed by the light source.
    
    // directional light
    output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightView);
    output.lightViewPos = mul(output.lightViewPos, lightProjection);
    
     // point light
    for (int i = 0; i < 6; i++)
    {
        output.pointLightViewPos[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.pointLightViewPos[i] = mul(output.pointLightViewPos[i], pointLightView[i]);
        output.pointLightViewPos[i] = mul(output.pointLightViewPos[i], pointLightProjection[i]);
    }
    
    // spot light
    output.spotlightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.spotlightViewPos = mul(output.spotlightViewPos, spotlightView);
    output.spotlightViewPos = mul(output.spotlightViewPos, spotlightProjection);
    
   // Store the texture coordinates for the pixel shader.
    output.tex = textureCoords;
    
    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(normalPosition, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // calculates world position to be used for the point and spot lights within the pixel shader
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;
    
    return output;
}

