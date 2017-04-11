


#include <Siv3D.hpp>


#include "MMD.hpp"

#include "Sky.hpp"
#include <numeric>

#include "Player.hpp"


#include "PhysX.hpp"
#include "PhysX_Render.hpp"



void Main()
{
	Window::SetStyle(WindowStyle::Sizeable);

	// Window::Resize(1280, 720);


	MMD::LoadToonTexture();

	// initPhysX();

	PhysX pX;

	PxRigidDynamic *ac = nullptr;


	auto playerActor = pX.create<PxRigidDynamic>(PxTransform(PxVec3(0, 10.f, 0)));
	{


		auto m = m_physics->createMaterial(0.6, 0.6, 0);

		// 反発係数
		m->setRestitution(0.0);

		m->setFrictionCombineMode(PxCombineMode::eMIN);
		m->setRestitutionCombineMode(PxCombineMode::eMIN);

		// PxCombineMode::
		
		// m->userData

		float radius = 0.3f;
		float height = 1.5f;

		height -= radius * 2.0;
		height /= 2.f;

		auto capsuleShape = playerActor->createShape(PxCapsuleGeometry(radius, height), *m);
		
		PxRigidBodyExt::updateMassAndInertia(*playerActor, 10.1f);
		
		playerActor->attachShape(*capsuleShape);
		capsuleShape->release();
	}
	pX.getScene().addActor(*playerActor);





	// capsuleShape->release();



	for (auto i : step(10))
	{

		auto pos = RandomVec2(10.0);

		auto vv = PxVec3(0.0f, 0.0f, -1000.0f);


		PxTransform pose(ToPxVec3(Vec3(pos.x, 10, pos.y)), PxQuat(PxIdentity));


		auto actor = m_physics->createRigidDynamic(pose);


		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.f, 0.f, 1.f)));

		auto m = m_physics->createMaterial(0, 0, 0);



		PxShape *capsuleShape = actor->createShape(PxCapsuleGeometry(1.f, 2.f), *m);



		// actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);


		capsuleShape->setLocalPose(relativePose);
		// 質量と慣性を計算する
		PxRigidBodyExt::updateMassAndInertia(*actor, 10.f);


		actor->attachShape(*capsuleShape);


		// capsuleShape->release();


		// actor->setAngularDamping(0.5f);
		// actor->setLinearVelocity(vv);


		ac = actor;

		pX.getScene().addActor(*actor);


	}














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

	player.previousPos = player.position;

	player.setHeight(1.5);

	model2.scale *= 0.7;

	// フォワードレンダリングのライトを消す
	Graphics3D::SetLightForward(0, Light::None());


	Sky sky;


	bool autoPlay = false;

	auto tts = std::move(model2.getT3D());

	auto frame = 0.0;

	double cameraDistance = 5.0;
	double cameraAngle = 0.0;


	VertexShader boader_vs(L"Assets/Shader/n_vs.hlsl");
	PixelShader boader_ps(L"Assets/Shader/n_ps.hlsl");


	VertexShader mmd_vs(L"Assets/Shader/mmd_vs.hlsl");
	PixelShader mmd_ps(L"Assets/Shader/mmd_ps.hlsl");



	{


		auto tts2 = std::move(model2.getT3D());

		Array<Vec3> ttsV;
		ttsV.reserve((tts2.size() * 3));
		for (auto &i : tts2)
		{
			ttsV.emplace_back(ToVec3(i.p0));
			ttsV.emplace_back(ToVec3(i.p1));
			ttsV.emplace_back(ToVec3(i.p2));
		}

		Array<uint32> ttsI(ttsV.size());
		for (auto i : step(ttsV.size()))
		{
			ttsI[i] = i;
		}




		auto p = pX.aaa(ttsV, ttsI);

		auto actor = m_physics->createRigidStatic(PxTransform(PxVec3(PxZero)));



		PxShape* shape = actor->createShape(PxTriangleMeshGeometry(p), pX.getMaterial()); //Here it all crash
		PX_ASSERT(shape);

		PxTransform relativePose(PxQuat(PxHalfPi, PxVec3(0.f, 0.f, 0.f)));


		pX.m_scene->addActor(*actor);

	}

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

		Ray plRay(plPos + plLn / 2.0, -plLn * 2);



		player.update(cameraAngle);



		Array<double> hits;

		/*
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


			auto r = plRay.intersectsAt(triangle);


			if (r.has_value() && !IsNaN(r.value().y))
			{

				hits.emplace_back(r.value().y);

			}


		}
		*/



		if (!hits.empty())
		{

			std::sort(hits.begin(), hits.end(), [&](auto a, auto b)
			{
				return a > b;
			});

			$(hits[0]);


			player.position.y = hits[0];

		}


		// プレイヤーの物理演算
		{

			auto ppp = (1.5 / 2.0) - 0.3;

			PxTransform transform;

			transform.p = ToPxVec3(player.position + Vec3(0, ppp, 0));
			transform.q = PxQuat(HalfPi, PxVec3(0, 0, 1.f));


			playerActor->setGlobalPose(transform);




			auto v = ToVec3(playerActor->getLinearVelocity());

			v.x = 0.f;
			v.z = 0.f;

			playerActor->setLinearVelocity(ToPxVec3(v));




			pX.update();

			player.position.x = playerActor->getGlobalPose().p.x;
			player.position.y = playerActor->getGlobalPose().p.y - ppp;
			player.position.z = playerActor->getGlobalPose().p.z;

			player.previousPos = player.position;


			/*

			// playerActor->setGlobalPose(PxTransform(ToPxVec3(player.position)));

			// playerActor->addForce(PxVec3(1.1f, 0, 0), PxForceMode::eFORCE);

			auto pPose = playerActor->getGlobalPose();

			pPose.q = PxQuat(HalfPi, PxVec3(0, 0, 1.f));

			playerActor->setGlobalPose(pPose);



			auto v = ToVec3(playerActor->getLinearVelocity());
			Log(Format(L"Velocity: ", v), font1);


			v += player.position - player.previousPos;

			// playerActor->setLinearVelocity(ToPxVec3(v));//ToPxVec3(player.position - player.previousPos), false);

			playerActor->addForce(ToPxVec3(player.position - player.previousPos), PxForceMode::eVELOCITY_CHANGE);

			Log(Format(L"aaaaa: ", player.position - player.previousPos), font1);

			pX.update();


			player.position.x = playerActor->getGlobalPose().p.x;
			player.position.y = playerActor->getGlobalPose().p.y - (1.5 /2.0) - 0.3;
			player.position.z = playerActor->getGlobalPose().p.z;

			player.previousPos = player.position;

			*/


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




		for (auto &triangle : tts)
		{
			auto v1 = ToVec3(triangle.p0);
			auto v2 = ToVec3(triangle.p1);
			auto v3 = ToVec3(triangle.p2);

			// 全ての頂点が範囲外なら無視
			if (v1.distanceFrom(player.position) > 20.0 &&
				v2.distanceFrom(player.position) > 20.0 &&
				v3.distanceFrom(player.position) > 20.0
				) continue;



			auto normal = CreatePolygonNormal(v1, v2, v3);



			ColorF color = Palette::Green;

			color.r = Abs(Vec3::Up.dot(normal));
			color.g = Abs(Vec3::Up.dot(normal));
			color.b = Abs(Vec3::Up.dot(normal));

			// color.a = 0.5;


			auto s1 = Graphics3D::ToScreenPos(v1);
			auto s2 = Graphics3D::ToScreenPos(v2);
			auto s3 = Graphics3D::ToScreenPos(v3);


			if (s1.z > 1.0) continue;
			if (s2.z > 1.0) continue;
			if (s3.z > 1.0) continue;
			if (s1.z < 0.0) continue;
			if (s2.z < 0.0) continue;
			if (s3.z < 0.0) continue;

			/*

			Triangle(
				Graphics3D::ToScreenPos(v1).xy(),
				Graphics3D::ToScreenPos(v2).xy(),
				Graphics3D::ToScreenPos(v3).xy()
			).draw(color);

			*/

		}





		player.drawShadow();
		model2.drawShadow();


		player.drawBoader(boader_vs, boader_ps);


		// player.drawMMD(mmd_vs, mmd_ps);
		player.drawForward();

		// player.drawForward();
		model2.draw();

		// player.drawBone();
		Sphere(player.position, 0.3).draw(Palette::Red);






		const auto dynColor = Palette::Green;
		const auto kinematicColor = Palette::Lightgreen;

		PxU32 actorsCount = pX.getScene().getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC);

		if (actorsCount)
		{


			Array<PxRigidActor *> actors(actorsCount);

			pX.getScene().getActors(PxActorTypeFlag::eRIGID_DYNAMIC,
				reinterpret_cast<PxActor **>(actors.data()),
				actorsCount);


			for (auto &actor : actors)
			{


				PxRigidDynamic* dyn = actor->is<PxRigidDynamic>();
				if (dyn->getRigidBodyFlags() & PxRigidBodyFlag::eKINEMATIC)
				{

					renderActors(&actor, 1, true, kinematicColor);
				}
				else
				{
					renderActors(&actor, 1, true, dynColor);
				}
			}
		}




	}
}
