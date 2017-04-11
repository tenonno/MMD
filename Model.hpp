#pragma once


#include <Siv3D.hpp>


#include "Utils.hpp"
# include "Vertex.hpp"
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


namespace PMX
{

	class Model
	{

	public:

		Model::Model(const FilePath &path);

		Array<PMXMesh> meshList;
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

		// ���f���̎O�p�ʂ�S�Ď擾����
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




		// �{�[���̐ڑ��悪�Ȃ��Ȃ� true
		bool $boneNoneConnect(const Bone &bone) const
		{
			return bone.flag.connectType == BoneConnectType::Index && bone.connectBoneIndex == -1;
		};

		// �{�[���̐ڑ�����W���擾����
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




		// �������Z�O�ό`�{�[��
		Array<Bone *> A$Physics_bones;

		// �������Z��ό`�{�[���i���j
		Array<Bone *> B$Physics_bones;


		// ���[�V������K�p���ă{�[����ό`����
		void updateMotion();

		// �{�[���̕ό`�ɍ��킹�Ē��_��ό`����
		void updateVertex();

		void draw() const;
		void drawForward() const;


		void drawBoader(const VertexShader &vs, const PixelShader &ps) const
		{

			Graphics3D::BeginVSForward(vs);
			Graphics3D::BeginPSForward(ps);

			Graphics3D::SetRasterizerStateForward(RasterizerState(FillMode::Solid, CullMode::Front));

			for (auto &mesh : meshList)
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


		// ���b�V���ɕό`��̒��_��ݒ肷��
		void updateMesh();


		void updateBoneLocalMatrix();


		double height;


		// �����i���[�g���j���烂�f���� scale ��ݒ肷��
		void setHeight(const double _height)
		{
			auto _scale = _height / height;
			scale = Vec3(_scale, _scale, _scale);
		}


	};











}