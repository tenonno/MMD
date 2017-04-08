struct VS_INPUT {
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	row_major float4x4 worldMatrix : MATRIX;
	float4 diffuseColor : COLOR;
};



struct VSOUTPUT {
	float4 position : SV_POSITION;


	float3 normal : TEXCOORD0;

};

cbuffer vscbMesh0 : register(b0)
{
	row_major float4x4 g_viewProjectionMatrix;
}





VSOUTPUT VS(in VS_INPUT input)
{
	VSOUTPUT output;

	float4 pos = input.position;
	
	
	pos.xyz += normalize(input.normal) * 0.03;


	float4 posWS = mul(pos, input.worldMatrix);

	output.normal = input.normal;


	output.position = mul(posWS, g_viewProjectionMatrix);
	return output;
}