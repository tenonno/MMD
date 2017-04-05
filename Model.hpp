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

	Array<MeshVertex> vertices;
	Array<int64> v_index;

};


namespace PMX
{


	class Model
	{


	public:

		Model::Model(const FilePath &path);

		Array<PMXMesh> meshList;

		Vec3 translate = Vec3::Zero;
		Vec3 scale = Vec3::One;
		Vec3 rotate = Vec3::Zero;

		Array<Material> materials;
		Array<Bone> bones;

		Array<Texture> textures;
		Array<Vertex> vertices;

		Array<Face> faceList;


		Array<Triangle3D> getT3D()
		{
			Array<Triangle3D> ts;


			for (auto &face : faceList)
			{

				Triangle3D tr(

					vertices[face.v1].position,
					vertices[face.v2].position,
					vertices[face.v3].position
						
						);

				ts.emplace_back(tr);

			}

			return ts;

		}

		// �{�[���̐ڑ��悪�Ȃ��Ȃ� true
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


		void updateBonePhase1() const
		{

		}

		// �������Z�O�ό`�{�[��
		Array<Bone *> A$Physics_bones;


		void updateMotion()
		{


			// �t�^�ό` 1
			updateBoneLocalMatrix(A$Physics_bones);


			PMX::Bone rootBone;
			rootBone.transformParameter.translate = Vec3::Zero;
			rootBone.transformParameter.transformed = true;
			rootBone.BOfMatrix = Mat4x4::Identity();
			rootBone.animationMatrix = Mat4x4::Identity();







			// �e -> �q�̏��ŕό`����悤�Ƀ��[�v
			while (true)
			{
				// �S�Ẵ{�[���̕ό`���I�������
				bool end = true;

				for (auto &p_bone : A$Physics_bones)
				{

					Bone &bone = *p_bone;

					if (bone.transformParameter.transformed) continue;



					Bone &parent = bone.parentBoneIndex == -1 ? rootBone : bones[bone.parentBoneIndex];

					// �e�̕ό`���I����Ă��Ȃ��Ȃ璆�f
					if (!parent.transformParameter.transformed) {
						end = false;
						continue;
					}



					bone.transformParameter.transformed = true;




					// ��
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


			

				}


				// �S�Ẵ{�[���̕ό`�I��
				if (end) break;

			}




			// IK �v�Z�i���j
			// �{���͕t�^�̑O�ɍs��

			// http://d.hatena.ne.jp/edvakf/20111102/1320268602


			ClearPrint();

			for (auto &bone : bones)
			{

				if (!bone.flag.IK) continue;


				Bone &ikBone = bone;

				Bone &targetBone = bones[bone.ik.targetIndex];




				for (auto _ : step(bone.ik.iteration))
				{

					auto i = 0;

					for (auto &ik_link : bone.ik.links)
					{
						++i;

						auto &linkBone = bones[ik_link.boneIndex];



						//  �����N�{�[������^�[�Q�b�g�{�[���ւ̌���
						auto tDir = targetBone.getGPos(bones) - linkBone.getGPos(bones);



						// �����N�{�[������ IK �{�[�� �ւ̌���
						auto lDir = ikBone.getGPos(bones) - linkBone.getGPos(bones);


						// (1)��(2)�̈ʒu�Ɍ������]�s��̌v�Z
						float rotationDotProduct = Vec3(tDir.normalized()).dot(lDir.normalized());


						if (rotationDotProduct < 0.000001) continue;

						double rotationAngle = Acos(rotationDotProduct);

						/*
						if (Abs(rotationAngle) > bone.ik.limitAngle)
						{
						rotationAngle = Sign(rotationAngle) * bone.ik.limitAngle;
						}
						*/

						// �����p�x�𒴂��Ă���
						if (rotationAngle > bone.ik.limitAngle) rotationAngle = bone.ik.limitAngle;


						// �O�ς���]��
						auto rotationAxis = Vec3(tDir).cross(lDir);
						rotationAxis.normalize();
						// ��]���Ɖ�]�p�x����N�H�[�^�j�I���𐶐�
						auto q2 = Quaternion(rotationAxis, rotationAngle);



						auto min = Vec3(-180_deg, -180_deg, -180_deg);
						auto max = Vec3(+180_deg, +180_deg, +180_deg);

						if (ik_link.limit)
						{
							min = ik_link.min;
							max = ik_link.max;

						}




						/*

						if (bone.ik.limits[i - 1])
						// if (limits[j])
						{

						q2 = QtoEtoQ(q2, min, max);

						}
						*/


						q2 = QtoEtoQ(q2, min, max);


						linkBone.transformParameter.localMatrix *= q2.toMatrix();// Mat4x4::Rotate(0.005, 0, 0);

						linkBone.transformParameter.rotate *= q2;

						linkBone.transformParameter._ik_r *= q2;


					}




				}

			}



			// IK �v�Z��t�^�����Čv�Z
			updateBoneLocalMatrix(A$Physics_bones);





			rootBone.transformParameter.___a___ = true;

			while (true)
			{
				bool end = true;

				for (auto &bone : bones)
				{

					Bone &parent = bone.parentBoneIndex == -1 ? rootBone : bones[bone.parentBoneIndex];

					if (!parent.transformParameter.___a___) {
						end = false;
						continue;
					}

					bone.animationMatrix = Mat4x4(


						bone.BOfMatrix *

						bone.transformParameter.localMatrix *

						bone.BOfMatrix.inverse()

						*
						parent.animationMatrix


					);

					bone.transformParameter.___a___ = true;

				}

				if (end) break;
			}


			for (auto &bone : bones)
			{
				bone.transformedPosition = bone.animationMatrix.transform(bone.position);
			}


			// ���_���{�[���ɍ��킹�ĕό`����
			for (auto &vertex : vertices)
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



					// �E�F�C�g�̍��v
					auto aw = (vertex.boneWeight1 + vertex.boneWeight2 + vertex.boneWeight3 + vertex.boneWeight4);



					// ���K������i���j
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
					$((int)vertex.weightType);
				}


			}


			updateMesh();
		}


		void transform() 
		{


			// ���_���{�[���ɍ��킹�ĕό`����
			for (auto &vertex : vertices)
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


					// �E�F�C�g�̍��v
					auto aw = (vertex.boneWeight1 + vertex.boneWeight2 + vertex.boneWeight3 + vertex.boneWeight4);



					// ���K������i���j
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
		void drawForward() const;


		void drawShadow() const;


		void drawBone() const;






		void updateMesh()
		{

			for (auto &mesh : meshList)
			{

				auto i = 0;

				for (auto &vertex : mesh.vertices)
				{
					vertex.position = vertices[mesh.v_index[i++]].transformedPosition;
				}

				mesh.mesh.fillVertices(mesh.vertices);

			}
		}




		void updateBoneLocalMatrix(const Array<Bone *> &A$Physics_bones)
		{
			while (true)
			{
				bool end = true;

				// �{�[���̃��[�J���ړ��ʁA��]�ʂ��v�Z����
				for (auto &p_bone : A$Physics_bones)
				{

					Bone &bone = *p_bone;



					Bone *�t�^_parent = nullptr;


					if (bone.�t�^_parentBoneIndex != -1)
					{
						�t�^_parent = &bones[bone.�t�^_parentBoneIndex];
					}


					// !!!!!!!!!!!!!!!!!!!!!



					// 0. ��]�ʂ�P�ʉ�]�ʂƂ��ĊJ�n
					auto rotate = Quaternion::Identity();
					// 0. �ړ��ʂ�0�ړ��ʂƂ��ĊJ�n
					auto translate = Vec3::Zero;



					// �t�^�𖳎��i�e�X�g�j

					// 1. ���Y�{�[�����t�^�̏ꍇ :
					if (�t�^_parent)
					{

						if (!�t�^_parent->transformParameter.�t�^��]�v�Z��)
						{
							end = false;
							continue;
						}

						//  1.1 ���[�J���t�^�̏ꍇ : * �t�^�e�̃��[�J���ό`��(�s��)�̉�]���� �����[�J���t�^�D��
						if (bone.flag.���[�J���t�^)
						{
							$(L"���[�J���t�^");

							// rotate *= MatrixToQuaternion(�t�^_parent->transformParameter.localMatrix);

							// 1.1 ���[�J���t�^�̏ꍇ : *�t�^�e�̃��[�J���ړ��� �����[�J���t�^�D��

							// TODO: localTranslate ���폜���� localMatrix �̈ړ������ɂ���
							// translate += �t�^_parent->transformParameter.localTranslate;

							// �t�^�e�̃��[�J���ړ���:�{�[���ړ��ʂƂ��Čv�Z ���b��Ή�
							// �{�[���ʒu(= ���[�J���s���4�s1 / 2 / 3��v�f) - �����{�[���ʒu


						}
						else
						{

							// <> �t�^�e���t�^�{�[���̏ꍇ : *�t�^�e�̕t�^��]��(����q)
							// ??????????
							if (bone.flag.��]�t�^)
							{

								rotate *= �t�^_parent->transformParameter._f_rotate;

							}


							if (bone.flag.�ړ��t�^)
							{
								translate += �t�^_parent->transformParameter._f_translate;
							}

						}
					}


					//  1.2 �t�^�e��IK�����N�̏ꍇ(�����[�J���t�^�ł͂Ȃ��ꍇ) : * �t�^�e��IK�����N��]��
					// ??????????
					if (�t�^_parent != nullptr)
					{
						rotate *= �t�^_parent->transformParameter._ik_r;
					}


					// 1.3 �t�^����1�ȊO�̏ꍇ : ��]�ʂ�t�^���ŕ␳(Quaternion.Slerp()�z��)
					// 1.2 �t�^����1�ȊO�̏ꍇ : �ړ��ʂ�t�^���ŕ␳(�t�^����Z��OK)

					rotate = Math::Slerp(Quaternion::Identity(), rotate, bone.�t�^��);

					translate *= bone.�t�^��;



					rotate *= bone.transformParameter.keyframeRotate;
					translate += bone.transformParameter.keyframeTranslate;

					rotate *= bone.transformParameter._ik_r;




					bone.transformParameter._f_rotate = rotate;



					bone.transformParameter.�t�^��]�v�Z�� = true;




					// 2. ���Y�{�[���̉�]�ʒǉ� : * ��] * ��]���[�t
					//rotate *= bone.transformParameter._morph_rotate;

					bone.transformParameter.rotate = rotate;


					// 2. ���Y�{�[���̈ړ��ʒǉ� : +�ړ� + �ړ����[�t
					bone.transformParameter.translate += translate + bone.transformParameter._morph_translate;


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
		}







	};











}