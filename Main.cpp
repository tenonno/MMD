


#include <Siv3D.hpp>


#include "MMD.hpp"


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

	


	Font font1(14, L"Consolas");
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



	const Texture textureTerrain(L"Example/Grass.jpg", TextureDesc::For3D);


	const Texture textureGround(L"Assets/ground.png", TextureDesc::For3D);



	Graphics3D::SetAmbientLight(ColorF(1.0));


	using namespace PMX;


	PMX::Model model(path);



	auto &bones = model.bones;
	auto &materials = model.materials;
	auto &meshList = model.meshList;
	auto &vertexList = model.vertexList;

	auto &textures = model.textures;

	// 物理演算前変形ボーン
	Array<Bone *> A$Physics_bones;

	// 物理演算後変形ボーン
	Array<Bone *> B$Physics_bones;


	for (auto &bone : bones)
	{
		// 仮に全て物理演算前ボーンとする
		A$Physics_bones.emplace_back(&bone);

		continue;

		if (bone.flag.物理後変形)
		{
			B$Physics_bones.emplace_back(&bone);
		}
		else
		{
			A$Physics_bones.emplace_back(&bone);
		}

	}


	std::sort(A$Physics_bones.begin(), A$Physics_bones.end(),
		[](const Bone *a, const Bone *b)
	{

		if (a->変形階層 == b->変形階層)
		{
			return a->index > b->index;
		}
		else
		{
			return a->変形階層 > b->変形階層;
		}

	});




	const auto label = L"Transform Bone";
	gui.addln(L"ts1", GUIToggleSwitch::Create(label, label, true));

	const auto label2 = L"Free Camera";
	gui.addln(L"freeCamera", GUIToggleSwitch::Create(label2, label2, false));

	const auto label3 = L"Bone Angle Limit";
	gui.addln(L"angleLimit", GUIToggleSwitch::Create(label3, label3, true));



	const auto defaultLight = Graphics3D::GetLight(0);
	Graphics3D::SetLight(0, Light::None());


	gui.addln(L"slider", GUISlider::Create(0, 5, 0, 100));

	

	while (System::Update())
	{
		logPos = 0.0;



		Camera camera;

		camera.pos = Vec3(0, 10.0, cameraDistance);
		camera.lookat = Vec3(0, 10.0, 0.0);



		camera.pos = Vec3(-cameraDistance / 2, 10.0, cameraDistance / 2);

		// camera.fovDegree = 30;

		Graphics3D::SetCamera(camera);

		if (gui.toggleSwitch(L"freeCamera").isRight) Graphics3D::FreeCamera();

		// const auto time = System::FrameCount() / 60.0;

		if (Input::Key1.pressed) frame -= timeSpeed;
		if (Input::Key2.pressed) frame += timeSpeed;

		cameraDistance -= Mouse::Wheel();


		frame = Max(0.0, frame);

		Log(Format(L"frame: ", frame), font1);

		// 0. すべてのボーンのローカル／グローバル変形状態を初期化
		for (auto &bone : bones)
		{
			bone.transformParameter.reset();
		}


		// 1. ユーザー操作の回転／移動量をすべてのボーンに設定
		// 2. ボーンモーフによる回転／移動量を対応するボーンに設定
		for (Bone *p_bone : A$Physics_bones)
		{
			Bone &bone = *p_bone;

			auto name = bone.name;

			// キーフレームが存在しない
			if (!motion.has(name)) continue;


			// frame 位置のボーン情報を取得する
			const auto boneStatus = motion.get(name, frame);


			// キーフレームが 2 つ以上あるボーンをデバッグ表示
			if (motion.frames[name].size() > 1)
			{
				Log(Format(name, L": ", motion.frames[name].size()), font2);
			}


			// 設定
			bone.transformParameter.keyframeTranslate = boneStatus.position;
			bone.transformParameter.keyframeRotate = boneStatus.rotation;

		}

		PMX::Bone rootBone;


		rootBone.transformParameter.translate = Vec3::Zero;



		while (true)
		{
			bool end = true;

			// ボーンのローカル移動量、回転量を計算する
			for (auto &p_bone : A$Physics_bones)
			{

				Bone &bone = *p_bone;



				// 親ボーン
				// インデックスが -1 なら rootBone にする
				Bone &parent = bone.parentBoneIndex == -1 ? rootBone : bones[bone.parentBoneIndex];


				Bone *付与_parent = nullptr;


				if (bone.付与_parentBoneIndex != -1)
				{
					付与_parent = &bones[bone.付与_parentBoneIndex];
				}


				// !!!!!!!!!!!!!!!!!!!!!



				// 0. 回転量を単位回転量として開始
				auto rotate = Quaternion::Identity();
				// 0. 移動量を0移動量として開始
				auto translate = Vec3::Zero;



				// 付与を無視（テスト）

				// 1. 当該ボーンが付与の場合 :
				if (付与_parent)
				{

					if (!付与_parent->transformParameter.付与回転計算済)
					{
						end = false;
						continue;
					}

					//  1.1 ローカル付与の場合 : * 付与親のローカル変形量(行列)の回転成分 ※ローカル付与優先
					if (bone.flag.ローカル付与)
					{
						MessageBox::Show(L"ローカル付与");

						rotate *= MatrixToQuaternion(付与_parent->transformParameter.localMatrix);

						// 1.1 ローカル付与の場合 : *付与親のローカル移動量 ※ローカル付与優先

						// TODO: localTranslate を削除して localMatrix の移動成分にする
						translate += 付与_parent->transformParameter.localTranslate;

						// 付与親のローカル移動量:ボーン移動量として計算 ※暫定対応
						// ボーン位置(= ローカル行列の4行1 / 2 / 3列要素) - 初期ボーン位置


					}
					else
					{

						// <> 付与親が付与ボーンの場合 : *付与親の付与回転量(※後述)
						// ??????????
						if (bone.flag.回転付与)
						{

							rotate *= 付与_parent->transformParameter._f_rotate;

						}



						/*
						if (bone.flag.移動付与)
						{
							translate += 付与_parent->transformParameter._f_translate;
							translate += 付与_parent->transformParameter.translate;
							translate += 付与_parent->transformParameter._morph_translate;
						}

						*/

					}
				}


				//  1.2 付与親がIKリンクの場合(且つローカル付与ではない場合) : * 付与親のIKリンク回転量
				// ??????????
				if (付与_parent != nullptr && 付与_parent->flag.IK)
				{
					// rotate *= 付与_parent->transformParameter._ik_r;
				}



				// 1.3 付与率が1以外の場合 : 回転量を付与率で補正(Quaternion.Slerp()想定)
				// 1.2 付与率が1以外の場合 : 移動量を付与率で補正(付与率乗算でOK)

				rotate *= bone.transformParameter.keyframeRotate;

				rotate = Math::Slerp(Quaternion::Identity(), rotate, bone.付与率);

				translate *= bone.付与率;




				// 1.4 付与親のIKリンク～回転モーフ(付与率で補正済み)～回転モーフ を当該ボーンの付与回転量として保存(別の付与ボーンの付与親になった場合の多重付与変形用)
				// ??????????
				bone.transformParameter._f_rotate = rotate;


				// 1.3 親付与移動量(付与率で補正済み)～移動モーフ を当該ボーンの付与移動量として保存(別の付与ボーンの付与親になった場合の多重付与変形用)
				// ??????????
				bone.transformParameter._f_translate = translate;

				bone.transformParameter.付与回転計算済 = true;




				translate += bone.transformParameter.keyframeTranslate;




				// 2. 当該ボーンの回転量追加 : * 回転 * 回転モーフ
				//rotate *= bone.transformParameter._morph_rotate;

				bone.transformParameter.rotate = rotate;


				// 2. 当該ボーンの移動量追加 : +移動 + 移動モーフ
				bone.transformParameter.translate += translate + bone.transformParameter._morph_translate;



				// 3. 当該ボーンがIKリンク回転量を持つ場合 : *IKリンク回転量
				// ??????????
				if (bone.flag.IK)
				{
					//bone.transformParameter.rotate *= bone.transformParameter._ik_r;
				}


				bone.transformParameter.translate = bone.transformParameter.keyframeTranslate;

			}

			if (end) break;
		}


		for (auto &bone : bones)
		{

			bone.transformParameter.localMatrix =

				bone.transformParameter.rotate.toMatrix()

				* Mat4x4::Translate(bone.transformParameter.translate);



		}




		rootBone.transformParameter.transformed = true;


		rootBone.BOfMatrix = Mat4x4::Identity();

		rootBone.animationMatrix = Mat4x4::Identity();

		// 親 -> 子の順で変形するようにループ
		while (true)
		{
			// 全てのボーンの変形が終わったか
			bool end = true;

			for (auto &p_bone : A$Physics_bones)
			{

				Bone &bone = *p_bone;

				if (bone.transformParameter.transformed) continue;



				Bone &parent = bone.parentBoneIndex == -1 ? rootBone : bones[bone.parentBoneIndex];

				// 親の変形が終わっていないなら中断
				if (!parent.transformParameter.transformed) {
					end = false;
					continue;
				}



				bone.transformParameter.transformed = true;




				// 仮
				bone.transformedConnectBonePosition = bone.connectBonePosition + RandomVec3(0.1);


				bone.animationMatrix = Mat4x4(


					bone.BOfMatrix *

					bone.transformParameter.localMatrix *

					bone.BOfMatrix.inverse()

					*
					parent.animationMatrix


				);

				bone.transformedPosition = bone.animationMatrix.transform(bone.position);


				bone.transformedConnectBonePosition =
					bone.animationMatrix.transform(bone.position + bone.connectBonePosition)
					- bone.transformedPosition;


				if (!gui.toggleSwitch(L"ts1").isRight)
				{

					// 初期値
					bone.transformedPosition = bone.position;
					bone.animationMatrix = Mat4x4::Identity();
					bone.transformedConnectBonePosition = bone.connectBonePosition;

				}
			}


			// 全てのボーンの変形終了
			if (end) break;

		}




		// IK 計算（仮）
		// 本来は付与の前に行う

		// http://d.hatena.ne.jp/edvakf/20111102/1320268602


		ClearPrint();

		for (auto &bone : bones)
		{

			if (!bone.flag.IK) continue;


			Bone &ikBone = bone;

			Bone &targetBone = bones[bone.ik.targetIndex];


			// ターゲット, リンク 1, リンク 2, ...
			Array<Bone *> boneList;
			Array<int8> limits;
			boneList.emplace_back(&targetBone);
			limits.emplace_back(0);
			for (auto link : bone.ik.links)
			{

				boneList.emplace_back(&bones[link]);
			}

			std::copy(bone.ik.limits.begin(), bone.ik.limits.end(), std::back_inserter(limits));


			for (auto _ : step( bone.ik.iteration))
			{

				auto i = 0;

				for (auto link : bone.ik.links)
				{
					++i;

					auto &linkBone = bones[link];



					//  リンクボーンからターゲットボーンへの向き
					auto tDir = targetBone.transformedPosition - linkBone.transformedPosition;

					/*
					font1.drawAt(Format(i), Graphics3D::ToScreenPos(
						linkBone.transformedPosition
					).xy(), Palette::Red);
					*/

					// リンクボーンから IK ボーン への向き
					auto lDir = ikBone.transformedPosition - linkBone.transformedPosition;




					// (1)を(2)の位置に向ける回転行列の計算
					float rotationDotProduct = Vec3(tDir.normalized()).dot(lDir.normalized());
					float rotationAngle = Acos(rotationDotProduct);

					// 外積が回転軸
					auto rotationAxis = Vec3(tDir).cross(lDir);
					rotationAxis.normalize();
					// 回転軸と回転角度からクォータニオンを生成
					auto q2 = Quaternion(rotationAxis, rotationAngle);


					Println(L"Rotation Angle: ", rotationAngle);
					Println(L"LIM: ", bone.ik.limitAngle);

					/*
					auto q = Quaternion::RotationArc(tDir.normalized(), lDir.normalized(), RandomVec3());
					q = q2;
					*/

					auto q = q2;


					// 制限角度を超えている
					if (Abs(rotationAngle) > bone.ik.limitAngle)
					{
						// $(L"ループの制限角度を実装していません");

						rotationAngle = Sign(rotationAngle) * bone.ik.limitAngle;

					}


					if (gui.toggleSwitch(L"angleLimit").isRight)
					{


					}




					auto min = Vec3(-180_deg, -180_deg, -180_deg);
					auto max = Vec3(+180_deg, +180_deg, +180_deg);

					bool log = false;



					if (bone.ik.limits[i - 1])
					{
					//	min = Vec3(-180_deg, 0, 0);
					//	max = Vec3(0.00, 0, 0);
						log = true;
					}



					// リンクボーンより先にあるボーンを変形する
					for (auto j : step(i))
					{

						Bone &linkPBone = *boneList[j];

						// ボーン n の座標を リンクボーンのローカルに戻す
						auto localPos = boneList[j]->transformedPosition - linkBone.transformedPosition;
						 
						auto mat = q.toMatrix();

						boneList[j]->animationMatrix = Mat4x4::Translate(RandomVec3());




						// ボーンを回すクォータニオン
						auto rq = Quaternion::RotationArc(tDir.normalized(), lDir.normalized());

			
						/*


						if (bone.ik.limits[i - 1])
						// if (limits[j])
						{

							rq = QtoEtoQ2(rq);

						}





						auto rqMat = rq.toMatrix();


						// TODO: クォータニオンをオイラー角にして、元に戻すだけの処理をテスト
						// QtoEtoQ(rq);



						// if (linkPBone.ik.limits
						*/


						// リンクボーン 
						// auto l_axis = (linkBone.transeformedPosition - bones[linkBone.parentBoneIndex].transeformedPosition).normalize();
						auto l_axis = (linkBone.transformedPosition - bones[linkBone.parentBoneIndex].transformedPosition).normalize();


						localPos = D3DXVec3RotateReg(rotationAxis, rotationAngle, localPos,
							min,
							max,
							l_axis
						);
						/*


						localPos = rqMat.transform(localPos);

						*/
						// Println(localPos);

						// 変形
						// localPos = mat.transform(localPos);

						// ボーン n の座標をグローバルに戻す
						boneList[j]->transformedPosition = linkBone.transformedPosition + localPos;

					}



				}




			}

		}


		// 頂点をボーンに合わせて変形する
		for (auto &vertex : vertexList)
		{


			if (vertex.weightType == WeightType::BDEF1)
			{

				auto &bone1 = bones[vertex.boneIndex1];

				auto lp = vertex.position;

				vertex.transformedPosition = bone1.animationMatrix.transform(lp);

			}

			else if (vertex.weightType == WeightType::BDEF2)
			{

				auto &bone1 = bones[vertex.boneIndex1];
				auto &bone2 = bones[vertex.boneIndex2];

				auto lp1 = vertex.position;
				auto lp2 = vertex.position;


				auto v1 = bone1.animationMatrix.transform(lp1);
				auto v2 = bone2.animationMatrix.transform(lp2);

				vertex.transformedPosition = Math::Lerp(v2, v1, vertex.boneWeight1);


			}
			else if (vertex.weightType == WeightType::BDEF4)
			{


				auto &bone1 = bones[vertex.boneIndex1];
				auto &bone2 = bones[vertex.boneIndex2];
				auto &bone3 = bones[vertex.boneIndex3];
				auto &bone4 = bones[vertex.boneIndex4];

				auto lp1 = vertex.position;
				auto lp2 = vertex.position;
				auto lp3 = vertex.position;
				auto lp4 = vertex.position;

				auto v1 = bone1.animationMatrix.transform(lp1);
				auto v2 = bone2.animationMatrix.transform(lp2);
				auto v3 = bone3.animationMatrix.transform(lp3);
				auto v4 = bone4.animationMatrix.transform(lp4);



				// ウェイトの合計
				auto aw = (vertex.boneWeight1 + vertex.boneWeight2 + vertex.boneWeight3 + vertex.boneWeight4);



				// 正規化する（仮）
				auto s = (aw / 1.0);
				double w1 = vertex.boneWeight1 * s;
				double w2 = vertex.boneWeight2 * s;
				double w3 = vertex.boneWeight3 * s;
				double w4 = vertex.boneWeight4 * s;


				vertex.transformedPosition = (
					v1 * w1 +
					v2 * w2 +
					v3 * w3 +
					v4 * w4
					);


			}

			else
			{
				MessageBox::Show(Format((int)vertex.weightType));
			}



			//



		}


		for (auto &mesh : meshList)
		{

			auto i = 0;

			for (auto &vertex : mesh.vertices)
			{
				vertex.position = vertexList[mesh.v_index[i++]].transformedPosition;
			}

			mesh.mesh.fillVertices(mesh.vertices);

		}





		model.draw();
		model.drawShadow();


		model.drawBone();




		Graphics3D::SetDepthState(DepthState::None);



		Graphics3D::SetDepthState(DepthState::Default3D);




		Plane(100).draw(textureGround);





	}
}
