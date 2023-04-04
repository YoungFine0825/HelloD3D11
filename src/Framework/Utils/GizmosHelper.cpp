
#include "../../Global.h"
#include "GizmosHelper.h"
#include "../Graphic.h"
#include "../Shader/Shader.h"
#include "../Shader/ShaderManager.h"

namespace Framework 
{
	namespace GizmosHelper
	{
		static d3dGraphic::VertexFormat_Base verts[8] =
		{
			{ {-1, -1, -1} },
			{ {1, -1, -1} },
			{ {1, -1, 1 } },
			{ {-1, -1, 1 } },
			{ {-1, 1, -1 } },
			{ {1, 1, -1} },
			{ {1, 1, 1} },
			{ {-1, 1, 1} }
		};
		static const UINT indices[] =
		{
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			0, 4,
			1, 5,
			2, 6,
			3, 7
		};

		d3dGraphic::VertexFormat_Base frustumLinePoints[24];

		Shader* m_gizmosShader;
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;
		ID3D11Buffer* m_pFrustumVertexBuffer;

		void Init() 
		{
			m_gizmosShader = ShaderManager::LoadFromFxFile("res/effects/Gizmos.fx");
			if (m_gizmosShader) 
			{
				m_gizmosShader->SetInputLayout(D3D_INPUT_DESC_BASE);
			}
			//
			d3dGraphic::CreateVertexBuffer(&m_pVertexBuffer, nullptr, sizeof(d3dGraphic::VertexFormat_Base), 8, D3D11_USAGE_DEFAULT);
			d3dGraphic::CreateIndexBuffer(&m_pIndexBuffer, nullptr, sizeof(UINT), 24, D3D11_USAGE_DEFAULT);
			d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pVertexBuffer, 0, nullptr, verts, 0, 0);
			d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pIndexBuffer, 0, nullptr, indices, 0, 0);
			//
			d3dGraphic::CreateVertexBuffer(&m_pFrustumVertexBuffer, nullptr, sizeof(d3dGraphic::VertexFormat_Base), 24, D3D11_USAGE_DEFAULT);
		}

		void Cleanup() 
		{
			ReleaseCOM(m_pVertexBuffer);
			ReleaseCOM(m_pIndexBuffer);
			ReleaseCOM(m_pFrustumVertexBuffer);
		}

		void DrawCube(XMMATRIX mvp, RGBA32 color) 
		{
			ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
			if (context == nullptr)
			{
				return;
			}
			m_gizmosShader->SetMatrix4x4("obj_MatMVP", mvp);
			m_gizmosShader->SetVector4("obj_Color", color);
			UINT stride = sizeof(d3dGraphic::VertexFormat_Base);
			UINT offset = 0;
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			context->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			context->IASetInputLayout(m_gizmosShader->GetInputLayout());
			m_gizmosShader->ApplyPass(0, context);
			context->DrawIndexed(24, 0, 0);
		}

		void DrawAABB(XNA::AxisAlignedBox localAABB, XMMATRIX mvp, RGBA32 color)
		{
			XMMATRIX scalingMat = XMMatrixScaling(localAABB.Extents.x, localAABB.Extents.y, localAABB.Extents.z);
			XMMATRIX translateMat = XMMatrixTranslationFromFloat3(localAABB.Center);
			XMMATRIX aabbMVP = (scalingMat * translateMat) * mvp;
			GizmosHelper::DrawCube(aabbMVP, color);
		}

		void DrawFrustum(XNA::Frustum frustum, XMMATRIX mvp, RGBA32 color) 
		{
			ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
			if (context == nullptr)
			{
				return;
			}
			XMVECTOR Origin = XMVectorSet(frustum.Origin.x, frustum.Origin.y, frustum.Origin.z, 0);
			FLOAT Near = frustum.Near;
			FLOAT Far = frustum.Far;
			FLOAT RightSlope = frustum.RightSlope;
			FLOAT LeftSlope = frustum.LeftSlope;
			FLOAT TopSlope = frustum.TopSlope;
			FLOAT BottomSlope = frustum.BottomSlope;

			XMFLOAT3 CornerPoints[8];
			CornerPoints[0] = XMFLOAT3(RightSlope * Near, TopSlope * Near, Near);
			CornerPoints[1] = XMFLOAT3(LeftSlope * Near, TopSlope * Near, Near);
			CornerPoints[2] = XMFLOAT3(LeftSlope * Near, BottomSlope * Near, Near);
			CornerPoints[3] = XMFLOAT3(RightSlope * Near, BottomSlope * Near, Near);

			CornerPoints[4] = XMFLOAT3(RightSlope * Far, TopSlope * Far, Far);
			CornerPoints[5] = XMFLOAT3(LeftSlope * Far, TopSlope * Far, Far);
			CornerPoints[6] = XMFLOAT3(LeftSlope * Far, BottomSlope * Far, Far);
			CornerPoints[7] = XMFLOAT3(RightSlope * Far, BottomSlope * Far, Far);

			XMVECTOR Orientation = XMLoadFloat4(&frustum.Orientation);
			XMMATRIX Mat = XMMatrixRotationQuaternion(Orientation);
			for (UINT i = 0; i < 8; i++)
			{
				XMVECTOR Result = XMVector3Transform(XMLoadFloat3(&CornerPoints[i]), Mat);
				Result = XMVectorAdd(Result, Origin);
				XMStoreFloat3(&CornerPoints[i], Result);
			}
			//
			frustumLinePoints[0].position = CornerPoints[0];
			frustumLinePoints[1].position = CornerPoints[1];
			frustumLinePoints[2].position = CornerPoints[1];
			frustumLinePoints[3].position = CornerPoints[2];
			frustumLinePoints[4].position = CornerPoints[2];
			frustumLinePoints[5].position = CornerPoints[3];
			frustumLinePoints[6].position = CornerPoints[3];
			frustumLinePoints[7].position = CornerPoints[0];
			//
			frustumLinePoints[8].position = CornerPoints[0];
			frustumLinePoints[9].position = CornerPoints[4];
			frustumLinePoints[10].position = CornerPoints[1];
			frustumLinePoints[11].position = CornerPoints[5];
			frustumLinePoints[12].position = CornerPoints[2];
			frustumLinePoints[13].position = CornerPoints[6];
			frustumLinePoints[14].position = CornerPoints[3];
			frustumLinePoints[15].position = CornerPoints[7];
			//
			frustumLinePoints[16].position = CornerPoints[4];
			frustumLinePoints[17].position = CornerPoints[5];
			frustumLinePoints[18].position = CornerPoints[5];
			frustumLinePoints[19].position = CornerPoints[6];
			frustumLinePoints[20].position = CornerPoints[6];
			frustumLinePoints[21].position = CornerPoints[7];
			frustumLinePoints[22].position = CornerPoints[7];
			frustumLinePoints[23].position = CornerPoints[4];
			//
			d3dGraphic::GetDeviceContext()->UpdateSubresource(m_pFrustumVertexBuffer, 0, nullptr, frustumLinePoints, 0, 0);
			//
			m_gizmosShader->SetMatrix4x4("obj_MatMVP", mvp);
			m_gizmosShader->SetVector4("obj_Color", color);
			UINT stride = sizeof(d3dGraphic::VertexFormat_Base);
			UINT offset = 0;
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			context->IASetVertexBuffers(0, 1, &m_pFrustumVertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
			context->IASetInputLayout(m_gizmosShader->GetInputLayout());
			m_gizmosShader->ApplyPass(0, context);
			context->Draw(24, 0);
		}
	}
}