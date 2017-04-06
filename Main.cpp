


#include <Siv3D.hpp>


#include "MMD.hpp"

#include "Sky.hpp"

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


	// https://bowlroll.net/file/131363
	PMX::Model player(L"Assets/Model/サーバルちゃんver1.01/サーバルちゃんver1.01.pmx");

	// https://bowlroll.net/file/10522
	PMX::Model model2(L"Assets/Model/空色町1.52/sssorairo1.52.pmx");

	// https://bowlroll.net/file/130173
	VMD::Motion motion(L"Assets/Motion/Rick式サーバルメトロノーム/サーバルメトロノーム.vmd");


	player.rotate.y = -Pi / 2.0;
	player.position.x -= 13.0;
	player.position.z += 25.0;
	player.setHeight(1.5);

	model2.scale *= 0.7;




	// フォワードレンダリングのライトを消す
	Graphics3D::SetLightForward(0, Light::None());


	Sky sky;


	bool autoPlay = false;

	auto &&tts = model2.getT3D();

	auto frame = 0.0;
	
	double cameraDistance = 5.0;
	double cameraAngle = 0.0;

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
		cameraDistance = Clamp(cameraDistance, 0.5, 10.0);


		Camera camera;

		camera.lookat = player.position + Vec3(0, 1, 0);
		camera.pos = player.position + Vec3(
			Sin(cameraAngle) * cameraDistance,
			1.0,
			Cos(cameraAngle) * cameraDistance);

		Graphics3D::SetCamera(camera);
		if (gui.toggleSwitch(L"freeCamera").isRight) Graphics3D::FreeCamera();




		player.drawShadow();
		model2.drawShadow();

		player.drawForward();
		model2.draw();

		// player.drawBone();



		// 1, 2 キーで時間を操作する
		const auto timeSpeed = 0.5;
		if (Input::Key1.pressed) frame -= timeSpeed;
		if (Input::Key2.pressed) frame += timeSpeed;
		frame = Max(0.0, frame);

		// モーションを適用する
		motion.apply(player, frame);



		sky.draw();
	

	}
}
