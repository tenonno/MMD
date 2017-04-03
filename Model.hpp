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
			// ����
			header.type[4] = 0;

			Println(WidenAscii((char *)header.type));

			reader.read(&header.version, 4);
			reader.read(&header.dataSize, 1);




			Println(header.version);

			Println(L"�f�[�^�T�C�Y", header.dataSize);


			header.datas.resize((size_t)header.dataSize);


			// 8 ��̃f�[�^��ǂݍ���
			for (auto i = 0; i < header.dataSize; i++)
			{

				reader.read(&header.datas[i], 1);

				// Println(L"�f�[�^", i, L": ", header.datas[i]);

			}


			// �G���R�[�h
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

			Println(L"���_��", vertexNum);


			const size_t optionalUVSize = header.datas[1];


			auto vertexIndexSize = header.datas[2];
			auto textureIndexSize = header.datas[3];
			auto materialIndexSize = header.datas[3];
			auto boneIndexSize = header.datas[5];

			for (auto vertexIndex : step(vertexNum))
			{

				Vertex vertex;

				// ���_���W
				vertex.position = reader.value<Vec3>();

				// �@��
				vertex.normal = reader.value<Vec3>();

				// UV
				vertex.texcoord = reader.value<Vec2>();


				// 4 �ȏ�̒ǉ� UV ���������Ă���ꍇ
				// PMX �̎d�l�ł͏�� 4
				if (optionalUVSize > 4)
				{
					MessageBox::Show(L"�ǉ� UV ���̐����ُ�ł�");
				}


				// �ǉ� UV
				for (auto i : step(optionalUVSize))
				{

					vertex.optionalUV[i] = reader.value<Vec4>();

				}

				uint8 wType;
				reader.read(&wType, 1);

				auto WT = (WeightType)wType;


				vertex.weightType = WT;


				// �{�[���̓X�L�b�v����

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



			// �ʂ��\�����Ă��钸�_�̐�
			const size_t faceVertexNum = reader.value<int32>();

			// �ʂ̐�
			const auto faceNum = faceVertexNum / 3;

			Println(L"�ʐ�", faceNum);


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

			Println(L"�e�N�X�`����", textureNum);


			for (auto textureIndex : step(textureNum))
			{

				const auto name = reader.value<String>();

				auto textureName = directory + name;

				Texture texture(textureName, TextureDesc::For3D);

				Println(textureIndex, L": ", textureName);

				textures.emplace_back(texture);

			}




			const auto materialNum = reader.value<int32>();


			Println(L"�ގ���", materialNum);

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

				// Specular �W��
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

				// �X�t�B�A���[�h 0:���� 1:��Z(sph) 2:���Z(spa) 3:�T�u�e�N�X�`��(�ǉ�UV1��x,y��UV�Q�Ƃ��Ēʏ�e�N�X�`���`����s��)
				reader.value<uint8>();

				material.UTOON = reader.value<bool>();


				// ���L�g�[���e�N�X�`�����g���Ȃ�
				// ���L�g�[���e�N�X�`���� 0 ~ 9 �̃C���f�b�N�X
				if (material.UTOON)
				{

					material.toneTextureIndex = reader.value<uint8>();

				}
				// �I���W�i���̃g�[���e�N�X�`�����g��
				else
				{

					material.toneTextureIndex = reader.value(textureIndexSize);

				}

				auto text = reader.value<String>();


				// �ʂ��\�����Ă��钸�_�̐�
				material.faceVertexCount = reader.value<int>();

				// �ʂ̐�
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

				bone.�ό`�K�w = reader.value<int>();



				BoneFlag flag;

				auto flags = reader.value<int16>();

				flag.connectType = (BoneConnectType)(flags & 0x0001);


				flag.��]�\ = flags & 0x0002;
				flag.�ړ��\ = flags & 0x0004;
				flag.visible = flags & 0x0008;
				flag.����� = flags & 0x0010;
				flag.IK = flags & 0x0020;
				flag.���[�J���t�^ = flags & 0x0080;
				flag.��]�t�^ = flags & 0x0100;
				flag.�ړ��t�^ = flags & 0x0200;
				flag.���Œ� = flags & 0x0400;
				flag.���[�J���� = flags & 0x0800;
				flag.������ό` = flags & 0x1000;
				flag.�O���e�ό` = flags & 0x2000;


				/*
				�ڑ���:0 �̏ꍇ
				12 : float3	| ���W�I�t�Z�b�g, �{�[���ʒu����̑��Ε�

				�ڑ���:1 �̏ꍇ
				n  : �{�[��Index�T�C�Y  | �ڑ���{�[���̃{�[��Index
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
				��]�t�^:1 �܂��� �ړ��t�^:1 �̏ꍇ
				n  : �{�[��Index�T�C�Y  | �t�^�e�{�[���̃{�[��Index
				4  : float	| �t�^��
				*/

				if (flag.��]�t�^ || flag.�ړ��t�^)
				{

					bone.�t�^_parentBoneIndex = reader.value(boneIndexSize);
					bone.�t�^�� = reader.value<double>();




				}

				/*
				���Œ�:1 �̏ꍇ
				12 : float3 | ���̕����x�N�g��
				*/
				if (flag.���Œ�)
				{
					auto ���̕����x�N�g�� = reader.readVec3();
				}

				/*
				���[�J����:1 �̏ꍇ
				12 : float3	| X���̕����x�N�g��
				12 : float3	| Z���̕����x�N�g�� ���t���[�����Z�o���@�͌�q
				*/
				if (flag.���[�J����)
				{
					// ���̏��͕K�v�Ȃ�
					/* auto X���̕����x�N�g�� = */ reader.value<Vec3>();
					/* auto Z���̕����x�N�g�� = */ reader.value<Vec3>();
				}

				/*
				�O���e�ό`:1 �̏ꍇ
				*/
				if (flag.�O���e�ό`)
				{
					auto KEY�l = reader.readInt();
				}

				/*
				IK:1 �̏ꍇ IK�f�[�^���i�[
				*/
				if (flag.IK)
				{


					// n: �{�[��Index�T�C�Y | IK�^�[�Q�b�g�{�[���̃{�[��Index
					bone.ik.targetIndex = reader.value(boneIndexSize);

					// 4 : int | IK���[�v��(PMD�y��MMD���ł�255�񂪍ő�ɂȂ�悤�ł�)
					bone.ik.iteration = reader.value<uint32>();

					// 4 : float | IK���[�v�v�Z����1�񂠂���̐����p�x->���W�A���p | PMD��IK�l�Ƃ�4�{�قȂ�̂Œ���
					bone.ik.limitAngle = reader.value<double>();

					// 4  : int  	| IK�����N�� : �㑱�̗v�f��
					auto ikLinkNum = reader.readInt();

					// <IK�����N>
					for (auto ikLinkIndex : step(ikLinkNum ))
					{

						// n: �{�[��Index�T�C�Y | �����N�{�[���̃{�[��Index
						auto ikLinkBoneIndex = reader.value(boneIndexSize);


						bone.ik.links.emplace_back(ikLinkBoneIndex);

						// 1 : byte | �p�x���� 0 : OFF 1 : ON
						auto limitAngle = reader.value<bool>();

						bone.ik.limits.emplace_back(limitAngle ? 1 : 0);

						// �p�x���� : 1�̏ꍇ
						if (limitAngle)
						{


						
							// $(L"�p�x����");

							// 12 : float3 | ����(x, y, z)->���W�A���p
							auto min = reader.readVec3();

							// 12 : float3 | ���(x, y, z)->���W�A���p
							auto max = reader.readVec3();

						}

					}

				}



				Println(bone.name, bone.nameEng);

				bone.flag = flag;

				bones.emplace_back(bone);

			}


			// �{�[���� BOf �s������
			for (auto &bone : bones)
			{



				Mat4x4 �����p���s��;


				if (!$boneNoneConnect(bone))
				{

					// �ڑ�����W
					auto connectPos = $getBoneConnectPosition(bone);

					// �{�[������ڑ���Ɍ�������]
					auto rotate = Quaternion(bone.position, connectPos).toMatrix();

					�����p���s�� = rotate * Mat4x4::Translate(bone.position);

				}
				// �ڑ��悪�Ȃ��Ȃ�
				else
				{
					�����p���s�� = Mat4x4::Translate(bone.position);
				}

				bone.BOfMatrix = �����p���s��.inverse();



			}


			auto ���[�t�� = reader.value<int32>();


			auto faceIndex = 0;



			// �}�e���A�����烁�b�V�������
			for (auto &material : materialList)
			{
				Array<MeshVertex> vertices;
				Array<uint32> indices;

				vertices.reserve(material.faceVertexCount);
				indices.reserve(material.faceVertexCount);

				PMXMesh pmxMesh;


				// �ʂ̐�������
				for (auto i : step(material.faceCount))
				{

					// ��
					auto face = faceList[faceIndex];

					// TODO: �������_�����ɒǉ�����Ă����炻����Q�Ƃ���悤��

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





		void updateBonePhase1() const
		{

		}



		void transform() 
		{


			// ���_���{�[���ɍ��킹�ĕό`����
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

		void drawShadow() const;


		void drawBone() const;



	};











}