#pragma once

#include <d3d11.h>
#include "Shader.h"

class Shaders
{
private:
	Shaders() {}
	~Shaders() {}

private:
	Shader skinnedMeshShader;
	Shader destructionShader;

	Shader spriteShader;
	Shader grassShader;
	Shader rampShader;
	Shader skyboxShader;
	Shader shadowmapShader;
	Shader outlineShader;
	Shader fullscreenQuadShader;	
	Shader blurShader;	
	Shader luminanceShader;	

	Shader uvscrollShader;
	
public:
	static Shaders* Ins() {
		static Shaders instance;
		return &instance;
	}

public:
	void Init(ID3D11Device* device);
	Shader GetSkinnedMeshShader() { return skinnedMeshShader; }
	Shader GetDestructionShader() { return destructionShader; }
	Shader GetSpriteShader() { return spriteShader; }
	Shader GetGrassShader() { return grassShader; }
	Shader GetRampShader() { return rampShader; }
	Shader GetSkyboxShader() { return skyboxShader; }
	Shader GetShadowmapShader() { return shadowmapShader; }
	Shader GetOutlineShader() { return outlineShader; }
	Shader GetFullscreenQuadShader() { return fullscreenQuadShader; }
	Shader GetBlurShader() { return blurShader; }
	Shader GetLuminanceShader() { return luminanceShader; }	

	Shader GetUvScrollShader() { return uvscrollShader; }
};
