

#include "Model.hpp"



namespace PMX
{


	void Model::draw() const
	{

		for (auto &mesh : meshList)
		{

			if (mesh.material.textureIndex == -1)
			{
				mesh.mesh.draw(mesh.material.diffuse);
				continue;
			}

			auto &texture = textures[mesh.material.textureIndex];

			mesh.mesh.draw(texture);

		}

	}

	void Model::drawForward() const
	{

		for (auto &mesh : meshList)
		{

			auto &texture = textures[mesh.material.textureIndex];

			mesh.mesh
				.scaled(scale)
				.rotated(rotate)
				.translated(translate)
				.drawForward(texture);

		}

	}









	void Model::drawShadow() const
	{

		for (auto &mesh : meshList)
		{

			mesh.mesh
				.scaled(scale)
				.rotated(rotate)
				.translated(translate)
				.drawShadow();

		}

	}

	void Model::drawBone() const
	{


		// ボーンを描画する
		for (auto &bone : bones)
		{

			// 非表示ボーン
			if (!bone.flag.visible) continue;


			const Vec2 position = Graphics3D::ToScreenPos(bone.transformedPosition).xy();




			// 接続先がない
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



}




