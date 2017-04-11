#pragma once


#include <Siv3D.hpp>


#include "Initialize.hpp"

#include "Utils.hpp"
#include "Vertex.hpp"
#include "Material.hpp"
#include "Reader.hpp"
#include "Bone.hpp"

struct Face
{
	uint64 v1;
	uint64 v2;
	uint64 v3;
};


struct PMXMesh
{

	DynamicMesh mesh;

	PMX::Material material;

	Array<int64> v_index;

};

struct alignas(16) PSPS
{
	Float4 edgeColor;
	float edgeSize;
	float useToon;
};
namespace PMX
{

	class Model
	{

	public:

		Model::Model(const FilePath &path);

		Array<PMXMesh> meshes;
		Array<Material> materials;
		Array<Bone> bones;
		Array<Texture> textures;
		Array<Vertex> vertices;
		Array<Face> faceList;


		Float3 position = Vec3::Zero;
		Float3 scale = Vec3::One;
		Float3 rotate = Vec3::Zero;

		Quaternion rotateQ = Quaternion::Identity();
		Quaternion nowRotateQ = Quaternion::Identity();

		std::unordered_map<String, Bone *> boneMap;

		// モデルの三角面を全て取得する
		Array<Triangle3D> getT3D()
		{
			Array<Triangle3D> ts;


			for (auto &face : faceList)
			{

				Triangle3D tr(

					transformVertex(vertices[face.v1].position),
					transformVertex(vertices[face.v2].position),
					transformVertex(vertices[face.v3].position)

				);

				ts.emplace_back(tr);

			}

			return ts;

		}


		Vec3 getBonePos(const String &name)
		{
			return transformVertex(boneMap[name]->transformedPosition);
		}

		Vec3 transformVertex(const Vec3 &vertex) const
		{

			const Mat4x4 matrix = Mat4x4::Scale(scale) * Mat4x4::Rotate(getRotate()) * Mat4x4::Translate(position);

			return matrix.transform(vertex);

		}




		// ボーンの接続先がないなら true
		bool $boneNoneConnect(const Bone &bone) const
		{
			return bone.flag.connectType == BoneConnectType::Index && bone.connectBoneIndex == -1;
		};

		// ボーンの接続先座標を取得する
		Vec3 $getBoneConnectPosition(const Bone &bone)  const
		{
			Vec3 position;

			if (bone.flag.connectType == BoneConnectType::Position)
			{
				position = bone.transformedPosition + bone.transformedConnectBonePosition;
			}
			if (bone.flag.connectType == BoneConnectType::Index)
			{
				position = bones[bone.connectBoneIndex].transformedPosition;
			}


			return transformVertex(position);

		};


		void forEach_bones(const std::function<bool(Bone &)> &_if, const std::function<void(Bone &)> &callback)
		{

			while (true)
			{
				bool end = true;

				for (auto &bone : bones)
				{

					if (!_if(bone))
					{
						end = false;
						continue;
					}

					callback(bone);

				}

				if (end) break;
			}

		}




		// 物理演算前変形ボーン
		Array<Bone *> A$Physics_bones;

		// 物理演算後変形ボーン（仮）
		Array<Bone *> B$Physics_bones;


		// モーションを適用してボーンを変形する
		void updateMotion();

		// ボーンの変形に合わせて頂点を変形する
		void updateVertex();

		void draw() const;
		void drawForward() const;



		struct alignas(16) PS_TOON
		{
			Float3 lightDirection;
			float useToon;
		};


		void drawMMD(const VertexShader &vs, const PixelShader &ps) const
		{

			Graphics3D::BeginVSForward(vs);
			Graphics3D::BeginPSForward(ps);

			Graphics3D::SetRasterizerStateForward(RasterizerState(FillMode::Solid, CullMode::None));

			for (auto &mesh : meshes)
			{
				auto &material = mesh.material;


				ConstantBuffer<PS_TOON> cv;
				cv->lightDirection = Vec3::Down;//RandomVec3().normalize();
				cv->useToon = 0.0;



				if (material.toneTextureIndex != -1)
				{
					Texture texture;

					if (material.UTOON) texture = MMD::GetToonTexture(material.toneTextureIndex);
					else texture = textures[material.toneTextureIndex];

					cv->useToon = 1.0;
					Graphics3D::SetTextureForward(ShaderStage::Pixel, 1, texture);

				}







				if (material.textureIndex != -1)
				{
					Graphics3D::SetTextureForward(ShaderStage::Pixel, 0, textures[material.textureIndex]);
				}



				Graphics3D::SetConstantForward(ShaderStage::Pixel, 1, cv);


				Graphics3D::SetSamplerStateForward(SamplerState::Default3D);

				transformMesh(mesh.mesh).drawForward();

			}

			Graphics3D::SetRasterizerStateForward(RasterizerState::Predefined::Default3D);

			Graphics3D::EndVSForward();
			Graphics3D::EndPSForward();











		}


		void drawBoader(const VertexShader &vs, const PixelShader &ps) const
		{

			Graphics3D::BeginVSForward(vs);
			Graphics3D::BeginPSForward(ps);


			Graphics3D::SetRasterizerStateForward(RasterizerState(FillMode::Solid, CullMode::Front));

			for (auto &mesh : meshes)
			{

				transformMesh(mesh.mesh).drawForward();

			}
			Graphics3D::SetRasterizerStateForward(RasterizerState::Predefined::Default3D);

			Graphics3D::EndVSForward();
			Graphics3D::EndPSForward();




		}

		void drawShadow() const;


		void drawBone() const;

		Quaternion getRotate() const
		{
			return rotateQ * Quaternion::RollPitchYaw(rotate.z, rotate.x, rotate.y);
		}


		TransformedMesh transformMesh(const Mesh &mesh) const;


		// メッシュに変形後の頂点を設定する
		void updateMesh();


		void updateBoneLocalMatrix();


		double height;


		// 高さ（メートル）からモデルの scale を設定する
		void setHeight(const double _height)
		{
			auto _scale = _height / height;
			scale = Vec3(_scale, _scale, _scale);
		}


	};











}