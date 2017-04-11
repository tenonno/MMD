
struct VS_OUTPUT {
	float4 position : SV_POSITION;

	float2 tex : TEXCOORD0;
	float3 normal : TEXCOORD1;
};
SamplerState sampler0 : register(s0);

Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);


cbuffer pscbMesh0 : register(b1)
{
	float3 lightDirection;
	float useToon;
}

float4 PS(in VS_OUTPUT input) : SV_Target
{

	float4 result;


	result = texture0.Sample(sampler0, input.tex);
	


	// useToon == true
	if (useToon > 0.5)
	{

		//�n�[�t�����o�[�g�g�U�Ɩ��ɂ�郉�C�e�B���O�v�Z
		float p = dot(input.normal, lightDirection.xyz);
		p = p * 0.5 + 0.5;
		p = p * p;
		//�F�����e�N�Z���̂t�����Ƃ��A�g�D�[���}�b�v�e�N�X�`���[������̔��˗����擾����
		float4 Col = texture1.Sample(sampler0, float2(0.0, p));
		result *= Col;

	}

	
	// result.rgb = lightDirection;


	result.a = 1.0;


	//result.xy = input.tex;

	// result.rgb = input.normal;
	
	return result;
}





