#pragma once




///////////////////////////////////////////////
// ��]�s�񁨃N�H�[�^�j�I���ϊ�
//
// qx, qy, qz, qw : �N�H�[�^�j�I�������i�o�́j
// m1[3] : ��]�s�񐬕� m11 - m13
// m2[3] : ��]�s�񐬕� m21 - m23
// m3[3] : ��]�s�񐬕� m31 - m33
//
// �����ӁF
// �s�񐬕���DirectX�`���i�s���������̌����j�ł�
// OpenGL�`���i����������̌����j�̏ꍇ��
// �]�u�����l�����ĉ������B

///////////////////////////////////////////////
// ��]�s�񁨃N�H�[�^�j�I���ϊ�
//
// qx, qy, qz, qw : �N�H�[�^�j�I�������i�o�́j
// m11-m33 : ��]�s�񐬕�
//
// �����ӁF
// �s�񐬕���DirectX�`���i�s���������̌����j�ł�
// OpenGL�`���i����������̌����j�̏ꍇ��
// �]�u�����l�����ĉ������B

bool transformRotMatToQuaternion(
	float &qx, float &qy, float &qz, float &qw,
	float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33
) {
	// �ő听��������
	float elem[4]; // 0:x, 1:y, 2:z, 3:w
	elem[0] = m11 - m22 - m33 + 1.0f;
	elem[1] = -m11 + m22 - m33 + 1.0f;
	elem[2] = -m11 - m22 + m33 + 1.0f;
	elem[3] = m11 + m22 + m33 + 1.0f;

	unsigned biggestIndex = 0;
	for (int i = 1; i < 4; i++) {
		if (elem[i] > elem[biggestIndex])
			biggestIndex = i;
	}

	if (elem[biggestIndex] < 0.0f)
		return false; // �����̍s��ɊԈႢ����I

					  // �ő�v�f�̒l���Z�o
	float *q[4] = { &qx, &qy, &qz, &qw };
	float v = sqrtf(elem[biggestIndex]) * 0.5f;
	*q[biggestIndex] = v;
	float mult = 0.25f / v;

	switch (biggestIndex) {
	case 0: // x
		*q[1] = (m12 + m21) * mult;
		*q[2] = (m31 + m13) * mult;
		*q[3] = (m23 - m32) * mult;
		break;
	case 1: // y
		*q[0] = (m12 + m21) * mult;
		*q[2] = (m23 + m32) * mult;
		*q[3] = (m31 - m13) * mult;
		break;
	case 2: // z
		*q[0] = (m31 + m13) * mult;
		*q[1] = (m23 + m32) * mult;
		*q[3] = (m12 - m21) * mult;
		break;
	case 3: // w
		*q[0] = (m23 - m32) * mult;
		*q[1] = (m31 - m13) * mult;
		*q[2] = (m12 - m21) * mult;
		break;
	}

	return true;
}

///////////////////////////////////////////////
// ��]�s�񁨃N�H�[�^�j�I���ϊ�
//
// qx, qy, qz, qw : �N�H�[�^�j�I�������i�o�́j
// m1[3] : ��]�s�񐬕� m11 - m13
// m2[3] : ��]�s�񐬕� m21 - m23
// m3[3] : ��]�s�񐬕� m31 - m33
//
// �����ӁF
// �s�񐬕���DirectX�`���i�s���������̌����j�ł�
// OpenGL�`���i����������̌����j�̏ꍇ��
// �]�u�����l�����ĉ������B

bool transformRotMatToQuaternion(
	float q[4],
	const float m1[3],
	const float m2[3],
	const float m3[3]
) {
	return transformRotMatToQuaternion(
		q[0], q[1], q[2], q[3],
		m1[0], m1[1], m1[2],
		m2[0], m2[1], m2[2],
		m3[0], m3[1], m3[2]
	);
}


Quaternion MatrixToQuaternion(const Mat4x4 &matrix)
{
	float q[4];

	const float m1[3] = { (float)matrix.r[0].m128_u32[0], (float)matrix.r[0].m128_u32[1], (float)matrix.r[0].m128_u32[2] };
	const float m2[3] = { (float)matrix.r[1].m128_u32[0], (float)matrix.r[1].m128_u32[1], (float)matrix.r[1].m128_u32[2] };
	const float m3[3] = { (float)matrix.r[2].m128_u32[0], (float)matrix.r[2].m128_u32[1], (float)matrix.r[2].m128_u32[2] };


	const auto result = transformRotMatToQuaternion(q, m1, m2, m3);

	if (!result) throw L"aaaaa";

	return Quaternion(q[0], q[1], q[2], q[3]);

}









namespace PMX
{



	enum class BoneConnectType
	{

		Position = 0,
		Index = 1,

	};

	struct BoneFlag
	{

		// �ڑ���(PMD�q�{�[���w��)�\�����@ -> 0:���W�I�t�Z�b�g�Ŏw�� 1 : �{�[���Ŏw��

		BoneConnectType connectType;

		// 0x0100  : ��]�t�^
		bool ��]�t�^;

		// 0x0200 : �ړ��t�^
		bool �ړ��t�^;

		bool ���Œ�;


		// VMD �ɂ̓O���[�o������̐��l�������Ă���̂Ŏg��Ȃ�
		// �G�f�B�^�ŃA�j���[�V������ҏW���鎞�ɕ֗��炵��
		bool ���[�J����;

		bool �O���e�ό`;


		bool ��]�\;
		bool �ړ��\;
		bool visible;
		bool �����;
		bool IK;

		bool ���[�J���t�^;
		bool ������ό`;


	};


	struct BoneTransformParameter
	{

		bool �t�^���v�Z�� = false;

		bool bbbb = false;

		// VMD �̃L�[�t���[���ɂ��ړ���
		Float3 keyframeTranslate;
		// VMD �̃L�[�t���[���ɂ���]��
		Quaternion keyframeRotate;

		// ���[�U�[����ɂ�� ��]�^�ړ��� | PMD / PMX�G�f�B�^���ł̓X�P�[���l������
		Vec3 translate;
		Quaternion rotate;

		// �{�[�����[�t�ɂ�� ��]�^�ړ���
		Float3 _morph_translate;
		Quaternion _morph_rotate;

		// �t�^����̏ꍇ     �t�^��]�^�t�^�ړ��� | �ۑ����Ă������Ƃő��d�t�^���\�ɂȂ�
		Float3 _f_translate;
		Quaternion _f_rotate;

		// IK�����N�̏ꍇ     IK��]��
		Quaternion _ik_r;


		// �{�[���ό`�����ɂ�郍�[�J���ό`���
		Mat4x4 localMatrix;

		Float3 localTranslate;
		
		// ���_�Ȃǌ`��ό`�p�̃O���[�o���ό`���
		Mat4x4 globalMatrix;
		Quaternion vvv;

		Quaternion globalRotate;
		Vec3 globalTranslate;


		bool transformed = false;

		void reset()
		{
			globalTranslate = Vec3::Zero;
			globalRotate = Quaternion::Identity();

			keyframeTranslate = Vec3::Zero;
			keyframeRotate = Quaternion::Identity();

			�t�^���v�Z�� = false;
			transformed = false;
			bbbb = false;
			translate = Vec3::Zero;
			rotate = Quaternion::Identity();
			_morph_translate = Vec3::Zero;
			_morph_rotate = Quaternion::Identity();
			_f_translate = Vec3::Zero;
			_f_rotate = Quaternion::Identity();
			_ik_r = Quaternion::Identity();
			localMatrix = Mat4x4::Identity();
			globalMatrix = Mat4x4::Identity();

		}

	};


	struct Bone
	{

		Mat4x4 animationMatrix;


		Vec3 globalTranslate;

		Vec3 transformedPosition;

		int64 index;

		String name;
		String nameEng;

		Vec3 position;

		int64 parentBoneIndex;

		int �ό`�K�w;

		// �ό`�ςݐڑ���{�[�����W
		Vec3 transformedConnectBonePosition;

		Vec3 connectBonePosition;
		// �� or ��
		int64 connectBoneIndex;


		BoneFlag flag;


		BoneTransformParameter transformParameter;
		


		double �t�^�� = 1.0;



		int64 �t�^_parentBoneIndex = -1;

	};







}