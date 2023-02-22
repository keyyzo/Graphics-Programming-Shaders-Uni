// Vertical Blur Pixel Shader
// Uses a passed in texture
// and applies a vertical blur
// to it using a weighting system
// to control how the blur is affected

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

// The blur makes uses of the screen size buffer
// in order to figure out the texture height
// and correctly blur the image to the right size
cbuffer ScreenSizeBuffer : register(b0)
{
    float screenHeight;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float weight0, weight1, weight2, weight3, weight4, weight5, weight6;
    float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur.
    // The blur is using seven weights
    // weight0 being the main point of blurring
    // with the surrounding weights affecting the 
    // overall blur gradually on the outskirts of the blur
    
    // the weights all add up to 1.0f
    // with weight0 being used once
    // and the rest of the weights being used twice
    
    weight0 = 0.250000f;
    weight1 = 0.125000f;
    weight2 = 0.100000f;
    weight3 = 0.050000f;
    weight4 = 0.050000f;
    weight5 = 0.025000f;
    weight6 = 0.025000f;

    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 1.0f / screenHeight;
    // Add the vertical pixels to the colour by the specific weight of each.
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -6.0f)) * weight6;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -5.0f)) * weight5;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -4.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 1.0f)) * weight1;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 2.0f)) * weight2;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 3.0f)) * weight3;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 4.0f)) * weight4;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 5.0f)) * weight5;
    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 6.0f)) * weight6;
 
    // Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}

