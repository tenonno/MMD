#pragma once

enum class WeightType
{

	BDEF1 = 0,
	BDEF2 = 1,
	BDEF4 = 2,
	SDEF = 3

};

struct Vertex
{
	Float3 position;
	Float3 normal;
	Float2 texcoord;

	// ボーン変形後の座標
	Vec3 transformedPosition;

	WeightType weightType;

	int64 boneIndex1;
	int64 boneIndex2;
	int64 boneIndex3;
	int64 boneIndex4;

	double boneWeight1;
	double boneWeight2;
	double boneWeight3;
	double boneWeight4;


	float edgeScale;

	std::array<Float4, 4> optionalUV;

	MeshVertex toMeshVertex() {

		MeshVertex vertex;

		vertex.position.set(this->position);
		vertex.normal.set(this->normal);
		vertex.texcoord.set(this->texcoord);

		return vertex;

	}

};

