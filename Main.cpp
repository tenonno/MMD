
# include <Siv3D.hpp>


# include "Vertex.hpp"


#include "Material.hpp"


# include "Reader.hpp"

#include "Bone.hpp"

#include "Model.hpp"


#include "Utils.hpp"


namespace PMX
{


	struct Header
	{

		// 4byte + 末尾
		uint8 type[5];

		float version;

		uint8 dataSize;

		Array<uint8> datas;




	};

}


#include "Motion.hpp"

#include <array>
#include <memory>

void Main()
{

	Window::Resize(1280, 720);

	FilePath path = L"reim/reim.pmx";


	// path = L"C:/Users/tis-teno/Desktop/サーバルちゃんver1.01/サーバルちゃんver1.01/サーバルちゃんver1.01.pmx";

	path = L"C:/Users/tis-teno/Desktop/サーバルちゃんver1.01/サーバルちゃんver1.01/付与なし.pmx";


	String directory = L"";

	auto vv = path.split('/');
	for (auto i = 0; i < vv.size() - 1; ++i)
	{
		directory += (vv[i]) + L"/";
	}

	// Motion motion(L"Assets/reim/motion.vmd");

	Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/サーバルメトロノーム.vmd");
	// Motion motion(L"C:/Users/tis-teno/Desktop/Rick式サーバルメトロノーム/Rick式サーバルメトロノーム/test.vmd");


	PMX::Reader reader(path);



	PMX::Header header;

	reader.read(&header.type, 4);
	// 末尾
	header.type[4] = 0;

	auto a2 = WidenAscii((char *)header.type);
	Println(a2);


	reader.read(&header.version, 4);
	reader.read(&header.dataSize, 1);




	Println(header.version);

	Println(L"データサイズ", header.dataSize);


	header.datas.resize((size_t)header.dataSize);


	// 8 列のデータを読み込む
	for (auto i = 0; i < header.dataSize; i++)
	{

		reader.read(&header.datas[i], 1);

		// Println(L"データ", i, L": ", header.datas[i]);

	}


	// エンコード
	const auto encoding = header.datas[0] ? TextEncoding::UTF8 : TextEncoding::UTF16LE;

	reader.setEncoding(encoding);


	const auto readBool = [&reader]() -> bool
	{
		return reader.value<uint8>() > 0;
	};


	const auto readInt = [&reader]() -> int
	{
		int size;
		reader.read(&size, 4);
		return size;
	};


	// name, nameEng, comment, commentEng
	for (auto i : step(4))
	{

		const auto text = reader.value<String>();

		Println(i, L": ", text);

	}


	int32 vertexNum;

	reader.read(&vertexNum, 4);

	Println(L"頂点数", vertexNum);

	Array<Vertex> vertexList;

	const size_t optionalUVSize = header.datas[1];


	auto vertexIndexSize = header.datas[2];
	auto textureIndexSize = header.datas[3];
	auto materialIndexSize = header.datas[3];
	auto boneIndexSize = header.datas[5];

	for (auto vertexIndex : step(vertexNum))
	{

		Vertex vertex;

		// 頂点座標
		vertex.position = reader.value<Vec3>();

		// 法線
		vertex.normal = reader.value<Vec3>();

		// UV
		vertex.texcoord = reader.value<Vec2>();


		// 4 つ以上の追加 UV 情報を持っている場合
		// PMX の仕様では上限 4
		if (optionalUVSize > 4)
		{
			MessageBox::Show(L"追加 UV 情報の数が異常です");
		}


		// 追加 UV
		for (auto i : step(optionalUVSize))
		{

			vertex.optionalUV[i] = reader.value<Vec4>();

		}

		uint8 wType;
		reader.read(&wType, 1);

		auto WT = (WeightType)wType;


		vertex.weightType = WT;


		// ボーンはスキップする

		if (WT == WeightType::BDEF1) {
			vertex.boneIndex1 = reader.value(boneIndexSize);
		}

		if (WT == WeightType::BDEF2) {

			vertex.boneIndex1 = reader.value(boneIndexSize);
			vertex.boneIndex2 = reader.value(boneIndexSize);

			vertex.boneWeight1 = reader.value<double>();

		}

		if (WT == WeightType::BDEF4) {


			vertex.boneIndex1 = reader.value(boneIndexSize);
			vertex.boneIndex2 = reader.value(boneIndexSize);
			vertex.boneIndex3 = reader.value(boneIndexSize);
			vertex.boneIndex4 = reader.value(boneIndexSize);

			vertex.boneWeight1 = reader.value<double>();
			vertex.boneWeight2 = reader.value<double>();
			vertex.boneWeight3 = reader.value<double>();
			vertex.boneWeight4 = reader.value<double>();
		}


		if (WT == WeightType::SDEF) {
			reader.skip(boneIndexSize);
			reader.skip(boneIndexSize);
			reader.skip(4);
			reader.skip(12);
			reader.skip(12);
			reader.skip(12);
		}



		vertex.edgeScale = reader.value<double>();

		vertexList.emplace_back(vertex);

	}



	// 面を構成している頂点の数
	const size_t faceVertexNum = reader.value<int32>();

	// 面の数
	const auto faceNum = faceVertexNum / 3;

	Println(L"面数", faceNum);


	struct Face
	{
		uint64 v1;
		uint64 v2;
		uint64 v3;
	};

	Array<Face> faceList;

	for (auto faceIndex : step(faceNum))
	{

		Face face;

		face.v1 = reader.value(vertexIndexSize);
		face.v2 = reader.value(vertexIndexSize);
		face.v3 = reader.value(vertexIndexSize);

		faceList.emplace_back(face);

	}


	const size_t textureNum = reader.value<int32>();

	Println(L"テクスチャ数", textureNum);




	Array<Texture> textures;

	for (auto textureIndex : step(textureNum))
	{

		const auto name = reader.value<String>();

		auto textureName = directory + name;

		Texture texture(textureName, TextureDesc::For3D);

		Println(textureIndex, L": ", textureName);

		textures.emplace_back(texture);

	}




	const auto materialNum = reader.value<int32>();


	Println(L"材質数", materialNum);

	Array<PMX::Material> materialList;
	materialList.resize(materialNum);


	for (auto materialIndex : step(materialNum))
	{

		PMX::Material material;



		auto materialName = reader.value<String>();



		auto materialNameEng = reader.value<String>();
		Println(materialName, materialNameEng);

		// Diffuse
		material.diffuse = reader.value<Vec4>();

		// Specular
		material.specular = reader.value<Vec3>();

		// Specular 係数
		material.Specular_K = reader.value<double>();


		// Ambient
		material.ambient = reader.value<Vec3>();

		uint8 bitFrag = reader.value<uint8>();

		// Edge Color
		material.edgeColor = reader.value<Vec4>();

		// Edge Size
		material.edgeSize = reader.value<double>();


		material.textureIndex = reader.value(textureIndexSize);
		material.sTextureIndex = reader.value(textureIndexSize);


		reader.value<uint8>();
		material.UTOON = readBool();


		// 共有トーンテクスチャを使うなら
		// 共有トーンテクスチャは 0 ~ 9 のインデックス
		if (material.UTOON)
		{

			material.toneTextureIndex = reader.value<uint8>();

		}
		// オリジナルのトーンテクスチャを使う
		else
		{

			material.toneTextureIndex = reader.value(textureIndexSize);

		}

		auto text = reader.value<String>();


		// 面を構成している頂点の数
		material.faceVertexCount = reader.value<int>();

		// 面の数
		material.faceCount = material.faceVertexCount / 3;

		materialList[materialIndex] = material;

	}


	const size_t boneCount = reader.value<int32>();


	using namespace PMX;

	Array<Bone> bones;


	// ボーンの接続先がないなら true
	const auto $boneNoneConnect = [](const Bone &bone)
	{
		return bone.flag.connectType == BoneConnectType::Index && bone.connectBoneIndex == -1;
	};

	const auto $getBoneConnectPosition = [&bones](const Bone &bone) -> Vec3
	{


		if (bone.flag.connectType == BoneConnectType::Position)
		{
			return bone.transformedPosition + bone.transformedConnectBonePosition;
		}

		if (bone.flag.connectType == BoneConnectType::Index)
		{
			return bones[bone.connectBoneIndex].transformedPosition;
		}

	};


	for (auto boneIndex : step(boneCount))
	{

		Bone bone;

		bone.index = boneIndex;

		bone.name = reader.value<String>();
		bone.nameEng = reader.value<String>();


		bone.position = reader.value<Vec3>();

		bone.parentBoneIndex = reader.value(boneIndexSize);

		bone.変形階層 = reader.value<int>();



		BoneFlag flag;

		auto flags = reader.value<int16>();

		flag.connectType = (BoneConnectType)(flags & 0x0001);


		flag.回転可能 = flags & 0x0002;
		flag.移動可能 = flags & 0x0004;
		flag.visible = flags & 0x0008;
		flag.操作可 = flags & 0x0010;
		flag.IK = flags & 0x0020;
		flag.ローカル付与 = flags & 0x0080;
		flag.回転付与 = flags & 0x0100;
		flag.移動付与 = flags & 0x0200;
		flag.軸固定 = flags & 0x0400;
		flag.ローカル軸 = flags & 0x0800;
		flag.物理後変形 = flags & 0x1000;
		flag.外部親変形 = flags & 0x2000;


		/*
		接続先:0 の場合
		12 : float3	| 座標オフセット, ボーン位置からの相対分

		接続先:1 の場合
		n  : ボーンIndexサイズ  | 接続先ボーンのボーンIndex
		*/

		Vec3 connectPos;

		if (flag.connectType == BoneConnectType::Index)
		{
			bone.connectBoneIndex = reader.value(boneIndexSize);
		}
		if (flag.connectType == BoneConnectType::Position)
		{
			bone.connectBonePosition = reader.value<Vec3>();
		}

		/*
		回転付与:1 または 移動付与:1 の場合
		n  : ボーンIndexサイズ  | 付与親ボーンのボーンIndex
		4  : float	| 付与率
		*/

		if (flag.回転付与 || flag.移動付与)
		{

			bone.付与_parentBoneIndex = reader.value(boneIndexSize);
			bone.付与率 = reader.value<double>();




		}

		/*
		軸固定:1 の場合
		12 : float3 | 軸の方向ベクトル
		*/
		if (flag.軸固定)
		{
			auto 軸の方向ベクトル = reader.readVec3();
		}

		/*
		ローカル軸:1 の場合
		12 : float3	| X軸の方向ベクトル
		12 : float3	| Z軸の方向ベクトル ※フレーム軸算出方法は後述
		*/
		if (flag.ローカル軸)
		{
			// この情報は必要ない
			/* auto X軸の方向ベクトル = */ reader.value<Vec3>();
			/* auto Z軸の方向ベクトル = */ reader.value<Vec3>();
		}

		/*
		外部親変形:1 の場合
		*/
		if (flag.外部親変形)
		{
			auto KEY値 = reader.readInt();
		}

		/*
		IK:1 の場合 IKデータを格納
		*/
		if (flag.IK)
		{

			// n: ボーンIndexサイズ | IKターゲットボーンのボーンIndex
			auto ikTargetBoneIndex = reader.fromSize(boneIndexSize);

			// 4 : int | IKループ回数(PMD及びMMD環境では255回が最大になるようです)
			auto ikLoopNum = reader.readInt();

			// 4 : float | IKループ計算時の1回あたりの制限角度->ラジアン角 | PMDのIK値とは4倍異なるので注意
			auto ikLoopLimitAngle = reader.readFloat();

			// 4  : int  	| IKリンク数 : 後続の要素数
			auto ikLinkNum = reader.readInt();

			// <IKリンク>
			for (auto ikLinkIndex = 0; ikLinkIndex < ikLinkNum; ++ikLinkIndex)
			{

				// n: ボーンIndexサイズ | リンクボーンのボーンIndex
				auto ikLinkBoneIndex = reader.fromSize(boneIndexSize);

				// 1 : byte | 角度制限 0 : OFF 1 : ON
				auto limitAngle = reader.readBool();

				// 角度制限 : 1の場合
				if (limitAngle)
				{
					// 12 : float3 | 下限(x, y, z)->ラジアン角
					auto min = reader.readVec3();

					// 12 : float3 | 上限(x, y, z)->ラジアン角
					auto max = reader.readVec3();

				}

			}

		}



		Println(bone.name, bone.nameEng);

		bone.flag = flag;

		bones.emplace_back(bone);

	}


	// ボーンの BOf 行列を作る
	for (auto &bone : bones)
	{







		Mat4x4 初期姿勢行列;


		if (!$boneNoneConnect(bone))
		{

			// 接続先座標
			auto connectPos = $getBoneConnectPosition(bone);

			// ボーンから接続先に向かう回転
			auto rotate = Quaternion(bone.position, connectPos).toMatrix();

			初期姿勢行列 = rotate * Mat4x4::Translate(bone.position);

		}
		// 接続先がないなら
		else
		{
			初期姿勢行列 = Mat4x4::Translate(bone.position);
		}

		bone.BOfMatrix = 初期姿勢行列.inverse();



	}


	auto モーフ数 = reader.value<int32>();


	auto faceIndex = 0;


	struct PMXMesh
	{

		DynamicMesh mesh;

		PMX::Material material;

		Array<MeshVertex> vertices;
		Array<int64> v_index;

	};
	Array<PMXMesh> meshList;

	// マテリアルからメッシュを作る
	for (auto material : materialList)
	{
		Array<MeshVertex> vertices;
		Array<uint32> indices;

		vertices.reserve(material.faceVertexCount);
		indices.reserve(material.faceVertexCount);

		PMXMesh pmxMesh;


		// 面の数だけ回す
		for (auto i : step(material.faceCount))
		{

			// 面
			auto face = faceList[faceIndex];

			// TODO: 同じ頂点が既に追加されていたらそれを参照するように

			vertices.emplace_back(vertexList[face.v1].toMeshVertex());
			vertices.emplace_back(vertexList[face.v2].toMeshVertex());
			vertices.emplace_back(vertexList[face.v3].toMeshVertex());


			pmxMesh.v_index.emplace_back(face.v1);
			pmxMesh.v_index.emplace_back(face.v2);
			pmxMesh.v_index.emplace_back(face.v3);


			indices.emplace_back(i * 3 + 0);
			indices.emplace_back(i * 3 + 1);
			indices.emplace_back(i * 3 + 2);


			++faceIndex;
		}



		auto mesh = DynamicMesh(MeshData(vertices, indices));

		pmxMesh.mesh = mesh;
		pmxMesh.material = material;

		pmxMesh.vertices = std::move(vertices);

		meshList.emplace_back(pmxMesh);


	}


	Array<Face>().swap(faceList);


	const Texture textureTerrain(L"Example/Grass.jpg", TextureDesc::For3D);


	Graphics3D::SetAmbientLight(ColorF(1.0));




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



	Font font1(14, L"Consolas");
	Font font2(8, L"メイリオ");




	double logPos = 0.0;
	const auto Log = [&](const String &text, const Font &font)
	{
		font.draw(text, 1000, logPos);
		logPos += font.height + 2.0;
	};


	Graphics3D::SetLight(0, Light::None());

	Graphics::SetBackground(ColorF(0.3));

	reader.close();

	auto frame = 0.0;

	const auto timeSpeed = 0.508;

	double cameraDistance = -30.0;



	GUI gui(GUIStyle::Default);

	gui.setPos(150, 50);



	const auto label = L"Transform Bone";
	gui.add(L"ts1", GUIToggleSwitch::Create(label, label, true));



	while (System::Update())
	{
		logPos = 0.0;





		Camera camera;

		camera.pos = Vec3(0, 10.0, cameraDistance);
		camera.lookat = Vec3(0, 10.0, 0.0);

		camera.fovDegree = 30;

		Graphics3D::FreeCamera();
		Graphics3D::SetCamera(camera);


		// const auto time = System::FrameCount() / 60.0;

		if (Input::Key1.pressed) frame -= timeSpeed;
		if (Input::Key2.pressed) frame += timeSpeed;

		cameraDistance -= Mouse::Wheel();


		frame = Max(0.0, frame);

		Log(Format(L"frame: ", frame), font1);


		// 0. すべてのボーンのローカル／グローバル変形状態を初期化
		for (auto &bone : A$Physics_bones)
		{
			bone->transformParameter.reset();
		}

		// 1. ユーザー操作の回転／移動量をすべてのボーンに設定
		// 2. ボーンモーフによる回転／移動量を対応するボーンに設定
		for (Bone *p_bone : A$Physics_bones)
		{
			Bone &bone = *p_bone;

			auto name = bone.name;

			// キーフレームが存在しない
			const auto count = motion.frames.count(name);
			if (!count) continue;


			// 名前が登録されているが、キーフレームの数が 0 の場合
			// 原因不明のバグ
			if (!motion.frames[name].size()) continue;


			// frame 位置のボーン情報を取得する
			const auto boneStatus = motion.get(name, frame);


			if (motion.frames[name].size() > 1)
			{
				Log(Format(name, L": ", motion.frames[name].size()), font2);
			}


			// 設定
			bone.transformParameter.keyframeTranslate = boneStatus.position;
			bone.transformParameter.keyframeRotate = boneStatus.rotation;

		}

		PMX::Bone rootBone;

		rootBone.transformParameter.付与情報計算済 = true;

		rootBone.transformParameter.translate = Vec3::Zero;

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
			if (付与_parent != nullptr)
			{

				//  1.1 ローカル付与の場合 : * 付与親のローカル変形量(行列)の回転成分 ※ローカル付与優先
				if (bone.flag.ローカル付与)
				{

					rotate *= MatrixToQuaternion(付与_parent->transformParameter.localMatrix);

					// 1.1 ローカル付与の場合 : *付与親のローカル移動量 ※ローカル付与優先

					// TODO: localTranslate を削除して localMatrix の移動成分にする
					translate += 付与_parent->transformParameter.localTranslate;

					// 付与親のローカル移動量:ボーン移動量として計算 ※暫定対応
					// ボーン位置(= ローカル行列の4行1 / 2 / 3列要素) - 初期ボーン位置

					MessageBox::Show(L"ローカル付与");

				}
				else
				{

					// <> 付与親が付与ボーンの場合 : *付与親の付与回転量(※後述)
					// ??????????
					if (bone.flag.回転付与)
					{
						rotate *= 付与_parent->transformParameter._f_rotate;
					}
					// <> 付与ボーンではない場合 : * 付与親の回転 * 付与親の回転モーフ
					else
					{
						rotate *= 付与_parent->transformParameter.rotate;

						rotate = rotate * 付与_parent->transformParameter._morph_rotate;


					}


					if (bone.flag.移動付与)
					{
						translate += 付与_parent->transformParameter._f_translate;
						translate += 付与_parent->transformParameter.translate;
						translate += 付与_parent->transformParameter._morph_translate;
					}



				}
			}


			//  1.2 付与親がIKリンクの場合(且つローカル付与ではない場合) : * 付与親のIKリンク回転量
			// ??????????
			if (付与_parent != nullptr && 付与_parent->flag.IK)
			{
				rotate *= 付与_parent->transformParameter._ik_r;
			}

			// 1.3 付与率が1以外の場合 : 回転量を付与率で補正(Quaternion.Slerp()想定)
			// 1.2 付与率が1以外の場合 : 移動量を付与率で補正(付与率乗算でOK)
			if (bone.付与率 != 1.0)
			{
				Math::Slerp(Quaternion::Identity(), rotate, bone.付与率);

				translate *= bone.付与率;

			}




			// 1.4 付与親のIKリンク～回転モーフ(付与率で補正済み)～回転モーフ を当該ボーンの付与回転量として保存(別の付与ボーンの付与親になった場合の多重付与変形用)
			// ??????????
			bone.transformParameter._f_rotate = rotate;
			// 1.3 親付与移動量(付与率で補正済み)～移動モーフ を当該ボーンの付与移動量として保存(別の付与ボーンの付与親になった場合の多重付与変形用)
			// ??????????
			bone.transformParameter._f_translate = translate;

			// 2. 当該ボーンの回転量追加 : * 回転 * 回転モーフ
			rotate *= bone.transformParameter.rotate * bone.transformParameter._morph_rotate;

			bone.transformParameter.rotate = rotate;


			// 2. 当該ボーンの移動量追加 : +移動 + 移動モーフ
			bone.transformParameter.translate += translate + bone.transformParameter._morph_translate;





			bone.transformParameter.translate += bone.transformParameter.keyframeTranslate;
			bone.transformParameter.rotate *= bone.transformParameter.keyframeRotate;




			/*
											  // 独自の処理
			bone.transformParameter.translate =

				bone.transformParameter.translate
				-
				parent.transformParameter.translate;

			*/


			// 3. 当該ボーンがIKリンク回転量を持つ場合 : *IKリンク回転量
			// ??????????
			if (bone.flag.IK)
			{
				bone.transformParameter.rotate *= bone.transformParameter._ik_r;
			}


		}



		// ボーンのローカル変形行列を作る
		for (Bone *p_bone : A$Physics_bones)
		{
			Bone &bone = *p_bone;

			bone.globalTranslate = Vec3::Zero;


			bone.transformParameter.localMatrix =

				bone.transformParameter.rotate.toMatrix()

				* Mat4x4::Translate(bone.transformParameter.translate);



			// bone.transformParameter.localTransformMatrix = Mat4x4::Identity();

		}


		rootBone.transformParameter.rotate = Quaternion::Identity();

		rootBone.transformedPosition = Vec3::Zero;

		rootBone.transformParameter.transformed = true;


		rootBone.BOfMatrix = Mat4x4::Identity();
		rootBone.ボーンオフセット行列 = Mat4x4::Identity();

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

					Mat4x4(bone.BOfMatrix).inverse()

					*
					parent.animationMatrix


				);

				bone.transformedPosition = bone.animationMatrix.transform(bone.position);




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

		for (auto &vertex : vertexList)
		{
			vertex.transformedPosition = vertex.position;



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

				Println(vertex.boneWeight1);

				vertex.transformedPosition = Math::Lerp(v2, v1, vertex.boneWeight1);

				// vertex.transformedPosition += RandomVec3(0.1);


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


				/*

				auto s1 = bones[vertex.boneIndex1].transformedPosition - bones[vertex.boneIndex1].position;
				auto s2 = bones[vertex.boneIndex2].transformedPosition - bones[vertex.boneIndex2].position;
				auto s3 = bones[vertex.boneIndex3].transformedPosition - bones[vertex.boneIndex3].position;
				auto s4 = bones[vertex.boneIndex4].transformedPosition - bones[vertex.boneIndex4].position;

				auto p = (
					s1 * vertex.boneWeight1 +
					s2 * vertex.boneWeight2 +
					s3 * vertex.boneWeight3 +
					s4 * vertex.boneWeight4
					);

				vertex.transformedPosition += p;
				*/

			}

			else
			{
				MessageBox::Show(Format((int)vertex.weightType));
			}



			//



		}


		for (auto &mesh : meshList)
		{

			auto texture = textures[mesh.material.textureIndex];


			auto i = 0;

			for (auto &vertex : mesh.vertices)
			{
				vertex.position = vertexList[mesh.v_index[i++]].transformedPosition;
			}

			mesh.mesh.fillVertices(mesh.vertices);


			mesh.mesh.draw(texture);
			mesh.mesh.drawShadow();

		}


		Graphics3D::SetDepthState(DepthState::None);


		// ボーンを描画する
		for (Bone *p_bone : A$Physics_bones)
		{
			Bone &bone = *p_bone;

			// 非表示ボーン
			if (!bone.flag.visible) continue;


			const Vec2 position = Graphics3D::ToScreenPos(bone.transformedPosition).xy();


			// Circle(position, 5.0).draw(color);


			// 接続先がない
			if ($boneNoneConnect(*p_bone)) continue;


			const Vec2 connectPosition = Graphics3D::ToScreenPos(
				$getBoneConnectPosition(bone)
			).xy();


			auto angle = (connectPosition - position).normalized();

			angle *= 5.0;

			auto L = position + Mat3x2::Rotate(90_deg).transform(angle);
			auto R = position + Mat3x2::Rotate(-90_deg).transform(angle);


			const ColorF lineColor(0.0, 0.8, 0.0);
			const double lineWidth = 1.0;


			Circle(position, 5.0).drawFrame(lineWidth, 0.0, lineColor);
			Line(L, connectPosition).draw(lineWidth, lineColor);
			Line(R, connectPosition).draw(lineWidth, lineColor);



		}




		for (auto &bone : bones)
		{
			if (bone.name != L"全ての親") continue;


			Log(Format(L"移動量: ", bone.transformParameter.translate), font1);
			Log(Format(L"回転量: ", bone.transformParameter.rotate), font1);

		}





		Graphics3D::SetDepthState(DepthState::Default3D);




		Plane(100).draw(ColorF(0.2));





	}
}
