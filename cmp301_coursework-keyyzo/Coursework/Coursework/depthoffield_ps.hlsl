
// Depth of Field Pixel Shader
// Takes in two textures, one non-blurred texture of the scene
// and one blurred texture of the scene being blurred with a gaussian blur technique
// with a depth map based from the camera view passed through as well
// to determine which pixels will have the applied depth of field technique

Texture2D nonBlurredTexture : register(t0);
Texture2D blurredTexture : register(t1);
Texture2D cameraDepthTexture : register(t2);

SamplerState sampleType : register(s0);
SamplerState shadowMapSampler : register(s1);

// Circle of Confusion buffer values
// controls how the circle of confusion affects the overall
// depth of field applied to the scene

cbuffer CircleOfConfusionBuffer : register(b0)
{
    float nearPlaneCoC;
    float farPlaneCoC;
    float apertureCoC;
    float focalLengthCoC;
    float planeInFocusCoC;
    float blurEnabledCoC;
    float2 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex1 : TEXCOORD0;
    float4 depthPosition : TEXCOORD3;
};

float4 main(InputType input) : SV_TARGET
{
    
    float4 nonBlurredTex, blurredTex;
    float cameraDepthTex;
    
    // saving each texture to a variable
    
    nonBlurredTex = nonBlurredTexture.Sample(sampleType, input.tex1);
    blurredTex = blurredTexture.Sample(sampleType, input.tex1);
    
    // saving the depth data to a variable
    cameraDepthTex = cameraDepthTexture.Sample(shadowMapSampler, input.tex1).r;
    
    /// Technique adapted from the official nvidia website: https://developer.nvidia.com/gpugems/gpugems/part-iv-image-processing/chapter-23-depth-field-survey-techniques#:~:text=Depth%20of%20field%20is%20the,range%20appear%20out%20of%20focus.&text=The%20diameter%20of%20the%20CoC,from%20the%20plane%20in%20focus.
    /// Depth of Field: A Survey of Techniques
    /// Author: Joe Demers, NVIDIA
    
    // The overall object distance is calculated using the Z-values of each plane and the Z-value calculated
    // from the camera depth map
    float objectDistance = -farPlaneCoC * nearPlaneCoC / (cameraDepthTex * (farPlaneCoC - nearPlaneCoC) - farPlaneCoC);
    
    // The Circle of Confusion effect replicates how camera lenses would 
    // focus in on an object of interest based on the distance of the plane in focus,
    // blurring everything thats outside this plane of focus and creating the circle of confusion
    // The circle of confusion can be affected in different ways, such as changing the diameter of the circle with
    // the aperture and changing the focal range to decide which area within the plane of focus wil be focused on
    float coc = abs(apertureCoC * (focalLengthCoC * (objectDistance - planeInFocusCoC)) / (objectDistance * (planeInFocusCoC - focalLengthCoC)));
    
    // check if the post processing is enabled
    if (blurEnabledCoC == 1.0f)
    {
        
        
        // check the overall CoC value 
        if (coc < 1.0f)
        {
            // lerps between the blurred and non-blurred
            // texture to make the depth of field effect
            // look more natural and smooth
            return lerp(nonBlurredTex, blurredTex, coc);
        }
        
        else
        {
            // return blurred if the CoC is outside a certain value
            return blurredTex;
        }
        
    }
    
    // don't enable the depth of field
    else
    {
        return nonBlurredTex;
    }
    
        
    
}