// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "HeightMapShader.h"
#include "TessellatedHeightMap.h"
#include "WaterShader.h"
#include "TessellatedOrb.h"
#include "OrbShader.h"
#include "DepthShader.h"
#include "TextureShader.h"
#include "WaterDepthShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "DepthOfFieldShader.h"
#include "GenericDepthShader.h"
#include "GeometryOrbShader.h"
#include "OrbParticles.h"
#include "ShadowShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

	// render passes

	// depth
	void directionalDepthPass();
	void pointDepthPass();
	void spotlightDepthPass();
	void cameraDepthPass();

	// geometry
	void geometryPass();

	// blurs
	void downSamplePass();
	void horizontalblurPass();
	void verticalblurPass();
	void upSamplePass();

	// post process
	void depthOfFieldPass();
	
	// final pass - rendering everything together
	void finalPass();
	void finalPassTest();

private:

	// declare objects
	TessellatedHeightMap* heightMap;
	TessellatedHeightMap* waterPlane;
	PlaneMesh* rightPlane;
	PlaneMesh* leftPlane;
	PlaneMesh* backPlane;
	PlaneMesh* frontPlane;
	TessellatedOrb* orb;
	TessellatedOrb* lavaOrb;
	OrbParticles* orbParticles;


	// declare shaders
	HeightMapShader* heightMapShader;
	WaterShader* waterShader;
	OrbShader* orbShader;
	DepthShader* depthShader;
	TextureShader* textureShader;
	WaterDepthShader* waterDepthShader;
	HorizontalBlurShader* horizontalBlurShader;
	VerticalBlurShader* verticalBlurShader;
	DepthOfFieldShader* depthOfFieldShader;
	GenericDepthShader* genericDepthShader;
	GeometryOrbShader* geometryOrbShader;
	ShadowShader* shadowShader;

	// declare ortho meshes
	OrthoMesh* directionalMapOrtho;
	OrthoMesh* pointMapOrthoXPositive;
	OrthoMesh* pointMapOrthoXNegative;
	OrthoMesh* pointMapOrthoYPositive;
	OrthoMesh* pointMapOrthoYNegative;
	OrthoMesh* pointMapOrthoZPositive;
	OrthoMesh* pointMapOrthoZNegative;
	OrthoMesh* spotlightOrtho;
	OrthoMesh* cameraDepthOrtho;
	OrthoMesh* finalSceneOrtho;

	bool directionalMapOrthoActive;
	bool pointXPosMapOrthoActive;
	bool pointXNegMapOrthoActive;
	bool pointYPosMapOrthoActive;
	bool pointYNegMapOrthoActive;
	bool pointZPosMapOrthoActive;
	bool pointZNegMapOrthoActive;
	bool spotlightMapOrthoActive;
	bool cameraMapOrthoActive;

	// declare render textures
	RenderTexture* nonBlurredTexture;
	RenderTexture* downSampleTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* upSampleTexture;
	RenderTexture* depthOfFieldTexture;

	// declare lights - may refactor into own class
	Light* directionalLight;
	Light* orbLight;
	Light* spotLight;

	// declare shadow maps
	ShadowMap* directionalMap;
	ShadowMap* pointMap[6];
	ShadowMap* spotlightMap;
	ShadowMap* cameraDepth;
	

	// light controller variables

	// directional light
	XMFLOAT3 directionalLightDirVals;
	XMFLOAT3 directionalLightPosVals;
	bool directionalLightControls;
	
	// orb point light
	XMFLOAT3 orbLightPosition;
	XMFLOAT3 orbAttenVals;
	XMFLOAT3 orbDirections[6];
	ID3D11ShaderResourceView* orbShadowTextures[6];
	XMMATRIX pointViewMatrices[6];
	XMMATRIX pointProjectionMatrices[6];
	bool pointLightControls;

	// spotlight 

	XMFLOAT3 spotlightPosition;
	XMFLOAT3 spotlightDirection;
	XMFLOAT3 spotlightAttenVals;
	float spotlightCone;
	float spotlightRange;
	bool spotLightControls;

	// Tessellation Value variables
	int tessellationValue;
	bool tessellationControls;

	// Height Map variables
	float heightMapDisplacementVal;
	bool heightMapControls;

	// Water variables
	float waterTime;
	float waterAmplitude;
	float waterFrequency;
	float waterSpeed;
	float waterPosY;
	bool waterControls;

	// Orb geometry variables 

	bool renderOrbVertexNormals;
	bool renderOrbFaceNormals;
	float vertexNormalSize, faceNormalSize;
	float vertexNormalActive, faceNormalActive;
	
	// blur depth variables
	float nearPlane, farPlane, aperture, focalLength, planeInFocus, blurEnabled;
	bool blurEnabledBool;


};

#endif