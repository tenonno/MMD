#pragma once





namespace PMX
{

	// �{�[���̐ڑ���̃^�C�v
	// Position: ���W�w��
	// Index: �{�[���C���f�b�N�X�w��
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

		// VMD �̃L�[�t���[���ɂ��ړ���
		Float3 keyframeTranslate;
		// VMD �̃L�[�t���[���ɂ���]��
		Quaternion keyframeRotate;

		// ���[�U�[����ɂ�� ��]�^�ړ��� | PMD / PMX�G�f�B�^���ł̓X�P�[���l������
		Float3 translate;
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


		bool transformed = false;
		bool ___a___ = false;


		bool �t�^��]�v�Z�� = false;

		void reset()
		{
			�t�^��]�v�Z�� = false;
			
			___a___ = false;

			keyframeTranslate = Vec3::Zero;
			keyframeRotate = Quaternion::Identity();

			transformed = false;
			translate = Vec3::Zero;
			rotate = Quaternion::Identity();
			_morph_translate = Vec3::Zero;
			_morph_rotate = Quaternion::Identity();
			_f_translate = Vec3::Zero;
			_f_rotate = Quaternion::Identity();
			_ik_r = Quaternion::Identity();
			localMatrix = Mat4x4::Identity();


		}

	};



	struct IK_Link
	{

		int64 boneIndex;

		bool limit;
		Vec3 min;
		Vec3 max;

		IK_Link()
		{

		}

	};


	struct IK
	{

		int64 targetIndex;

		Array<IK_Link> links;


		uint32 iteration;

		double limitAngle;

	};

	struct Bone
	{



		Mat4x4 animationMatrix = Mat4x4::Identity();

		IK ik;


		Float3 transformedPosition = Vec3::Zero;

		int64 index;

		String name;
		String nameEng;

		Float3 position = Vec3::Zero;;

		int64 parentBoneIndex;

		int �ό`�K�w;

		// �ό`�ςݐڑ���{�[�����W
		Vec3 transformedConnectBonePosition = Vec3::Zero;;

		Vec3 connectBonePosition = Vec3::Zero;;
		// �� or ��
		int64 connectBoneIndex;


		BoneFlag flag;


		BoneTransformParameter transformParameter;



		double �t�^�� = 1.0;



		int64 �t�^_parentBoneIndex = -1;



		Mat4x4 BOfMatrix;

		Vec3 getGPos(Array<Bone> &bones) const
		{

			return getGlobal(bones).transform(position);

		}

		Mat4x4 getGlobal(Array<Bone> &bones) const
		{

			auto parentG = parentBoneIndex == -1 ? Mat4x4::Identity() : bones[parentBoneIndex].getGlobal(bones);

			return Mat4x4(


				BOfMatrix *

				transformParameter.localMatrix *

				BOfMatrix.inverse()

				*
				parentG

			);

		}


		Mat4x4 local() const
		{
			return
				BOfMatrix *
				transformParameter.localMatrix *
				BOfMatrix.inverse();
		}


	};







}