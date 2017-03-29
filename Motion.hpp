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

		char bezier[64]; // ��ԃp�����[�^
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

			if (boneFrame.name == L"�E�ړ�")
			{
				MessageBox::Show(L"�E�ړ���");
			}

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

			// �L�[�t���[�������ԏ��Ƀ\�[�g����
			std::sort(frame.second.begin(), frame.second.end(), [](auto &a, auto &b)
			{
				return a.frame < b.frame;
			});


		}


	}


};