#pragma once



namespace VMD
{

	// ヘッダ
	struct Header
	{
		char header[30]; // "Vocaloid Motion Data 0002\0\0\0\0\0" 30byte
						 // (MMDver2以前のvmdは"Vocaloid Motion Data file\0")
		char modelName[20]; // モデル名 20byte(MMDver2以前のvmdは10byte)
							// 内容がカメラ,照明,セルフ影の場合は"カメラ・照明\0on Data"となる
	};

	struct BoneFrame
	{
		String name;

		uint32 frame; // フレーム番号

		Vec3 position;

		Quaternion rotation;

		CubicBezier b_x;
		CubicBezier b_y;
		CubicBezier b_z;
		CubicBezier b_r;

	};


	// ボーン情報
	struct Bone
	{
		Vec3 position;
		Quaternion rotation;

		Bone() = default;

		Bone(const BoneFrame &boneFrame)
			: position(boneFrame.position),
			rotation(boneFrame.rotation)
		{

		}

	};

}









class Motion
{


public:

	std::unordered_map<String, Array<VMD::BoneFrame>> frames;


	Motion(const FilePath &path)
	{

		PMX::Reader reader(path);


		VMD::Header header;

		// Header
		reader.skip(50);


		const size_t boneCount = reader.value<uint32>();


		for (auto boneIndex : step(boneCount))
		{

			VMD::BoneFrame boneFrame;

			char name[16] = { 0 };

			reader.read(name, 15);


			boneFrame.name = Widen(name);


			/*
				DWORD frame; // フレーム番号
				float x; // ボーンのX軸位置,位置データがない場合は0
				float y; // ボーンのY軸位置,位置データがない場合は0
				float z; // ボーンのZ軸位置,位置データがない場合は0
				float qx; // ボーンのクォータニオンX回転,データがない場合は0
				float qy; // ボーンのクォータニオンY回転,データがない場合は0
				float qz; // ボーンのクォータニオンZ回転,データがない場合は0
				float qw; // ボーンのクォータニオンW回転,データがない場合は1
				char bezier[64]; // 補間パラメータ
			*/

			auto frameNum = reader.value<uint32>();



			boneFrame.position = reader.value<Vec3>();
			boneFrame.rotation = reader.value<Quaternion>();



			char
				// X 軸の補間パラメータ
				X_ax, X_ay, X_bx, X_by,
				// Y 軸の補間パラメータ
				Y_ax, Y_ay, Y_bx, Y_by,
				// Z 軸の補間パラメータ
				Z_ax, Z_ay, Z_bx, Z_by,
				// 回転の補間パラメータ
				R_ax, R_ay, R_bx, R_by;


			X_ax = reader.value<char>();
			Y_ax = reader.value<char>();
			Z_ax = reader.value<char>();
			R_ax = reader.value<char>();
			X_ay = reader.value<char>();
			Y_ay = reader.value<char>();
			Z_ay = reader.value<char>();
			R_ay = reader.value<char>();
			X_bx = reader.value<char>();
			Y_bx = reader.value<char>();
			Z_bx = reader.value<char>();
			R_bx = reader.value<char>();
			X_by = reader.value<char>();
			Y_by = reader.value<char>();
			Z_by = reader.value<char>();
			R_by = reader.value<char>();

			Y_ax = reader.value<char>();
			Z_ax = reader.value<char>();
			R_ax = reader.value<char>();
			X_ay = reader.value<char>();
			Y_ay = reader.value<char>();
			Z_ay = reader.value<char>();
			R_ay = reader.value<char>();
			X_bx = reader.value<char>();
			Y_bx = reader.value<char>();
			Z_bx = reader.value<char>();
			R_bx = reader.value<char>();
			X_by = reader.value<char>();
			Y_by = reader.value<char>();
			Z_by = reader.value<char>();
			R_by = reader.value<char>();
			// 01 = reader.value<char>();
			reader.skip(1);
			Z_ax = reader.value<char>();
			R_ax = reader.value<char>();
			X_ay = reader.value<char>();
			Y_ay = reader.value<char>();
			Z_ay = reader.value<char>();
			R_ay = reader.value<char>();
			X_bx = reader.value<char>();
			Y_bx = reader.value<char>();
			Z_bx = reader.value<char>();
			R_bx = reader.value<char>();
			X_by = reader.value<char>();
			Y_by = reader.value<char>();
			Z_by = reader.value<char>();
			R_by = reader.value<char>();

			// 01 = reader.value<char>();
			reader.skip(1);
			// 00 = reader.value<char>();
			reader.skip(1);

			R_ax = reader.value<char>();
			X_ay = reader.value<char>();
			Y_ay = reader.value<char>();
			Z_ay = reader.value<char>();
			R_ay = reader.value<char>();
			X_bx = reader.value<char>();
			Y_bx = reader.value<char>();
			Z_bx = reader.value<char>();
			R_bx = reader.value<char>();
			X_by = reader.value<char>();
			Y_by = reader.value<char>();
			Z_by = reader.value<char>();
			R_by = reader.value<char>();
			// 01 = reader.value<char>();
			reader.skip(1);
			// 00 = reader.value<char>();
			reader.skip(1);
			// 00 = reader.value<char>();
			reader.skip(1);


			boneFrame.b_x.p0 = Vec2(0, 0);
			boneFrame.b_y.p0 = Vec2(0, 0);
			boneFrame.b_z.p0 = Vec2(0, 0);
			boneFrame.b_r.p0 = Vec2(0, 0);

			boneFrame.b_x.p3 = Vec2(127, 127);
			boneFrame.b_y.p3 = Vec2(127, 127);
			boneFrame.b_z.p3 = Vec2(127, 127);
			boneFrame.b_r.p3 = Vec2(127, 127);



			boneFrame.b_x.p1 = Vec2(X_ax, X_ay);
			boneFrame.b_y.p1 = Vec2(Y_ax, Y_ay);
			boneFrame.b_z.p1 = Vec2(Z_ax, Z_ay);
			boneFrame.b_r.p1 = Vec2(R_ax, R_ay);

			boneFrame.b_x.p2 = Vec2(X_bx, X_by);
			boneFrame.b_y.p2 = Vec2(Y_bx, Y_by);
			boneFrame.b_z.p2 = Vec2(Z_bx, Z_by);
			boneFrame.b_r.p2 = Vec2(R_bx, R_by);


			boneFrame.frame = frameNum;


			frames[boneFrame.name].emplace_back(boneFrame);


		}


		sort();

	}



	VMD::Bone get(const String &name, const double frame)
	{


		const auto &boneFrames = frames[name];

		if (boneFrames.size() == 0) MessageBox::Show(Format(L"次のボーン名にキーフレームは存在しません: ", name));

		// キーフレームが 1 つしかないなら
		if (boneFrames.size() == 1) return boneFrames[0];


		Optional<VMD::BoneFrame> o_begin = none;
		Optional<VMD::BoneFrame> o_end = none;

		for (auto &boneFrame : boneFrames)
		{

			if (frame >= boneFrame.frame) o_begin = boneFrame;

			if (!o_end && frame < boneFrame.frame) o_end = boneFrame;

		}


		// frame より後にキーフレームがないなら
		if (o_begin == none)
		{
			// 最後のキーフレームを返す
			return boneFrames[boneFrames.size() - 1];
		}

		// 次のキーフレームがないなら
		if (o_end == none)
		{
			return o_begin.value();
		}





		const auto begin = o_begin.value();
		const auto end = o_end.value();


		// begin ~ end のフレーム数
		const double distance = end.frame - begin.frame;



		// 0.0 ~ 1.0
		// begin と end の間のどの位置か 
		const double pos = (frame - begin.frame) / distance;




		const double pos_x = end.b_x.getPos(pos).y / 127.0;
		const double pos_y = end.b_y.getPos(pos).y / 127.0;
		const double pos_z = end.b_z.getPos(pos).y / 127.0;
		const double pos_r = end.b_r.getPos(pos).y / 127.0;



		VMD::Bone bone;

		bone.position.x = Math::Lerp(begin.position.x, end.position.x, pos_x);
		bone.position.y = Math::Lerp(begin.position.y, end.position.y, pos_y);
		bone.position.z = Math::Lerp(begin.position.z, end.position.z, pos_z);

		bone.rotation = Math::Slerp(begin.rotation, end.rotation, pos_r);

		return bone;

	}


	bool has(const String &key) 
	{
		// キーフレームが存在しない
		if (!frames.count(key)) return false;

		// 名前が登録されているが、キーフレームの数が 0 の場合
		// 原因不明のバグ
		if (!frames[key].size()) return false;

		return true;

	}



private:

	void sort()
	{

		for (auto &frame : frames)
		{

			// キーフレームを時間順にソートする
			std::sort(frame.second.begin(), frame.second.end(), [](auto &a, auto &b)
			{
				return a.frame < b.frame;
			});


		}


	}


};