

#include "Model.hpp"



namespace PMX
{


	void Model::draw() const
	{

		for (auto &mesh : meshes)
		{
			// �e�N�X�`�������݂��Ȃ��ꍇ�� diffuse �ŕ`��
			// TODO: MMD ���f���p�̃V�F�[�_������
			if (mesh.material.textureIndex == -1)
			{
				transformMesh(mesh.mesh).draw(mesh.material.diffuse);
				continue;
			}

			auto &texture = textures[mesh.material.textureIndex];

			transformMesh(mesh.mesh).draw(texture);

		}

	}



	void Model::drawForward() const
	{

		for (auto &mesh : meshes)
		{

			auto &texture = textures[mesh.material.textureIndex];

			transformMesh(mesh.mesh).drawForward(texture);

		}

	}









	void Model::drawShadow() const
	{

		for (auto &mesh : meshes)
		{
			transformMesh(mesh.mesh).drawShadow();

		}

	}

	void Model::drawBone() const
	{


		// �{�[����`�悷��
		for (auto &bone : bones)
		{

			// ��\���{�[��
			if (!bone.flag.visible) continue;


			const Vec2 position = Graphics3D::ToScreenPos(
				transformVertex(bone.transformedPosition)
			).xy();



			// �ڑ��悪�Ȃ�
			if ($boneNoneConnect(bone)) continue;


			const Vec2 connectPosition = Graphics3D::ToScreenPos(
				$getBoneConnectPosition(bone)
			).xy();


			if (bone.flag.IK)
			{
				Circle(position, 10.0).drawFrame(1.0, 0.0, ColorF(0, 1, 1));

				/*

				DrawConnectLine(position, Graphics3D::ToScreenPos(
				bones[bone.ik.targetIndex].transformedPosition
				).xy(), 5.0, ColorF(1, 0, 0));


				for (auto link : bone.ik.links)
				{

				DrawConnectLine(position, Graphics3D::ToScreenPos(
				bones[link].transformedPosition
				).xy(), 5.0, ColorF(1, 0, 0));

				}

				*/
			}

			const ColorF color(0.0, 0.8, 0.0);




			DrawConnectLine(position, connectPosition, 5.0, color);


		}







	}

	TransformedMesh Model::transformMesh(const Mesh & mesh) const
	{
		return mesh
			.scaled(scale)
			.rotated(getRotate())
			.translated(position);
	}

	



}




