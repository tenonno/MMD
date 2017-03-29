#pragma once




///////////////////////////////////////////////
// 回転行列→クォータニオン変換
//
// qx, qy, qz, qw : クォータニオン成分（出力）
// m1[3] : 回転行列成分 m11 - m13
// m2[3] : 回転行列成分 m21 - m23
// m3[3] : 回転行列成分 m31 - m33
//
// ※注意：
// 行列成分はDirectX形式（行方向が軸の向き）です
// OpenGL形式（列方向が軸の向き）の場合は
// 転置した値を入れて下さい。

///////////////////////////////////////////////
// 回転行列→クォータニオン変換
//
// qx, qy, qz, qw : クォータニオン成分（出力）
// m11-m33 : 回転行列成分
//
// ※注意：
// 行列成分はDirectX形式（行方向が軸の向き）です
// OpenGL形式（列方向が軸の向き）の場合は
// 転置した値を入れて下さい。

bool transformRotMatToQuaternion(
	float &qx, float &qy, float &qz, float &qw,
	float m11, float m12, float m13,
	float m21, float m22, float m23,
	float m31, float m32, float m33
) {
	// 最大成分を検索
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
		return false; // 引数の行列に間違いあり！

					  // 最大要素の値を算出
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
// 回転行列→クォータニオン変換
//
// qx, qy, qz, qw : クォータニオン成分（出力）
// m1[3] : 回転行列成分 m11 - m13
// m2[3] : 回転行列成分 m21 - m23
// m3[3] : 回転行列成分 m31 - m33
//
// ※注意：
// 行列成分はDirectX形式（行方向が軸の向き）です
// OpenGL形式（列方向が軸の向き）の場合は
// 転置した値を入れて下さい。

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

		bool 付与情報計算済 = false;

		bool bbbb = false;

		// VMD のキーフレームによる移動量
		Float3 keyframeTranslate;
		// VMD のキーフレームによる回転量
		Quaternion keyframeRotate;

		// ユーザー操作による 回転／移動量 | PMD / PMXエディタ内ではスケール値も所持
		Vec3 translate;
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

		Float3 localTranslate;
		
		// 頂点など形状変形用のグローバル変形状態
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

			付与情報計算済 = false;
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

		int 変形階層;

		// 変形済み接続先ボーン座標
		Vec3 transformedConnectBonePosition;

		Vec3 connectBonePosition;
		// ↑ or ↓
		int64 connectBoneIndex;


		BoneFlag flag;


		BoneTransformParameter transformParameter;
		


		double 付与率 = 1.0;



		int64 付与_parentBoneIndex = -1;

	};







}