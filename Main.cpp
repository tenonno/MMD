


#include <Siv3D.hpp>


#include "MMD.hpp"

#include "Sky.hpp"
#include <numeric>

#include "Player.hpp"


void Main()
{

	Window::Resize(1280, 720);



	Font font1(14, L"メイリオ");
	Font font2(8, L"メイリオ");
	double logPos = 0.0;
	const auto Log = [&](const String &text, const Font &font)
	{
		font.draw(text, 1000, logPos);
		logPos += font.height + 2.0;
	};




	GUI gui(GUIStyle::Default);
	gui.setPos(150, 50);
	gui.add(L"play", GUIButton::Create(L"▶"));
	gui.add(L"freeCamera", GUIToggleSwitch::Create(L"Free Camera", L"Free Camera", false));



	const Texture textureGround(L"Assets/ground.png", TextureDesc::For3D);


	Graphics::SetBackground(ColorF(0.3));
	Graphics3D::SetAmbientLight(ColorF(0.4));
	Graphics3D::SetAmbientLightForward(ColorF(1.0));



	// https://bowlroll.net/file/10522
	// https://bowlroll.net/file/130173
	// https://bowlroll.net/file/131363



	PMX::Model model2(L"Assets/Model/空色町1.52/sssorairo1.52.pmx");

	VMD::Motion motion(L"Assets/Motion/Rick式サーバルメトロノーム/サーバルメトロノーム.vmd");



	Player player(L"Assets/Model/サーバルちゃんver1.01/サーバルちゃんver1.01.pmx");


	player.position.x -= 13.0;
	player.position.z += 25.0;
	player.setHeight(1.5);

	model2.scale *= 0.5;


	// フォワードレンダリングのライトを消す
	Graphics3D::SetLightForward(0, Light::None());


	Sky sky;


	bool autoPlay = false;

	auto &&tts = model2.getT3D();

	auto frame = 0.0;
	
	double cameraDistance = 5.0;
	double cameraAngle = 0.0;


	VertexShader boader_vs(L"Assets/Shader/n_vs.hlsl");
	PixelShader boader_ps(L"Assets/Shader/n_ps.hlsl");


	while (System::Update())
	{
		logPos = 0.0;

		//
		if (gui.button(L"play").pressed) autoPlay = true;
		if (autoPlay) frame += 0.5;


		if (Input::MouseL.pressed)
		{
			cameraAngle += Mouse::Delta().x * 0.01;
		}
		cameraDistance += Mouse::Wheel() * 0.5;
		cameraDistance = Clamp(cameraDistance, 0.5, 100.0);


		// 1, 2 キーで時間を操作する
		const auto timeSpeed = 0.5;
		if (Input::Key1.pressed) frame -= timeSpeed;
		if (Input::Key2.pressed) frame += timeSpeed;
		frame = Max(0.0, frame);

		// モーションを適用する
		motion.apply(player, frame);



		auto plPos = player.getBonePos(L"全ての親");

		auto plLn = Vec3(0, 1.5, 0);

		Ray plRay(plPos + plLn,  -plLn * 2);



		player.update(cameraAngle);



		Array<double> hits;

		for (auto &triangle : tts)
		{

			auto v1 = ToVec3(triangle.p0);
			auto v2 = ToVec3(triangle.p1);
			auto v3 = ToVec3(triangle.p2);

			// 全ての頂点が範囲外なら無視
			if (
				v1.distanceFrom(player.position) > 20.0
				&&
				v2.distanceFrom(player.position) > 20.0
				&&
				v3.distanceFrom(player.position) > 20.0
				) continue;


			Line3D(v1, v2).drawForward(Palette::White);
			Line3D(v2, v3).drawForward(Palette::White);
			Line3D(v3, v1).drawForward(Palette::White);



			if (auto r = plRay.intersectsAt(triangle))
			{

				hits.emplace_back(r.value().y);

			}


		}


		if (!hits.empty())
		{

			std::sort(hits.begin(), hits.end(), [&](auto a, auto b)
			{
				return a > b;
			});

		
			// player.position.y = hits[0];

		}


		Camera camera;

		camera.lookat = player.position + Vec3(0, 1, 0);
		camera.pos = player.position + Vec3(
			Sin(cameraAngle) * cameraDistance,
			1.0 + cameraDistance * 0.3,
			Cos(cameraAngle) * cameraDistance);

		Graphics3D::SetCamera(camera);
		if (gui.toggleSwitch(L"freeCamera").isRight) Graphics3D::FreeCamera();


		Vec3 skyPos = Vec3(player.position);
		skyPos.y = 0.0;
		sky.position = skyPos;
		sky.draw();
	

		player.drawShadow();
		model2.drawShadow();


		player.drawBoader(boader_vs, boader_ps);


		player.drawForward();
		model2.draw();

		// player.drawBone();
		Sphere(player.position, 0.3).draw(Palette::Red);

		
		Log(Format(player.position), font1);
	}
}
