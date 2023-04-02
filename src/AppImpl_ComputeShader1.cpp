/*
* Compute Shader 例子：向量相加
* 
*/
#include "Global.h"
#include "App.h"

#ifdef  AppImpl_ComputeShader1

#include "Window.h"
#include "math/MathLib.h"
#include "Color.h"

#include "Framework/Graphic.h"
#include "Framework/Shader/ShaderManager.h"
#include <vector>
#include <fstream>

using namespace Framework;

struct Data
{
	XMFLOAT3 v1;
	XMFLOAT2 v2;
};

UINT mNumElements = 32;

ID3D11ShaderResourceView* pInputASrv;
ID3D11ShaderResourceView* pInputBSrv;
ID3D11UnorderedAccessView* pOutputUav;
ID3D11Buffer* outputBuffer;
ID3D11Buffer* outputBufferCPUSide;

Shader* pComputeShader;

void App_PreCreateWindow() 
{

}

void Init() 
{
	ID3D11Device* device = d3dGraphic::GetDevice();
	//
	std::vector<Data> dataA(mNumElements);
	std::vector<Data> dataB(mNumElements);
	for (int i = 0; i < mNumElements; ++i)
	{
		dataA[i].v1 = XMFLOAT3(i, i, i);
		dataA[i].v2 = XMFLOAT2(i, 0);

		dataB[i].v1 = XMFLOAT3(-i, i, 0.0f);
		dataB[i].v2 = XMFLOAT2(0, -i);
	}
	//
	//create input buffer
	D3D11_BUFFER_DESC inputDesc;
	inputDesc.Usage = D3D11_USAGE_DEFAULT;
	inputDesc.ByteWidth = sizeof(Data) * mNumElements;
	inputDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	inputDesc.CPUAccessFlags = 0;
	inputDesc.StructureByteStride = sizeof(Data);
	inputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//
	D3D11_SUBRESOURCE_DATA vinitDataA;
	vinitDataA.pSysMem = &dataA[0];
	//
	ID3D11Buffer* bufferA = nullptr;
	DXHR(device->CreateBuffer(&inputDesc, &vinitDataA, &bufferA));
	//
	D3D11_SUBRESOURCE_DATA vinitDataB;
	vinitDataB.pSysMem = &dataB[0];
	//
	ID3D11Buffer* bufferB = nullptr;
	DXHR(device->CreateBuffer(&inputDesc, &vinitDataB, &bufferB));
	//
	//create output buffer
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_DEFAULT;
	outputDesc.ByteWidth = sizeof(Data) * mNumElements;
	outputDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	outputDesc.CPUAccessFlags = 0;
	outputDesc.StructureByteStride = sizeof(Data);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	DXHR(device->CreateBuffer(&outputDesc, nullptr, &outputBuffer));
	//
	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	DXHR(device->CreateBuffer(&outputDesc, nullptr, &outputBufferCPUSide));
	//
	//create srv
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = mNumElements;
	//
	device->CreateShaderResourceView(bufferA, &srvDesc, &pInputASrv);
	device->CreateShaderResourceView(bufferB, &srvDesc, &pInputBSrv);
	//
	//create uav
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = mNumElements;
	device->CreateUnorderedAccessView(outputBuffer, &uavDesc, &pOutputUav);
	//
	ReleaseCOM(bufferA);
	ReleaseCOM(bufferB);
	//

}

void DoComputeWork() 
{
	pComputeShader->SetShaderResourceView("g_InputA", pInputASrv);
	pComputeShader->SetShaderResourceView("g_InputB", pInputBSrv);
	pComputeShader->SetUnorderredAccessView("g_Output", pOutputUav);
	ID3D11DeviceContext* context = d3dGraphic::GetDeviceContext();
	//
	pComputeShader->ApplyPass(0, context);
	context->Dispatch(1, 1, 1);
	//
		// Unbind the input textures from the CS for good housekeeping.
	ID3D11ShaderResourceView* nullSRV[1] = { 0 };
	context->CSSetShaderResources(0, 1, nullSRV);

	// Unbind output from compute shader (we are going to use this output as an input in the next pass, 
	// and a resource cannot be both an output and input at the same time.
	ID3D11UnorderedAccessView* nullUAV[1] = { 0 };
	context->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);

	// Disable compute shader.
	context->CSSetShader(0, 0, 0);

	std::ofstream fout("compute_vec_add_result.txt");

	// Copy the output buffer to system memory.
	context->CopyResource(outputBufferCPUSide, outputBuffer);

	// Map the data for reading.
	D3D11_MAPPED_SUBRESOURCE mappedData;
	context->Map(outputBufferCPUSide, 0, D3D11_MAP_READ, 0, &mappedData);

	Data* dataView = reinterpret_cast<Data*>(mappedData.pData);

	for (int i = 0; i < mNumElements; ++i)
	{
		fout << "( v1 = " << dataView[i].v1.x << ", " << dataView[i].v1.y << ", " << dataView[i].v1.z <<
			", v2 = " << dataView[i].v2.x << ", " << dataView[i].v2.y << ")" << std::endl;
	}

	context->Unmap(outputBufferCPUSide, 0);

	fout.close();
}


bool App_Init() 
{
	if (!d3dGraphic::Init())
	{
		return false;
	}
	//
	pComputeShader = ShaderManager::LoadFromFxFile("res/effects/ComputeShader_VecAdd.fx");
	if (pComputeShader == nullptr) 
	{
		win_MsgBox(L"加载Shader失败！");
		return false;
	}
	//
	Init();
	//
	DoComputeWork();
	//
	return true;
}


void App_Tick(float dt)
{

}


void App_Draw() 
{
	Graphics::ClearBackground(Colors::Blue);
	Graphics::ClearDepthStencil();
	Graphics::Present();
}


void App_Cleanup()
{
	ReleaseCOM(pInputASrv);
	ReleaseCOM(pInputBSrv);
	ReleaseCOM(pOutputUav);
	ReleaseCOM(outputBuffer);
	ReleaseCOM(outputBufferCPUSide);
	ShaderManager::Cleanup();
	d3dGraphic::Shutdown();
}

#endif