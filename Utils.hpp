#pragma once


namespace s3d
{
	namespace Math
	{
		constexpr double PI = Pi;
	}
}




namespace PMX
{


	struct Header
	{

		// 4byte + ––”ö
		uint8 type[5];

		float version;

		uint8 dataSize;

		Array<uint8> datas;




	};

}



inline void DrawConnectLine(Vec2 v1, Vec2 v2, double size, Color color, double thickness = 1.0)
{

	auto direction = (v2 - v1).normalize() * size;

	auto L = v1 + Mat3x2::Rotate(+90_deg).transform(direction);
	auto R = v1 + Mat3x2::Rotate(-90_deg).transform(direction);




	Circle(v1, size).drawFrame(thickness / 2.0, thickness / 2.0, color);

	Line(L, v2).draw(thickness, color);
	Line(R, v2).draw(thickness, color);


}


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



template<class ...Args>
inline void $(const Args &...args)
{
	MessageBox::Show(Format(args...));
}





#include <array>
#include <limits>

typedef std::array<float, 3> float3;
typedef std::array<float3, 3> float3x3;



inline Vec3 q_eulerAngles(Quaternion q)
{

	double q0 = DirectX::XMVectorGetX(q.component);
	double q1 = DirectX::XMVectorGetY(q.component);
	double q2 = DirectX::XMVectorGetZ(q.component);
	double q3 = DirectX::XMVectorGetW(q.component);

	return
	{
		atan2(2 * (q0*q1 + q2*q3), 1 - 2 * (q1*q1 + q2*q2)),
		asin(2 * (q0*q2 - q3*q1)),
		atan2(2 * (q0*q3 + q1*q2), 1 - 2 * (q2*q2 + q3*q3))
	};
}



inline Quaternion toQuaternion(double pitch, double roll, double yaw)
{
	double x, y, z, w;

	double t0 = Cos(yaw * 0.5);
	double t1 = Sin(yaw * 0.5);
	double t2 = Cos(roll * 0.5);
	double t3 = Sin(roll * 0.5);
	double t4 = Cos(pitch * 0.5);
	double t5 = Sin(pitch * 0.5);

	x = t0 * t2 * t4 + t1 * t3 * t5;
	y = t0 * t3 * t4 - t1 * t2 * t5;
	z = t0 * t2 * t5 + t1 * t3 * t4;
	w = t1 * t2 * t4 - t0 * t3 * t5;

	return Quaternion(x, y, z, w);
}




inline bool closeEnough(const float& a, const float& b, const float& epsilon = std::numeric_limits<float>::epsilon()) {
	return (epsilon > std::abs(a - b));
}
inline float3 eulerAngles(Mat4x4 m) {
	

	float3x3 R;
	using namespace DirectX;


	R[0][0] = XMVectorGetX(m.r[0]);
	R[0][1] = XMVectorGetY(m.r[0]);
	R[0][2] = XMVectorGetZ(m.r[0]);

	R[1][0] = XMVectorGetX(m.r[1]);
	R[1][1] = XMVectorGetY(m.r[1]);
	R[1][2] = XMVectorGetZ(m.r[1]);

	R[2][0] = XMVectorGetX(m.r[2]);
	R[2][1] = XMVectorGetY(m.r[2]);
	R[2][2] = XMVectorGetZ(m.r[2]);


	//check for gimbal lock
	if (closeEnough(R[0][2], -1.0f)) {
		float x = 0; //gimbal lock, value of x doesn't matter
		float y = PI / 2;
		float z = x + atan2(R[1][0], R[2][0]);
		float3 returnvalue;
		returnvalue[0] = x;
		returnvalue[1] = y;
		returnvalue[2] = z;
		return returnvalue;
	}
	else if (closeEnough(R[0][2], 1.0f)) {
		float x = 0;
		float y = -PI / 2;
		float z = -x + atan2(-R[1][0], -R[2][0]);
		float3 returnvalue;
		returnvalue[0] = x;
		returnvalue[1] = y;
		returnvalue[2] = z;
		return returnvalue;
	}
	else { //two solutions exist
		float x1 = -asin(R[0][2]);
		float x2 = PI - x1;

		float y1 = atan2(R[1][2] / cos(x1), R[2][2] / cos(x1));
		float y2 = atan2(R[1][2] / cos(x2), R[2][2] / cos(x2));

		float z2 = atan2(R[0][1] / cos(x2), R[0][0] / cos(x2));
		float z1 = atan2(R[0][1] / cos(x1), R[0][0] / cos(x1));

		//choose one solution to return
		//for example the "shortest" rotation
		if ((std::abs(x1) + std::abs(y1) + std::abs(z1)) <= (std::abs(x2) + std::abs(y2) + std::abs(z2))) {
			float3 returnvalue;
			returnvalue[0] = x1;
			returnvalue[1] = y1;
			returnvalue[2] = z1;
			return returnvalue;
		}
		else {
			float3 returnvalue;
			returnvalue[0] = x2;
			returnvalue[1] = y2;
			returnvalue[2] = z2;
			return returnvalue;
		}
	}
}



inline Quaternion QtoEtoQ2(const Quaternion &q)
{
	using namespace DirectX;

	Vec3 euler = q_eulerAngles(q.normalized());

	double fX = euler.x;
	double fY = euler.y;
	double fZ = euler.z;

	Quaternion r;


	Println(fX, fY, fZ);


	r = toQuaternion(fY, fX, fZ);


	return r;
}


inline Quaternion QtoEtoQ(const Quaternion &q, const Vec3 &min, const Vec3 &max)
{

	using namespace DirectX;

	auto mtx = q.toMatrix();


	auto RotMat = mtx;


	// XŽ²‰ñ‚è
	float fXLimit = 80.0f / 180.0f * Pi;
	float fSX = -RotMat.r[2].m128_f32[1];    // sin(ƒÆx)
	double fX = Asin(fSX);   // XŽ²‰ñ‚èŒˆ’è
	float fCX = (float)cos(fX);

	// ƒWƒ“ƒoƒ‹ƒƒbƒN‰ñ”ð
	if (fabs(fX) > fXLimit) {
		fX = (fX < 0) ? -fXLimit : fXLimit;
		fCX = (float)cos(fX);
	}

	// YŽ²‰ñ‚è
	float fSY = RotMat.r[2].m128_f32[0] / fCX;
	float fCY = RotMat.r[2].m128_f32[2] / fCX;
	double fY = Atan2(fSY, fCY);   // YŽ²‰ñ‚èŒˆ’è

										 // ZŽ²‰ñ‚è
	float fSZ = RotMat.r[0].m128_f32[1] / fCX;
	float fCZ = RotMat.r[1].m128_f32[1] / fCX;
	double fZ = Atan2(fSZ, fCZ);



	/*
	Vec3 euler = q_eulerAngles(q.normalized());
	fX = euler.x;
	fY = euler.y;
	fZ = euler.z;
	*/

	//mtx = XMMatrixRotationRollPitchYaw(rx,ry,rz);
	//mtx‚©‚çƒIƒCƒ‰[Šp‚ð‹‚ß‚é
	//‚½‚¾‚µ -ƒÎ/2 < rx < +ƒÎ/2
	/*
	rx = atan2f(XMVectorGetZ(mtx.r[1]), XMVectorGetZ(mtx.r[2]));
	ry = -asinf(XMVectorGetZ(mtx.r[0]));
	rz = atan2f(XMVectorGetY(mtx.r[0]), XMVectorGetX(mtx.r[0]));
	*/

	/*

	fX = rx;
	fY = ry;
	fZ = rz;

	*/

	/*

	fX = eulerAngles(mtx)[0];
	fY = eulerAngles(mtx)[1];
	fZ = eulerAngles(mtx)[2];


	fX = rx;
	fY = ry;
	fZ = rz;
	*/

	// Println(L"X: ", Floor(Degrees(fX)), L", Y: ", Floor(Degrees(fY)), L", Z: ", Floor(Degrees(fZ)));


	fX = Clamp(fX, min.x, max.x);
	fY = Clamp(fY, min.y, max.y);
	fZ = Clamp(fZ, min.z, max.z);

	/*

	Quaternion r;

	switch (guiValueInt)
	{
	case 0:
		r = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);
		break;
	case 1:
		r = XMQuaternionRotationRollPitchYaw(fX, fZ, fY);
		break;
	case 2:
		r = XMQuaternionRotationRollPitchYaw(fY, fX, fZ);
		break;
	case 3:
		r = XMQuaternionRotationRollPitchYaw(fY, fZ, fX);
		break;
	case 4:
		r = XMQuaternionRotationRollPitchYaw(fZ, fX, fY);
		break;
	case 5:
		r = XMQuaternionRotationRollPitchYaw(fZ, fY, fX);
		break;
	}


	Println(guiValueInt);

	*/


	return XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

}

















inline Vec3 D3DXVec3RotateReg(
	Vec3 axis,      // ‰ñ“]Ž²
	float Angle,                              // ‰ñ“]Šp“x
	Vec3 pV,          // ‰ñ“]‚³‚¹‚½‚¢ˆÊ’uƒxƒNƒgƒ‹
	
	Vec3 min, Vec3 max,

	Vec3 lBonePos

	, bool log = false

)
{

	auto ppp = pV;


	double length = pV.length();



	/////////////////////////////////
	// ‰ñ“]s—ñ‚ðì¬
	//////
	// ‰ñ“]ŒãƒxƒNƒgƒ‹‚ÌŽZo
	Vec3 After = Quaternion(axis.normalized(), Angle).toMatrix().transform(pV).normalized();



	// auto f = Vec3(0, 1, 0);

	auto f = lBonePos.normalized();

	auto v = f * Vec3(length, length, length);



	// ‰ñ“]ŒãƒxƒNƒgƒ‹‚ÆZŽ²•ûŒü‚ÌƒxƒNƒgƒ‹‚©‚ç
	// ‰ñ“]Ž²‚Æ‰ñ“]Šp“x‚ðŽZo
	Vec3 Norm = Vec3(f).cross(After).normalized();


	double ang = Acos(Vec3(f).dot(After));

	auto rP = v;


	auto rQ = Quaternion(Norm, ang);



	using namespace DirectX;

	Vec3 euler = q_eulerAngles(rQ.normalized());

	double fX = euler.x;
	double fY = euler.y;
	double fZ = euler.z;


	/*


	return r;

	////////////////////////
	// Ž²‰ñ“]Šp“x‚ðŽZo
	/////



	// XŽ²‰ñ‚è
	float fXLimit = 80.0f / 180.0f * Pi;
	float fSX = -RotMat.r[2].m128_f32[1];    // sin(ƒÆx)
	float fX = (float)asin(fSX);   // XŽ²‰ñ‚èŒˆ’è
	float fCX = (float)cos(fX);

	// ƒWƒ“ƒoƒ‹ƒƒbƒN‰ñ”ð
	if (fabs(fX) > fXLimit) {
		fX = (fX < 0) ? -fXLimit : fXLimit;
		fCX = (float)cos(fX);
	}

	// YŽ²‰ñ‚è
	float fSY = RotMat.r[2].m128_f32[0] / fCX;
	float fCY = RotMat.r[2].m128_f32[2] / fCX;
	float fY = (float)Atan2(fSY, fCY);   // YŽ²‰ñ‚èŒˆ’è

										 // ZŽ²‰ñ‚è
	float fSZ = RotMat.r[0].m128_f32[1] / fCX;
	float fCZ = RotMat.r[1].m128_f32[1] / fCX;
	float fZ = (float)Atan2(fSZ, fCZ);

	if (log)
		Println(L"Angle: ", Degrees(fX), L", ", Degrees(fY), L", ", Degrees(fZ));

	if (log)
	{




	}

	*/



	// Šp“x‚Ì§ŒÀ
	if (min.x != max.x)
	{
		if (fX < min.x) fX = min.x;
		if (fX > max.x) fX = max.x;
		Println(L"limit axis x");
	}
	if (min.y != max.y)
	{
		if (fY < min.y) fY = min.y;
		if (fY > max.y) fY = max.y;
		Println(L"limit axis y");
	}
	if (min.z != max.z)
	{
		if (fZ < min.z) fZ = min.z;
		if (fZ > max.z) fZ = max.z;
		Println(L"limit axis z");
	}



	/*

	auto q = Quaternion::RollPitchYaw(fZ, fX, fY);



	RotMat = q.toMatrix();

	*/



	Quaternion r = toQuaternion(fY, fX, fZ);

	auto RotMat = r.toMatrix();

	// Println(L"L: ", pV.length());

	Vec3 tmp = RotMat.transform(rP);


	return tmp;
}