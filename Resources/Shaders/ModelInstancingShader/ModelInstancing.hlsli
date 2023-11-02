cbuffer DirectionLight : register(b0) {
	float3 ligDirection;
	float3 ligColor;
	float3 eyePos;

	float3 ptPos;
	float3 ptColor;
	float ptRange;
}

cbuffer Color : register(b1){
	float4 color;
}

struct VertexShaderOutput{
    float32_t4 position : POSITION0;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
};

struct GeometoryOutPut{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
};

struct ConstantHullShaderoutPut
{
  float Edges[3] : SV_TessFactor;
  float Inside : SV_InsideTessFactor;
};

struct HullShaderoutPut{
    float32_t4 position : SV_POSITION;
    float32_t3 normal : NORMAL;
};

struct DSoutPut{
    float32_t4 position : POSITION0;
    float32_t3 normal : NORMAL;
    float32_t4 worldPosition : POSITION1;
    float32_t2 uv : TEXCOORD;
};