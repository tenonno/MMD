#pragma once

struct alignas(16) VS_SKY
{
	Float3 sunPosition;
	float rayleigh;

	float turbidity;
	float mieCoefficient;
};

struct alignas(16) PS_SKY
{
	Float3 cameraPos;
	float luminance;

	float mieDirectionalG;
};



class Sky
{

public:

	ConstantBuffer<VS_SKY> vs_cb_1;
	ConstantBuffer<PS_SKY> ps_cb_1;

	VertexShader vs;
	PixelShader ps;

	Vec3 position;

	Sky() :
		position(Vec3::Zero)
	{
		vs = VertexShader(L"Assets/Shader/sky_vs.hlsl");
		ps = PixelShader(L"Assets/Shader/sky_ps.hlsl");
	}



	void draw()
	{

		vs_cb_1->rayleigh = 2.0;
		vs_cb_1->turbidity = 0.5;
		vs_cb_1->mieCoefficient = 0.005;
		vs_cb_1->sunPosition = Vec3(-98.83775220505791, 20, 14.598319389250124).normalized();

		ps_cb_1->cameraPos = Vec3::Zero;
		ps_cb_1->luminance = 0.6;
		ps_cb_1->mieDirectionalG = 0.8;


		Graphics3D::BeginVSForward(vs);
		Graphics3D::BeginPSForward(ps);


		Graphics3D::SetConstantForward(ShaderStage::Vertex, 1, vs_cb_1);
		Graphics3D::SetConstantForward(ShaderStage::Pixel, 1, ps_cb_1);

		Graphics3D::SetRasterizerStateForward(RasterizerState(FillMode::Solid, CullMode::Front));

		Sphere(position, 300.0).drawForward(Palette::Gray);

		Graphics3D::SetRasterizerStateForward(RasterizerState::Predefined::Default3D);

		Graphics3D::EndVSForward();
		Graphics3D::EndPSForward();
	}





};