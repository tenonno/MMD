#pragma once


namespace PMX
{


	class Reader : public BinaryReader
	{

		TextEncoding m_encoding = TextEncoding::Default;

	public:

		Reader(const FilePath &path) : BinaryReader(path)
		{

		}


		void setEncoding(const TextEncoding &encoding)
		{
			m_encoding = encoding;
		}

	
		template<class T>
		T value()
		{
			T value;
			read<T>(value);
			return value;
		}


		template<>
		Vec2 value()
		{
			Float2 value;
			read<Float2>(value);
			return value;
		}


		template<>
		Vec3 value()
		{
			Float3 value;
			read<Float3>(value);
			return value;
		}

		template<>
		bool value()
		{
			return value<uint8>();
		}

		

		template<>
		Vec4 value()
		{
			Float4 value;
			read<Float4>(value);
			return value;
		}


		template<>
		Quaternion value()
		{
			Float4 value;
			read<Float4>(value);
			return Quaternion(value.x, value.y, value.z, value.w);
		}


		template<>
		double value()
		{
			float value;
			read<float>(value);
			return value;
		}


		int64 value(const size_t size)
		{
			return fromSize(size);
		}

		template<>
		String value()
		{

			auto size = value<int>();



			if (m_encoding == TextEncoding::UTF8)
			{

				auto buffer = new int8[size + 1]();

				read(buffer, size);

				std::string utf8 = (const char *)buffer;


				delete[] buffer;

				return FromUTF8(utf8);

			}


			if (m_encoding == TextEncoding::UTF16LE)
			{



				auto buffer = new int8[size + 4]();


				read(buffer, size);

				String bb = (const wchar *)buffer;


				delete[] buffer;

				return bb;

			}

			return L"?";

		}






		int readInt()
		{
			int value;
			BinaryReader::read(&value, 4);
			return value;
		}

		uint32 readUInt32()
		{
			uint32 value;
			BinaryReader::read(&value, 4);
			return value;
		}

		Vec3 readVec3()
		{

			Float3 value;

			BinaryReader::read<Float3>(value);

			return value;
		}

		Vec4 readVec4()
		{
			float x, y, z, w;
			BinaryReader::read(&x, 4);
			BinaryReader::read(&y, 4);
			BinaryReader::read(&z, 4);
			BinaryReader::read(&w, 4);
			return Vec4(x, y, z, w);
		}


		double readFloat()
		{
			float value;
			BinaryReader::read(&value, 4);
			return value;
		}


		int64 fromSize(const size_t size)
		{


			switch (size)
			{

			case 1:


				int8 v8;


				BinaryReader::read(&v8, size);

				return static_cast<int64>(v8);

			case 2:

				int16 v16;

				BinaryReader::read(&v16, size);

				return static_cast<int64>(v16);

			case 4:

				int32 v32;

				BinaryReader::read(&v32, size);

				return static_cast<int64>(v32);

			case 8:

				int64 v64;

				BinaryReader::read(&v64, size);

				return v64;


			}

			static_assert(L"aa", L"");

		}



	};
}