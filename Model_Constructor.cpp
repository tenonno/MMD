
#include "Model.hpp"





namespace PMX
{



	Model::Model(const FilePath &path)
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


		auto minY = DBL_MAX;
		auto maxY = -DBL_MAX;


		for (auto vertexIndex : step(vertexNum))
		{

			Vertex vertex;

			// ���_���W
			vertex.position = reader.value<Vec3>();

			minY = Min(minY, (double)vertex.position.y);
			maxY = Max(maxY, (double)vertex.position.y);

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

			vertices.emplace_back(vertex);

		}

		height = maxY - minY;

		// �ʂ��\�����Ă��钸�_�̐�
		const size_t faceVertexNum = reader.value<int32>();

		// �ʂ̐�
		const auto faceNum = faceVertexNum / 3;

		Println(L"�ʐ�", faceNum);


	


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

		materials.resize(materialNum);
		

		for (auto materialIndex : step(materialNum))
		{

			PMX::Material material;



			auto materialName = reader.value<String>();



			auto materialNameEng = reader.value<String>();
			Println(materialName, materialNameEng);


			auto diffuse = reader.value<Vec4>();

			// Diffuse
			material.diffuse = ColorF(diffuse.x, diffuse.y, diffuse.z, diffuse.w);

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

			materials[materialIndex] = material;

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
				for (auto ikLinkIndex : step(ikLinkNum))
				{

					IK_Link ikLink;

					// n: �{�[��Index�T�C�Y | �����N�{�[���̃{�[��Index
					auto ikLinkBoneIndex = reader.value(boneIndexSize);

					ikLink.boneIndex = ikLinkBoneIndex;

					// 1 : byte | �p�x���� 0 : OFF 1 : ON
					auto limitAngle = reader.value<bool>();

					ikLink.limit = limitAngle;

					// �p�x���� : 1�̏ꍇ
					if (limitAngle)
					{


						// $(L"�p�x����");

						// 12 : float3 | ����(x, y, z)->���W�A���p
						ikLink.min = reader.readVec3();

						// 12 : float3 | ���(x, y, z)->���W�A���p
						ikLink.max = reader.readVec3();

					}


					bone.ik.links.emplace_back(ikLink);

				}

			}



			Println(bone.name, bone.nameEng);

			bone.flag = flag;

			bones.emplace_back(bone);

			boneMap[bone.name] = &bones.back();


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




		struct A {

			uint64 indexIndex;

			bool none = false;

		};

		// �}�e���A�����烁�b�V�������
		for (auto &material : materials)
		{
			Array<MeshVertex> meshVertices;
			Array<uint32> indices;

			meshVertices.reserve(material.faceVertexCount);
			indices.reserve(material.faceVertexCount);

			PMXMesh pmxMesh;


			auto _indexIndex = 0;


			std::unordered_map<uint64, A> overlap;

			const auto find = [&](const uint64 vertexIndex)
			{

				A result;
				result.none = false;


				// �܂����_���o�^����Ă��Ȃ��Ȃ�
				if (!overlap.count(vertexIndex))
				{
					result.none = true;

					result.indexIndex = _indexIndex++;

				}
				else
				{

					result = overlap[vertexIndex];


					result.none = false;
				}

				overlap[vertexIndex] = result;

				return result;

			};

			// �ʂ̐�������
			for (auto i : step(material.faceCount))
			{

				// ��
				auto &face = faceList[faceIndex];


				A v1 = find(face.v1);
				A v2 = find(face.v2);
				A v3 = find(face.v3);



				if (v1.none) meshVertices.emplace_back(vertices[face.v1].toMeshVertex());
				if (v2.none) meshVertices.emplace_back(vertices[face.v2].toMeshVertex());
				if (v3.none) meshVertices.emplace_back(vertices[face.v3].toMeshVertex());


				if (v1.none) pmxMesh.v_index.emplace_back(face.v1);
				if (v2.none) pmxMesh.v_index.emplace_back(face.v2);
				if (v3.none) pmxMesh.v_index.emplace_back(face.v3);

				indices.push_back(v1.indexIndex);
				indices.push_back(v2.indexIndex);
				indices.push_back(v3.indexIndex);

				++faceIndex;
			}


			auto mesh = DynamicMesh(MeshData(meshVertices, indices));
			pmxMesh.mesh = mesh;
			pmxMesh.material = material;


			meshes.emplace_back(pmxMesh);


		}



		








		for (auto &bone : bones)
		{
			// ���ɑS�ĕ������Z�O�{�[���Ƃ���
			A$Physics_bones.emplace_back(&bone);

		}


		std::sort(A$Physics_bones.begin(), A$Physics_bones.end(),
			[](const Bone *a, const Bone *b)
		{

			if (a->�ό`�K�w == b->�ό`�K�w)
			{
				return a->index > b->index;
			}
			else
			{
				return a->�ό`�K�w > b->�ό`�K�w;
			}

		});






	}




}