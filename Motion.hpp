#pragma once



namespace VMD
{

	// �w�b�_
	struct Header
	{
		char header[30]; // "Vocaloid Motion Data 0002\0\0\0\0\0" 30byte
						 // (MMDver2�ȑO��vmd��"Vocaloid Motion Data file\0")
		char modelName[20]; // ���f���� 20byte(MMDver2�ȑO��vmd��10byte)
							// ���e���J����,�Ɩ�,�Z���t�e�̏ꍇ��"�J�����E�Ɩ�\0on Data"�ƂȂ�
	};

	struct BoneFrame
	{
		String name;

		uint32 frame; // �t���[���ԍ�

		Vec3 position;

		Quaternion rotation;

		CubicBezier b_x;
		CubicBezier b_y;
		CubicBezier b_z;
		CubicBezier b_r;

	};


	// �{�[�����
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
				DWORD frame; // �t���[���ԍ�
				float x; // �{�[����X���ʒu,�ʒu�f�[�^���Ȃ��ꍇ��0
				float y; // �{�[����Y���ʒu,�ʒu�f�[�^���Ȃ��ꍇ��0
				float z; // �{�[����Z���ʒu,�ʒu�f�[�^���Ȃ��ꍇ��0
				float qx; // �{�[���̃N�H�[�^�j�I��X��],�f�[�^���Ȃ��ꍇ��0
				float qy; // �{�[���̃N�H�[�^�j�I��Y��],�f�[�^���Ȃ��ꍇ��0
				float qz; // �{�[���̃N�H�[�^�j�I��Z��],�f�[�^���Ȃ��ꍇ��0
				float qw; // �{�[���̃N�H�[�^�j�I��W��],�f�[�^���Ȃ��ꍇ��1
				char bezier[64]; // ��ԃp�����[�^
			*/

			auto frameNum = reader.value<uint32>();



			boneFrame.position = reader.value<Vec3>();
			boneFrame.rotation = reader.value<Quaternion>();



			char
				// X ���̕�ԃp�����[�^
				X_ax, X_ay, X_bx, X_by,
				// Y ���̕�ԃp�����[�^
				Y_ax, Y_ay, Y_bx, Y_by,
				// Z ���̕�ԃp�����[�^
				Z_ax, Z_ay, Z_bx, Z_by,
				// ��]�̕�ԃp�����[�^
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

		if (boneFrames.size() == 0) MessageBox::Show(Format(L"���̃{�[�����ɃL�[�t���[���͑��݂��܂���: ", name));

		// �L�[�t���[���� 1 �����Ȃ��Ȃ�
		if (boneFrames.size() == 1) return boneFrames[0];


		Optional<VMD::BoneFrame> o_begin = none;
		Optional<VMD::BoneFrame> o_end = none;

		for (auto &boneFrame : boneFrames)
		{

			if (frame >= boneFrame.frame) o_begin = boneFrame;

			if (!o_end && frame < boneFrame.frame) o_end = boneFrame;

		}


		// frame ����ɃL�[�t���[�����Ȃ��Ȃ�
		if (o_begin == none)
		{
			// �Ō�̃L�[�t���[����Ԃ�
			return boneFrames[boneFrames.size() - 1];
		}

		// ���̃L�[�t���[�����Ȃ��Ȃ�
		if (o_end == none)
		{
			return o_begin.value();
		}





		const auto begin = o_begin.value();
		const auto end = o_end.value();


		// begin ~ end �̃t���[����
		const double distance = end.frame - begin.frame;



		// 0.0 ~ 1.0
		// begin �� end �̊Ԃ̂ǂ̈ʒu�� 
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
		// �L�[�t���[�������݂��Ȃ�
		if (!frames.count(key)) return false;

		// ���O���o�^����Ă��邪�A�L�[�t���[���̐��� 0 �̏ꍇ
		// �����s���̃o�O
		if (!frames[key].size()) return false;

		return true;

	}



private:

	void sort()
	{

		for (auto &frame : frames)
		{

			// �L�[�t���[�������ԏ��Ƀ\�[�g����
			std::sort(frame.second.begin(), frame.second.end(), [](auto &a, auto &b)
			{
				return a.frame < b.frame;
			});


		}


	}


};