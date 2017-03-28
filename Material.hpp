#pragma once

namespace PMX
{


	struct Material
	{


		Vec4 diffuse;
		Vec3 specular;
		double Specular_K;
		Vec3 ambient;

		uint8 bitFlag;


		Vec4 edgeColor;

		double edgeSize;

		int64 textureIndex;
		int64 sTextureIndex;

		int64 toneTextureIndex;

		// ���L�g�[��
		bool UTOON;

		// �ʐ�
		int faceCount;

		// �ʒ��_�� ( faceCount * 3 )
		int faceVertexCount;


	};

}