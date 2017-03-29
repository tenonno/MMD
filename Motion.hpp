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

		char bezier[64]; // 補間パラメータ
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

			if (boneFrame.name == L"右目動")
			{
				MessageBox::Show(L"右目動き");
			}

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

			auto frameNum = reader.readUInt32();



			boneFrame.position = reader.value<Vec3>();
			boneFrame.rotation = reader.value<Quaternion>();

			reader.skip(64);


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


		VMD::Bone bone;

		bone.position = Math::Lerp(begin.position, end.position, pos);
		bone.rotation = Math::Slerp(begin.rotation, end.rotation, pos);

		return bone;

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