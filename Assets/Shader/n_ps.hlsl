
struct VS_OUTPUT {
	float4 position : SV_POSITION;

	float3 normal : TEXCOORD0;
};

float4 PS(in VS_OUTPUT input) : SV_Target
{

	float4 result;


	result.rgb = input.normal;
	result.a = 1.0;


	result.rgb = 0.0;


	return result;
}





