#pragma once


inline String GetDirectory(const FilePath &path)
{

	String directory = L"";

	auto vv = path.split('/');
	for (auto i : step(vv.size() - 1))
	{
		directory += vv[i] + L"/";
	}

	return directory;

}