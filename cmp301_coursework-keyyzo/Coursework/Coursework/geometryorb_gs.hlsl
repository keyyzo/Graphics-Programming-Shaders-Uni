// Geometry Orb Generation Geometry Shader
// The goal of this shader is to generate
// line primitive vertices to show off
// the vertex normals and the face normals
// of any object making use of this shader
// as long as it is using a trianglelist topology

// Takes in a matrix buffer 
// and recalculates each vertex position 
// using the passed through matrices

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Passes in a buffer which controls the size of the generated geometry
// and whether or not the geometry can be generated based on a value 

cbuffer VisualNormalsBuffer : register(b1)
{
    float vertexNormalSize;
    float faceNormalSize;
    float vertexNormalActive;
    float faceNormalActive;
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
    float3 normal : NORMAL;
    float4 colour : COLOR;
};

// Technique adapted from the geeks3d website: https://www.geeks3d.com/20130905/exploring-glsl-normal-visualizer-with-geometry-shaders-shader-library/
// Exploring GLSL - Normal Visualizer with Geometry Shaders (Shader Library)
// Author: JEGX

// The geometry created from this shader
// will be a maximum of eight vertices,
// six of them being generated for each vertex normal 
// from the passed in trianglelists of the object
// with another two being generated for each face normal
// of each trianglelist 
// the eight vertices generated will form into a line,
// outputting from each type of normal and direction of the normal from the shape

[maxvertexcount(8)]
void main(triangle InputType input[3], inout LineStream<OutputType> triStream)
{
    OutputType output;
    
    // loops for each vertex within the trianglelist
    for (int i = 0; i < 3; i++)
    {
        // saves each vertex and normal position
        // to a variable
        
        float3 vPosition = input[i].position.xyz;
        float3 normalPosition = input[i].normal.xyz;
     
        // Calculate the position of the first vertex against the world, view, and projection matrices.
        output.position = mul(float4(vPosition.xyz, 1.0f), worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        
        // colours the vertex to differentiate against the other generated vertices
        output.colour = float4(1.0f, 0.0f, 0.0f, 1.0f);
        
        // if the vertex normals have been activated then generate
        // the vertex
        if (vertexNormalActive == 1.0f)
        {
            triStream.Append(output);
        }
        
        // Calculate the normal vector against the world matrix only and normalise.
        output.normal = mul(input[i].normal.xyz, (float3x3) worldMatrix);
        output.normal = normalize(output.normal);
        
        // Calculate the position of the second vertex against the world, view, and projection matrices.
        // this vertex adds together both vertex position and normal position
        // multiplied by a slider to determine how long the line generated from the normal will be
        
        output.position = mul(float4(vPosition + normalPosition * vertexNormalSize, 1.0f), worldMatrix);
        output.position = mul(output.position, viewMatrix);
        output.position = mul(output.position, projectionMatrix);
        
        // colours the vertex to differentiate against the other generated vertices
        output.colour = float4(0.0f, 1.0f, 0.0f, 1.0f);
        
        // if the vertex normals have been activated then generate
        // the vertex
        if (vertexNormalActive == 1.0f)
        {
            triStream.Append(output);
        }
        
        // if the vertex normals have been activated then
        // restart the strip to move on to the next set of vertices
        // to be generated
        if (vertexNormalActive == 1.0f)
        {
            triStream.RestartStrip();
        }
        
    }
    
    // saves each vertex position
    // from the triangle to their respective variables
        
    float3 vPositionOne = input[0].position.xyz;
    float3 vPositionTwo = input[1].position.xyz;
    float3 vPositionThree = input[2].position.xyz;
    
    // create two seperate vectors
    // from the three vertices in order to cross prodcut
    // the values to calculate the face normal
    float3 vP1toP2 = vPositionOne - vPositionTwo;
    float3 vP3toP2 = vPositionThree - vPositionTwo;
   
    // calculate the face normal position
    // by using a cross product on the two vectors
    // then normalizing the final result to achieve
    // the correct position of the face normal
    float3 normalPosition = cross(vP3toP2, vP1toP2);
    normalPosition = normalize(normalPosition);
    
    // calculate the middle position of the triangle
    // by adding the position of all vertices together from the triangle
    // and dividing them by the total number of vertices
    
    float3 faceCentre = (vPositionOne + vPositionTwo + vPositionThree) / 3.0f;
    
    // Calculate the position of the first vertex against the world, view, and projection matrices.
    output.position = mul(float4(faceCentre.xyz, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // colours the vertex to differentiate against the other generated vertices
    output.colour = float4(0.0f, 1.0f, 1.0f, 1.0f);
    
    // if the face normals have been activated then generate
    // the vertex
    if (faceNormalActive == 1.0f)
    {
        triStream.Append(output);
    }
    
    // Calculate the position of the second vertex against the world, view, and projection matrices.
    // this vertex takes away the normal position from the vertex position,
    // multiplied by a slider to determine how long the line generated from the face normal will be
    output.position = mul(float4(faceCentre.xyz - normalPosition * faceNormalSize, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.colour = float4(1.0f, 0.0f, 1.0f, 1.0f);
    
    // if the face normals have been activated then generate
    // the vertex
    if (faceNormalActive == 1.0f)
    {
        triStream.Append(output);
    }
    
    // if the face normals have been activated then
    // restart the strip to move on to the next set of vertices
    // to be generated or finish generating
    if (faceNormalActive == 1.0f)
    {
        triStream.RestartStrip();
    }
    
    
}