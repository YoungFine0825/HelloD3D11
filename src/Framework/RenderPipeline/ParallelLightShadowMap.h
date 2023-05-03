#pragma once
#include "../../math/MathLib.h"
#include "../Collision/Collision.h"
#include "ShadowMap.h"
#include "ShaderStruct.h"
#include "Renderer.h"
#include "FrameData.h"

namespace Framework 
{
	class Camera;


	class ParallelLightShadowMap : public ShadowMap 
	{
	public:
		ParallelLightShadowMap();
		~ParallelLightShadowMap();
		//
		void Update(Camera* camera,RendererVector* visibleRenderers, RendererVector* allRenderers, XMFLOAT3 litRotationW);
		void PreRender(FrameData* frameData) override;
		void PostRender();
		void SetShaderParamters(Shader* shader) override;
		//
		ParallelLightShadowMap* SetNearestShadowDistance(float distance);
	private:
		struct CascadePart 
		{
			D3D11_VIEWPORT m_viewport;
			XMMATRIX m_viewProjectMatrix;
			AxisAlignedBox m_aabbW;
		};
		int m_cascadeCnt{2};
		CascadePart* m_cascadeParts;
		AxisAlignedBox m_wholeSceneAABBW;
		float m_depthMapWidth{0};
		float m_depthMapHeight{0};
		float m_nearestShadowDistance{300};
		//
		void OnCreateShadowMap(unsigned int size) override;
		//
		AxisAlignedBox ComputeFrustumAABB(float fov, float aspect, float nearDistance, float farDistance,XMMATRIX worldMatrix);
		void BuildViewProjectMartrix(XMFLOAT3 litRotationW, AxisAlignedBox* sceneAABBW, Camera* camera);
		void BuildCascadeParts(Camera* camera, RendererVector* visibleRenderers);
	};
}