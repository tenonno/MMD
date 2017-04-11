#pragma once

class Player : public PMX::Model
{

public:


	Key up = Input::KeyW;
	Key down = Input::KeyS;
	Key left = Input::KeyA;
	Key right = Input::KeyD;

	Vec3 previousPos;


	Player(const FilePath &path)
		: Model(path)
	{





	}


	void update(const double cameraAngle)
	{


		Vector2D<int8> move(0, 0);

		if (Input::KeyW.pressed) --move.y;
		if (Input::KeyS.pressed) ++move.y;
		if (Input::KeyA.pressed) ++move.x;
		if (Input::KeyD.pressed) --move.x;



		position += Quaternion(Vec3::Up, cameraAngle) * Vec3(move.x, 0, move.y) * 0.1;


		auto an = Atan2((double)move.y, (double)move.x);// Vec2(0, 0.99).cross(Vec2(move));


		if (move.length() > 0.0)
		{
			nowRotateQ = Quaternion(Vec3::Up, cameraAngle - an - HalfPi);
		}

		rotateQ = Math::Slerp(rotateQ, nowRotateQ, 0.1);

	}









};