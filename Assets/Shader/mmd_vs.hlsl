struct VS_INPUT {
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	row_major float4x4 worldMatrix : MATRIX;
	float4 diffuseColor : COLOR;
};



struct VSOUTPUT {
	float4 position : SV_POSITION;

	float2 tex : TEXCOORD0;
	float3 normal : TEXCOORD1;

};

cbuffer vscbMesh0 : register(b0)
{
	row_major float4x4 g_viewProjectionMatrix;
}





VSOUTPUT VS(in VS_INPUT input)
{
	VSOUTPUT output;

	float4 pos = input.position;
	
	

	float4 posWS = mul(pos, input.worldMatrix);

	output.normal = input.normal;


	output.position = mul(posWS, g_viewProjectionMatrix);

	output.tex = input.tex;

	return output;
}