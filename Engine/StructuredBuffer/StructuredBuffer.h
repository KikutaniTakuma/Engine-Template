#pragma once
#include "Engine/Engine.h"
#include <cassert>
#include <wrl.h>
#include "Engine/ErrorCheck/ErrorCheck.h"
//#include "Engine/ShaderResource/ShaderResourceHeap.h"

// ポインタをテンプレートパラメータに設定してはいけない
template<class T>
class StructuredBuffer {
	static_assert(!std::is_pointer<T>::value, "Do not use pointer types");

public:
	inline StructuredBuffer() = delete;

	inline StructuredBuffer(uint32_t instanceNum) noexcept :
		bufferResource(),
		srvDesc(),
		data(nullptr),
		isWright(true),
		isCreateView(false),
		range(),
		roootParamater(),
		kInstanseNum(instanceNum)
	{
		// バイトサイズは256アライメントする(vramを効率的に使うための仕組み)
		bufferResource = Engine::CreateBufferResuorce(sizeof(T) * kInstanseNum);
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Buffer.NumElements = UINT(kInstanseNum);
		srvDesc.Buffer.StructureByteStride = sizeof(T);

		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}

		range = {};
		range.BaseShaderRegister = 1;
		range.NumDescriptors = 1;
		range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


		roootParamater = {};
		roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		roootParamater.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		roootParamater.DescriptorTable.pDescriptorRanges = &range;
		roootParamater.DescriptorTable.NumDescriptorRanges = 1;
	}

	inline ~StructuredBuffer() noexcept {
		bufferResource->Release();
	}

	inline StructuredBuffer(const StructuredBuffer& right) noexcept :
		bufferResource(),
		srvDesc(),
		data(nullptr),
		isWright(true),
		roootParamater(),
		shaderVisibility(D3D12_SHADER_VISIBILITY_ALL),
		shaderRegister(0)
	{
		*this = right;
	}

	inline StructuredBuffer<T>& operator=(const StructuredBuffer& right) {

		if (bufferResource) {
			bufferResource->Release();
			bufferResource.Reset();
		}
		bufferResource = Engine::CreateBufferResuorce((sizeof(T) + 0xff) & ~0xff);
		srvDesc.BufferLocation = bufferResource->GetGPUVirtualAddress();
		srvDesc.SizeInBytes = UINT(bufferResource->GetDesc().Width);

		if (isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
		}
		roootParamater = right.roootParamater;

		*data = *right.data;

		if (!isCreateView) {
			assert(!"created view");
			ErrorCheck::GetInstance()->ErrorTextBox("operator= Created view fail", "Const Buffer");
		}

		return *this;
	}

public:
	void OnWright() noexcept {
		if (!isWright) {
			bufferResource->Map(0, nullptr, reinterpret_cast<void**>(&data));
			isWright = !isWright;
		}
	}

	void OffWright() noexcept {
		if (isWright) {
			bufferResource->Unmap(0, nullptr);
			isWright = !isWright;
		}
	}

	T& operator[](uint32_t index) {
		assert(kInstanseNum < index);
		return data[index];
	}

	const T& operator[](uint32_t index) const {
		assert(kInstanseNum < index);
		return data[index];
	}

	uint32_t Size() const {
		return kInstanseNum;
	}

	D3D12_GPU_VIRTUAL_ADDRESS GetGPUVtlAdrs() const noexcept {
		return bufferResource->GetGPUVirtualAddress();
	}

	const D3D12_ROOT_PARAMETER& GetRoootParamater() noexcept {
		return roootParamater;
	}

	void CrerateView(D3D12_CPU_DESCRIPTOR_HANDLE descriptorHandle) noexcept {
		Engine::GetDevice()->CreateConstantBufferView(&srvDesc, descriptorHandle);
		isCreateView = true;
	}

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> bufferResource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D12_DESCRIPTOR_RANGE range;

	T* data;

	bool isWright;

	bool isCreateView;

	D3D12_ROOT_PARAMETER roootParamater;

	const uint32_t kInstanseNum;
};