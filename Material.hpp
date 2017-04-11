#pragma once

namespace PMX
{


	struct Material
	{

		ColorF diffuse;

		Vec3 specular;
		double Specular_K;
		Vec3 ambient;

		uint8 bitFlag;


		Vec4 edgeColor;

		double edgeSize;

		int64 textureIndex;
		int64 sTextureIndex;

		int64 toneTextureIndex = -1;

		// 共有トーン
		bool UTOON;

		// 面数
		int faceCount;

		// 面頂点数 ( faceCount * 3 )
		int faceVertexCount;


	};

}
