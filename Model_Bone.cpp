
// ボーン関係の処理

#include "Model.hpp"

namespace PMX
{



	void Model::updateMesh()
	{

		for (auto &mesh : meshes)
		{

			Array<MeshVertex> meshVertices(mesh.mesh.num_vertices);


			auto i = 0;
			for (auto &vertex : meshVertices)
			{

				vertex.position = vertices[mesh.v_index[i]].transformedPosition;
				vertex.normal = vertices[mesh.v_index[i]].transformedNormal.normalized();
				vertex.texcoord = vertices[mesh.v_index[i]].texcoord;

				++i;
			}
			
			mesh.mesh.fillVertices(meshVertices);

		}
	}

	void Model::updateBoneLocalMatrix()
	{

		{
			while (true)
			{
				bool end = true;

				// ボーンのローカル移動量、回転量を計算する
				for (auto &p_bone : A$Physics_bones)
				{

					Bone &bone = *p_bone;



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
							$(L"ローカル付与");

							// rotate *= MatrixToQuaternion(付与_parent->transformParameter.localMatrix);

							// 1.1 ローカル付与の場合 : *付与親のローカル移動量 ※ローカル付与優先

							// TODO: localTranslate を削除して localMatrix の移動成分にする
							// translate += 付与_parent->transformParameter.localTranslate;

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


							if (bone.flag.移動付与)
							{
								translate += 付与_parent->transformParameter._f_translate;
							}

						}
					}


					//  1.2 付与親がIKリンクの場合(且つローカル付与ではない場合) : * 付与親のIKリンク回転量
					// ??????????
					if (付与_parent != nullptr)
					{
						rotate *= 付与_parent->transformParameter._ik_r;
					}


					// 1.3 付与率が1以外の場合 : 回転量を付与率で補正(Quaternion.Slerp()想定)
					// 1.2 付与率が1以外の場合 : 移動量を付与率で補正(付与率乗算でOK)

					rotate = Math::Slerp(Quaternion::Identity(), rotate, bone.付与率);

					translate *= bone.付与率;



					rotate *= bone.transformParameter.keyframeRotate;
					translate += bone.transformParameter.keyframeTranslate;

					rotate *= bone.transformParameter._ik_r;




					bone.transformParameter._f_rotate = rotate;



					bone.transformParameter.付与回転計算済 = true;




					// 2. 当該ボーンの回転量追加 : * 回転 * 回転モーフ
					//rotate *= bone.transformParameter._morph_rotate;

					bone.transformParameter.rotate = rotate;


					// 2. 当該ボーンの移動量追加 : +移動 + 移動モーフ
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



	}



	void Model::updateMotion()
	{


		// 付与変形 1
		updateBoneLocalMatrix();


		PMX::Bone rootBone;
		rootBone.transformParameter.translate = Vec3::Zero;
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




			}


			// 全てのボーンの変形終了
			if (end) break;

		}




		// IK 計算
		// http://d.hatena.ne.jp/edvakf/20111102/1320268602
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



					//  リンクボーンからターゲットボーンへの向き
					auto tDir = targetBone.getGPos(bones) - linkBone.getGPos(bones);



					// リンクボーンから IK ボーン への向き
					auto lDir = ikBone.getGPos(bones) - linkBone.getGPos(bones);


					// (1)を(2)の位置に向ける回転行列の計算
					float rotationDotProduct = Vec3(tDir.normalized()).dot(lDir.normalized());


					if (rotationDotProduct < 0.000001) continue;

					double rotationAngle = Acos(rotationDotProduct);

					/*
					if (Abs(rotationAngle) > bone.ik.limitAngle)
					{
					rotationAngle = Sign(rotationAngle) * bone.ik.limitAngle;
					}
					*/

					// 制限角度を超えている
					if (rotationAngle > bone.ik.limitAngle) rotationAngle = bone.ik.limitAngle;


					// 外積が回転軸
					auto rotationAxis = Vec3(tDir).cross(lDir);
					rotationAxis.normalize();
					// 回転軸と回転角度からクォータニオンを生成
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



		// IK 計算後付与率を再計算
		updateBoneLocalMatrix();





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


		// 頂点をボーンに合わせて変形する
		for (auto &vertex : vertices)
		{


			if (vertex.weightType == WeightType::BDEF1)
			{

				auto &bone1 = bones[vertex.boneIndex1];

				auto lp = vertex.position;

				vertex.transformedPosition = bone1.animationMatrix.transform(lp);
				vertex.transformedNormal = bone1.transformParameter.localMatrix.transform(vertex.normal);
			}

			else if (vertex.weightType == WeightType::BDEF2)
			{

				auto &bone1 = bones[vertex.boneIndex1];
				auto &bone2 = bones[vertex.boneIndex2];

				auto lp1 = vertex.position;
				auto lp2 = vertex.position;


				auto v1 = bone1.animationMatrix.transform(lp1);
				auto v2 = bone2.animationMatrix.transform(lp2);

				auto n1 = bone1.transformParameter.localMatrix.transform(vertex.normal);
				auto n2 = bone2.transformParameter.localMatrix.transform(vertex.normal);

				vertex.transformedPosition = Math::Lerp(v2, v1, vertex.boneWeight1);
				vertex.transformedNormal = Math::Lerp(n2, n1, vertex.boneWeight1);

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

				auto n1 = bone1.transformParameter.localMatrix.transform(vertex.normal);
				auto n2 = bone2.transformParameter.localMatrix.transform(vertex.normal);
				auto n3 = bone3.transformParameter.localMatrix.transform(vertex.normal);
				auto n4 = bone4.transformParameter.localMatrix.transform(vertex.normal);



				// ウェイトの合計
				auto aw = (vertex.boneWeight1 + vertex.boneWeight2 + vertex.boneWeight3 + vertex.boneWeight4);



				// 正規化する
				// TODO: モデル読み込み時に正規化するように
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

				vertex.transformedNormal = (
					n1 * w1 +
					n1 * w2 +
					n1 * w3 +
					n1 * w4
					);

			}

			else
			{
				$((int)vertex.weightType);
			}


		}


		updateMesh();
	}

	void Model::updateVertex()
	{


		// 頂点をボーンに合わせて変形する
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



}
