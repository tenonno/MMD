struct VSINPUT {
	float4 position : POSITION;
	float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	row_major float4x4 worldMatrix : MATRIX;
	float4 diffuseColor : COLOR;
};

/*
varying vec3 vWorldPosition;
varying vec3 vSunDirection;
varying float vSunfade;
varying vec3 vBetaR;
varying vec3 vBetaM;
varying float vSunE;
*/


struct VSOUTPUT {
	float4 position : SV_POSITION;

	float3 vWorldPosition : TEXCOORD0;
	float3 vSunDirection : TEXCOORD1;
	float vSunfade : TEXCOORD2;
	float3 vBetaR : TEXCOORD3;
	float3 vBetaM : TEXCOORD4;
	float vSunE : TEXCOORD5;
};

cbuffer vscbMesh0 : register(b0)
{
	row_major float4x4 g_viewProjectionMatrix;
}


cbuffer vscbMesh1 : register(b1)
{
	float3 sunPosition;
	float rayleigh;
	float turbidity;
	float mieCoefficient;
}





static const float3 up = float3(0.0, 1.0, 0.0);
static const float e = 2.718281828459045;
static const float3 totalRayleigh = float3(0.000005804542996261093, 0.000013562911419845635, 0.000030265902468824876);
static const float3 K = float3(0.686, 0.678, 0.666);
static const float3 MieConst = float3(183999185144339.78, 277980239196605.28, 407904795438610.94);
static const float cutoffAngle = 1.6110731556870734;
static const float steepness = 1.5;
static const float EE = 1000.0;


float sunIntensity(float zenithAngleCos)
{
	zenithAngleCos = clamp(zenithAngleCos, -1.0, 1.0);
	return EE * max(0.0, 1.0 - pow(e, -((cutoffAngle - acos(zenithAngleCos)) / steepness)));
}



float3 totalMie(float T)
{
	float c = (0.2 * T) * 10E-18;
	return 0.434 * c * MieConst;
}





VSOUTPUT VS(in VSINPUT input)
{
	VSOUTPUT output;

	
	float4 posWS = mul(input.position, input.worldMatrix);

	output.vWorldPosition = posWS.xyz;


	output.vSunDirection = normalize(sunPosition);
	output.vSunE = sunIntensity(dot(output.vSunDirection, up));

	output.vSunfade = 1.0 - clamp(1.0 - exp((sunPosition.y / 450000.0)), 0.0, 1.0);
	float rayleighCoefficient = rayleigh - (1.0 * (1.0 - output.vSunfade));



	output.vBetaR = totalRayleigh * rayleighCoefficient;
	output.vBetaM = totalMie(turbidity) * mieCoefficient;

	output.position = mul(posWS, g_viewProjectionMatrix);
	return output;
}