// Shadow Pixel Shader
// Takes in all the shadow maps
// from each light and all light variables
// to calculate how the shadows and lighting
// will affect the final look of the scene
// matched with the passed in geometry information
// and final texture layer

Texture2D texture0 : register(t0);
Texture2D directionalShadowTex : register(t1);
Texture2D pointShadowTex[6] : register(t2);
Texture2D spotlightShadowTex : register(t9);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

// Directional Light Buffer

cbuffer LightBuffer : register(b0)
{
    float4 diffuse;
    float4 ambient;
    float3 direction;
    float padding;
};

// Point Light Buffer

cbuffer PointLightBuffer : register(b1)
{
    float4 diffusePoint;
    float4 ambientPoint;
    float4 positionPoint;
    float4 attenValuesPoint;
};

// Spotlight Buffer

cbuffer SpotlightBuffer : register(b2)
{
    float4 diffuseSpot;
    float4 ambientSpot;
    float3 positionSpot;
    float paddingSpot;
    float3 directionSpot;
    float coneSpot;
    float3 attenValuesSpot;
    float rangeSpot;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
    float4 pointLightViewPos[6] : TEXCOORD3;
    float4 spotlightViewPos : TEXCOORD9;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

// calculate the attenuation value of the light
// affecting the geometry based on the distance from the object
float calcAttenuation(float constantFactor, float linearFactor, float quadraticFactor, float distance)
{
    float attenuation = 1 / (constantFactor + (linearFactor * distance) + (quadraticFactor * pow(distance, 2)));
    return attenuation;

}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
	// initialise each lights colour value
    // to be calculated together at the end
    
    float4 textureColour;
    float4 lightColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 pointLightColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 spotlightColour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 finalColours = float4(0.0f, 0.0f, 0.0f, 1.0f);

    // setting up distance and attenuation 
    // variables for both point light and spotlight
    float distancePoint, distanceSpotlight;
    float attenPoint, attenSpotlight;
    
    // pre set shadowmap bias value
    // for consistent bias across all shadows
    float shadowMapBias = 0.0175f; // standard setting 0.005f
    
    textureColour = texture0.Sample(sampler0, input.tex);
    
    // calculating each lights projected tex coords
    
    float2 directionalProjectedTexCoords = getProjectiveCoords(input.lightViewPos);
    float2 pointProjectedTexCoords[6];
    
    for (int i = 0; i < 6; i++)
    {
        pointProjectedTexCoords[i] = getProjectiveCoords(input.pointLightViewPos[i]);
    }
    
    float2 spotlightProjectedTexCoords = getProjectiveCoords(input.spotlightViewPos);
    
    // calculating the point light and spotlights vector to the object
    // based on the position of the light and the objects world position
    
    float3 pointLightVector = float3(positionPoint.xyz - input.worldPosition.xyz);
    
    float3 spotlightVector = float3(positionSpot - input.worldPosition.xyz);

    // calculating the distance value for each light
    // and normalizing their respective vectors against their distance values
    // then calculating their attenuation values using the newly calculated distance value
    
    distancePoint = length(pointLightVector);
    pointLightVector /= distancePoint;
    attenPoint = calcAttenuation(attenValuesPoint.x, attenValuesPoint.y, attenValuesPoint.z, distancePoint);
    
    distanceSpotlight = length(spotlightVector);
    spotlightVector /= distanceSpotlight;
    attenSpotlight = calcAttenuation(attenValuesSpot.x, attenValuesSpot.y, attenValuesSpot.z, distanceSpotlight);
    
    
     // checks to see if the directional light has depth data
    // if so, checks if it can see the geometry from the depth data
    // if it can, light the area up 
    // if not, cast it in shadow
    if (hasDepthData(directionalProjectedTexCoords))
    {
        if (!isInShadow(directionalShadowTex, directionalProjectedTexCoords, input.lightViewPos, shadowMapBias))
        {
            lightColour = saturate(calculateLighting(-direction, input.normal, diffuse));
        }
            
    }
    
    // checks to see if the point light has depth data in each direction
    // if so, checks if it can see the geometry from the depth data
    // if it can, light the area up 
    // if not, cast it in shadow
    
    for (int j = 0; j < 6; j++)
    {
        if (hasDepthData(pointProjectedTexCoords[j]))
        {
            if (!isInShadow(pointShadowTex[j], pointProjectedTexCoords[j], input.pointLightViewPos[j], shadowMapBias))
            {
                pointLightColour = saturate(calculateLighting(pointLightVector, input.normal, diffusePoint * attenPoint));
            }
            
        }
    }
    
    // checks to see if the spot light has depth data
    // if so, checks if it can see the geometry from the depth data
    // if it can, light the area up 
    // if not, cast it in shadow
    if (hasDepthData(spotlightProjectedTexCoords))
    {
        if (!isInShadow(spotlightShadowTex, spotlightProjectedTexCoords, input.spotlightViewPos, shadowMapBias))
        {
           
            spotlightColour = saturate(calculateLighting(spotlightVector, input.normal, diffuseSpot * attenSpotlight));
                
            // carries out an extra calculation that forms the light into a cone shape
            // based on a passed in value
            spotlightColour *= saturate(pow(max(dot(-spotlightVector, directionSpot), 0.0f), coneSpot));

         
            
        }
            
    }
    
    // calculates the final look of the scene
    // by adding together all of the light values with the directional ambient colour
    // and multiplies them all with the texture to generate the correct look on the scene
    finalColours = saturate(lightColour + pointLightColour + spotlightColour + ambient) * textureColour;
	
    return finalColours;
}