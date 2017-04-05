


#include <Siv3D.hpp>


#include "MMD.hpp"

#include "Sky.hpp"

void Main()
{

	Window::Resize(1280, 720);

	FilePath path = L"Assets/reim/reim.pmx";


	path = L"C:/Users/tis-teno/Desktop/サーバルちゃんver1.01/サーバルちゃんver1.01/サーバルちゃんver1.01.pmx";

	// path = L"C:/Users/tis-teno/Desktop/サーバルちゃんver1.01/サーバルちゃんver1.01/付与なし.pmx";



	// Motion motion(L"Assets/reim/motion.vmd");

	Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/サーバルメトロノーム.vmd");
	// Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/test.vmd");

	// Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/t2.vmd");

	// Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/IK.vmd");
	// Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/IK test.vmd");







	Font font1(14, L"メイリオ");
	Font font2(8, L"メイリオ");



	double logPos = 0.0;
	const auto Log = [&](const String &text, const Font &font)
	{
		font.draw(text, 1000, logPos);
		logPos += font.height + 2.0;
	};



	Graphics::SetBackground(ColorF(0.3));


	auto frame = 10.0;

	const auto timeSpeed = 0.508;

	double cameraDistance = -30.0;





	GUI gui(GUIStyle::Default);

	gui.setPos(150, 50);

	const Texture textureGround(L"Assets/ground.png", TextureDesc::For3D);



	Graphics3D::SetAmbientLight(ColorF(0.4));
	Graphics3D::SetAmbientLightForward(ColorF(1.0));


	using namespace PMX;


	PMX::Model model2(L"Assets/空色町1.52/sssorairo1.52.pmx");


	PMX::Model model(path);


	model.rotate.y = -Pi / 2.0;
	model.scale *= 0.2;

	model.translate.x -= 13.0;
	model.translate.z += 25.0;
	model.translate.y += 0.5;




	const auto defaultLight = Graphics3D::GetLight(0);
	// Graphics3D::SetLight(0, Light::None());
	Graphics3D::SetLightForward(0, Light::None());

	gui.add(L"play", GUIButton::Create(L"▶"));
	gui.add(L"freeCamera", GUIToggleSwitch::Create(L"FC", L"FC", true));

	gui.hide();


	bool autoPlay = false;


	auto &&tts = model2.getT3D();


	Sky sky;

	// Triangle3D

	while (System::Update())
	{
		logPos = 0.0;

		if (gui.button(L"play").pressed)
		{
			autoPlay = true;
		}

		if (autoPlay)
		{
			frame += 0.5;
		}


		Camera camera;

		camera.pos = Vec3(0, 10.0, cameraDistance);
		camera.lookat = model.translate;


		camera.pos = Vec3(-cameraDistance / 2, 10.0, cameraDistance / 2);


		model.drawShadow();
		model2.drawShadow();

		model.drawForward();
		model2.draw();

		// camera.fovDegree = 30;

		Graphics3D::SetCamera(camera);

		// if (gui.toggleSwitch(L"freeCamera").isRight) Graphics3D::FreeCamera();

		// const auto time = System::FrameCount() / 60.0;

		if (Input::Key1.pressed) frame -= timeSpeed;
		if (Input::Key2.pressed) frame += timeSpeed;

		cameraDistance -= Mouse::Wheel();


		frame = Max(0.0, frame);

		Log(Format(L"サーバルちゃんver1.01.pmx", frame), font1);
		Log(Format(L"空色町1.52.pmx"), font1);
		
		motion.apply(model, frame);



		// model.drawBone();


		sky.draw();

		// Plane(100).draw(textureGround);

		const Ray mouseRay = Mouse::Ray();

		Sphere(model.translate, 1.0).draw(Palette::Red);

		for (auto &tt : tts)
		{
			auto v = mouseRay.intersectsAt(tt);

			if (v.has_value())
			{

				auto c = RandomColor();

				Sphere(ToVec3(tt.p0), 1.0).draw(c);
				Sphere(ToVec3(tt.p1), 1.0).draw(c);


				Sphere(v.value(), 1.0).draw(c);

				Sphere(ToVec3(tt.p2), 1.0).draw(c);



			}

		}


	}
}
