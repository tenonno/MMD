#pragma once


#define MAX_NUM_ACTOR_SHAPES 128


inline Mat4x4 GetMat4x4Px(const PxMat44 &mat)
{
	Mat4x4 mat4x4;

	mat4x4.r[0].m128_f32[0] = mat.column0.x;
	mat4x4.r[0].m128_f32[1] = mat.column0.y;
	mat4x4.r[0].m128_f32[2] = mat.column0.z;
	mat4x4.r[0].m128_f32[3] = mat.column0.w;

	mat4x4.r[1].m128_f32[0] = mat.column1.x;
	mat4x4.r[1].m128_f32[1] = mat.column1.y;
	mat4x4.r[1].m128_f32[2] = mat.column1.z;
	mat4x4.r[1].m128_f32[3] = mat.column1.w;

	mat4x4.r[2].m128_f32[0] = mat.column2.x;
	mat4x4.r[2].m128_f32[1] = mat.column2.y;
	mat4x4.r[2].m128_f32[2] = mat.column2.z;
	mat4x4.r[2].m128_f32[3] = mat.column2.w;

	mat4x4.r[3].m128_f32[0] = mat.column3.x;
	mat4x4.r[3].m128_f32[1] = mat.column3.y;
	mat4x4.r[3].m128_f32[2] = mat.column3.z;
	mat4x4.r[3].m128_f32[3] = mat.column3.w;

	return mat4x4;
}


void renderActors(PxRigidActor** actors, const PxU32 numActors, bool shadows, const Color &color)
{
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	for (PxU32 i = 0;i < numActors;i++)
	{
		const PxU32 nbShapes = actors[i]->getNbShapes();
		PX_ASSERT(nbShapes <= MAX_NUM_ACTOR_SHAPES);
		actors[i]->getShapes(shapes, nbShapes);
		bool sleeping = actors[i]->is<PxRigidDynamic>() ? actors[i]->is<PxRigidDynamic>()->isSleeping() : false;

		for (PxU32 j = 0;j < nbShapes;j++)
		{


			const auto pose(PxShapeExt::getGlobalPose(*shapes[j], *actors[i]));

			const auto matrix = GetMat4x4Px(pose);

			PxGeometryHolder h = shapes[j]->getGeometry();

			if (shapes[j]->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
				$(L"?");
			// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			// render object


			if (h.getType() == PxGeometryType::eCAPSULE)
			{

				const PxF32 radius = h.capsule().radius;
				const PxF32 halfHeight = h.capsule().halfHeight;

				auto mesh = Cylinder(radius, halfHeight * 2.0, Quaternion(Vec3::Forward, HalfPi)).asMesh();
				mesh.matrix *= matrix;
				mesh.draw(color);

				if (shadows) mesh.drawShadow();

				auto s1 = Sphere(radius).moveBy(Vec3(-halfHeight, 0, 0)).asMesh();
				s1.matrix *= matrix;
				s1.draw(color);
				if (shadows) s1.drawShadow();

				auto s2 = Sphere(radius).moveBy(Vec3(+halfHeight, 0, 0)).asMesh();
				s2.matrix *= matrix;
				s2.draw(color);
				if (shadows) s2.drawShadow();



			}

			/*

			glPushMatrix();
			glMultMatrixf(reinterpret_cast<const float*>(&shapePose));
			if (sleeping)
			{
			PxVec3 darkColor = color * 0.25f;
			glColor4f(darkColor.x, darkColor.y, darkColor.z, 1.0f);
			}
			else
			{
			glColor4f(color.x, color.y, color.z, 1.0f);
			}

			renderGeometry(h);
			glPopMatrix();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			*/

		}
	}
}

inline Vec3 ToVec3(const PxVec3 &vec)
{
	return {
		(double)vec.x,
		(double)vec.y,
		(double)vec.z
	};
}



const auto PoseIdentity = PxTransform(PxVec3(PxZero));
