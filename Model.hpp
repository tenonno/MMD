#pragma once


#include <Siv3D.hpp>


#include "Utils.hpp"



# include "Vertex.hpp"


#include "Material.hpp"


#include "Reader.hpp"

#include "Bone.hpp"




struct PMXMesh
{

	DynamicMesh mesh;

	PMX::Material material;

	Array<MeshVertex> vertices;
	Array<int64> v_index;

};


namespace PMX
{


	class Model
	{


	public:


		Model(const FilePath &path)
		{

			
			String directory = GetDirectory(path);


			PMX::Reader reader(path);



			PMX::Header header;

			reader.read(&header.type, 4);
			// 末尾
			header.type[4] = 0;

			Println(WidenAscii((char *)header.type));

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



			// name, nameEng, comment, commentEng
			for (auto i : step(4))
			{

				const auto text = reader.value<String>();

				Println(i, L": ", text);

			}


			int32 vertexNum;

			reader.read(&vertexNum, 4);

			Println(L"頂点数", vertexNum);


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

				// スフィアモード 0:無効 1:乗算(sph) 2:加算(spa) 3:サブテクスチャ(追加UV1のx,yをUV参照して通常テクスチャ描画を行う)
				reader.value<uint8>();

				material.UTOON = reader.value<bool>();


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
					bone.ik.targetIndex = reader.value(boneIndexSize);

					// 4 : int | IKループ回数(PMD及びMMD環境では255回が最大になるようです)
					bone.ik.iteration = reader.value<uint32>();

					// 4 : float | IKループ計算時の1回あたりの制限角度->ラジアン角 | PMDのIK値とは4倍異なるので注意
					bone.ik.limitAngle = reader.value<double>();

					// 4  : int  	| IKリンク数 : 後続の要素数
					auto ikLinkNum = reader.readInt();

					// <IKリンク>
					for (auto ikLinkIndex : step(ikLinkNum ))
					{

						// n: ボーンIndexサイズ | リンクボーンのボーンIndex
						auto ikLinkBoneIndex = reader.value(boneIndexSize);


						bone.ik.links.emplace_back(ikLinkBoneIndex);

						// 1 : byte | 角度制限 0 : OFF 1 : ON
						auto limitAngle = reader.value<bool>();

						bone.ik.limits.emplace_back(limitAngle ? 1 : 0);

						// 角度制限 : 1の場合
						if (limitAngle)
						{


						
							// $(L"角度制限");

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



			// マテリアルからメッシュを作る
			for (auto &material : materialList)
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






			this->materials = materialList;
		}


		Array<PMXMesh> meshList;


		Array<Material> materials;
		Array<Bone> bones;

		Array<Texture> textures;
		Array<Vertex> vertexList;

		// ボーンの接続先がないなら true
		bool $boneNoneConnect(const Bone &bone) const
		{
			return bone.flag.connectType == BoneConnectType::Index && bone.connectBoneIndex == -1;
		};

		Vec3 $getBoneConnectPosition(const Bone &bone)  const
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





		void updateBonePhase1() const
		{

		}



		void transform() 
		{


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

					auto v1 = bone1.animationMatrix.transform(vertex.position);
					auto v2 = bone2.animationMatrix.transform(vertex.position);
					auto v3 = bone3.animationMatrix.transform(vertex.position);
					auto v4 = bone4.animationMatrix.transform(vertex.position);


					// ウェイトの合計
					auto aw = (vertex.boneWeight1 + vertex.boneWeight2 + vertex.boneWeight3 + vertex.boneWeight4);



					// 正規化する（仮）
					auto s = 1.0;// (aw / 1.0);
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
					$((int)vertex.weightType);
				}


			}







		}



		void draw() const;

		void drawShadow() const;


		void drawBone() const;



	};











}