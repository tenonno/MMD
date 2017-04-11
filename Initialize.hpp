#pragma once

namespace MMD
{
	inline void LoadToonTexture()
	{

		for (auto i : step(11))
		{

			auto name = Format(L"toon", Pad(i, { 2, L'0' }));

			TextureAsset::Register(
				Format(L"MMD_toon", i),
				Format(L"Assets/toon/", name, L".bmp"));

		}

	}

	inline Texture GetToonTexture(const uint8 index)
	{
		return TextureAsset(Format(L"MMD_toon", index));
	}

}