#pragma once





namespace PMX
{

	// ボーンの接続先のタイプ
	// Position: 座標指定
	// Index: ボーンインデックス指定
	enum class BoneConnectType
	{

		Position = 0,
		Index = 1,

	};

	struct BoneFlag
	{

		// 接続先(PMD子ボーン指定)表示方法 -> 0:座標オフセットで指定 1 : ボーンで指定

		BoneConnectType connectType;

		// 0x0100  : 回転付与
		bool 回転付与;

		// 0x0200 : 移動付与
		bool 移動付与;

		bool 軸固定;


		// VMD にはグローバル軸基準の数値が入っているので使わない
		// エディタでアニメーションを編集する時に便利らしい
		bool ローカル軸;

		bool 外部親変形;


		bool 回転可能;
		bool 移動可能;
		bool visible;
		bool 操作可;
		bool IK;

		bool ローカル付与;
		bool 物理後変形;


	};


	struct BoneTransformParameter
	{

		// VMD のキーフレームによる移動量
		Float3 keyframeTranslate;
		// VMD のキーフレームによる回転量
		Quaternion keyframeRotate;

		// ユーザー操作による 回転／移動量 | PMD / PMXエディタ内ではスケール値も所持
		Float3 translate;
		Quaternion rotate;

		// ボーンモーフによる 回転／移動量
		Float3 _morph_translate;
		Quaternion _morph_rotate;

		// 付与ありの場合     付与回転／付与移動量 | 保存しておくことで多重付与が可能になる
		Float3 _f_translate;
		Quaternion _f_rotate;

		// IKリンクの場合     IK回転量
		Quaternion _ik_r;


		// ボーン変形処理によるローカル変形状態
		Mat4x4 localMatrix;


		bool transformed = false;
		bool ___a___ = false;


		bool 付与回転計算済 = false;

		void reset()
		{
			付与回転計算済 = false;
			
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

		int 変形階層;

		// 変形済み接続先ボーン座標
		Vec3 transformedConnectBonePosition = Vec3::Zero;;

		Vec3 connectBonePosition = Vec3::Zero;;
		// ↑ or ↓
		int64 connectBoneIndex;


		BoneFlag flag;


		BoneTransformParameter transformParameter;



		double 付与率 = 1.0;



		int64 付与_parentBoneIndex = -1;



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