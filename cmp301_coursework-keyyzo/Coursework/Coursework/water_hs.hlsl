
// Water Hull Shader
// Takes in the vertices and sets 
// each of them to the correct patch 
// to then be put through to the domain shader
// The tessellation factor is also calculated inside this shader

// Takes in the current set tessellation value
// and applies it to the geometry
cbuffer TessellationValueBufferType : register(b0)
{
    int tessellationVal;
    float3 padding;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
};

// determines the tessellation value inside each patch of the object
ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;
    
    // Set the tessellation factors for the four edges of the quad.
    output.edges[0] = tessellationVal;
    output.edges[1] = tessellationVal;
    output.edges[2] = tessellationVal;
    output.edges[3] = tessellationVal;
    
    // Set the tessellation factor for tessallating inside the quad.
    output.inside[0] = tessellationVal;
    output.inside[1] = output.inside[0];
    
    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]


// passes through each vertex position, normal and tex coord 
// and sets them to patches to be used for tessellation on the geometry
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the normal for this control point as the output normal.
    output.normal = patch[pointId].normal;
    
    // Set the tex for this control point as the output tex.
    output.tex = patch[pointId].tex;
    
    // Set the world position for this control point as the output world position.
    output.worldPosition = patch[pointId].worldPosition;
    

    return output;
}

