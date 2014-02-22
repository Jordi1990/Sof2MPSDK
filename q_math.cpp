// Copyright (C) 2001-2002 Raven Software.
//
// q_math.c -- stateless support routines that are included in each code module
#include "q_shared.h"
#include <random>

std::random_device rd;
std::mt19937 eng(rd());                         // Mersenne Twister generator with
// a different seed at each run

vec3_t	vec3_origin = { 0.0f, 0.0f, 0.0f };
vec3_t	vec3_identity = { 1.0f, 1.0f, 1.0f };

vec3_t	bytedirs[NUMVERTEXNORMALS] =
{
	{ -0.525731f, 0.000000f, 0.850651f }, { -0.442863f, 0.238856f, 0.864188f },
	{ -0.295242f, 0.000000f, 0.955423f }, { -0.309017f, 0.500000f, 0.809017f },
	{ -0.162460f, 0.262866f, 0.951056f }, { 0.000000f, 0.000000f, 1.000000f },
	{ 0.000000f, 0.850651f, 0.525731f }, { -0.147621f, 0.716567f, 0.681718f },
	{ 0.147621f, 0.716567f, 0.681718f }, { 0.000000f, 0.525731f, 0.850651f },
	{ 0.309017f, 0.500000f, 0.809017f }, { 0.525731f, 0.000000f, 0.850651f },
	{ 0.295242f, 0.000000f, 0.955423f }, { 0.442863f, 0.238856f, 0.864188f },
	{ 0.162460f, 0.262866f, 0.951056f }, { -0.681718f, 0.147621f, 0.716567f },
	{ -0.809017f, 0.309017f, 0.500000f }, { -0.587785f, 0.425325f, 0.688191f },
	{ -0.850651f, 0.525731f, 0.000000f }, { -0.864188f, 0.442863f, 0.238856f },
	{ -0.716567f, 0.681718f, 0.147621f }, { -0.688191f, 0.587785f, 0.425325f },
	{ -0.500000f, 0.809017f, 0.309017f }, { -0.238856f, 0.864188f, 0.442863f },
	{ -0.425325f, 0.688191f, 0.587785f }, { -0.716567f, 0.681718f, -0.147621f },
	{ -0.500000f, 0.809017f, -0.309017f }, { -0.525731f, 0.850651f, 0.000000f },
	{ 0.000000f, 0.850651f, -0.525731f }, { -0.238856f, 0.864188f, -0.442863f },
	{ 0.000000f, 0.955423f, -0.295242f }, { -0.262866f, 0.951056f, -0.162460f },
	{ 0.000000f, 1.000000f, 0.000000f }, { 0.000000f, 0.955423f, 0.295242f },
	{ -0.262866f, 0.951056f, 0.162460f }, { 0.238856f, 0.864188f, 0.442863f },
	{ 0.262866f, 0.951056f, 0.162460f }, { 0.500000f, 0.809017f, 0.309017f },
	{ 0.238856f, 0.864188f, -0.442863f }, { 0.262866f, 0.951056f, -0.162460f },
	{ 0.500000f, 0.809017f, -0.309017f }, { 0.850651f, 0.525731f, 0.000000f },
	{ 0.716567f, 0.681718f, 0.147621f }, { 0.716567f, 0.681718f, -0.147621f },
	{ 0.525731f, 0.850651f, 0.000000f }, { 0.425325f, 0.688191f, 0.587785f },
	{ 0.864188f, 0.442863f, 0.238856f }, { 0.688191f, 0.587785f, 0.425325f },
	{ 0.809017f, 0.309017f, 0.500000f }, { 0.681718f, 0.147621f, 0.716567f },
	{ 0.587785f, 0.425325f, 0.688191f }, { 0.955423f, 0.295242f, 0.000000f },
	{ 1.000000f, 0.000000f, 0.000000f }, { 0.951056f, 0.162460f, 0.262866f },
	{ 0.850651f, -0.525731f, 0.000000f }, { 0.955423f, -0.295242f, 0.000000f },
	{ 0.864188f, -0.442863f, 0.238856f }, { 0.951056f, -0.162460f, 0.262866f },
	{ 0.809017f, -0.309017f, 0.500000f }, { 0.681718f, -0.147621f, 0.716567f },
	{ 0.850651f, 0.000000f, 0.525731f }, { 0.864188f, 0.442863f, -0.238856f },
	{ 0.809017f, 0.309017f, -0.500000f }, { 0.951056f, 0.162460f, -0.262866f },
	{ 0.525731f, 0.000000f, -0.850651f }, { 0.681718f, 0.147621f, -0.716567f },
	{ 0.681718f, -0.147621f, -0.716567f }, { 0.850651f, 0.000000f, -0.525731f },
	{ 0.809017f, -0.309017f, -0.500000f }, { 0.864188f, -0.442863f, -0.238856f },
	{ 0.951056f, -0.162460f, -0.262866f }, { 0.147621f, 0.716567f, -0.681718f },
	{ 0.309017f, 0.500000f, -0.809017f }, { 0.425325f, 0.688191f, -0.587785f },
	{ 0.442863f, 0.238856f, -0.864188f }, { 0.587785f, 0.425325f, -0.688191f },
	{ 0.688191f, 0.587785f, -0.425325f }, { -0.147621f, 0.716567f, -0.681718f },
	{ -0.309017f, 0.500000f, -0.809017f }, { 0.000000f, 0.525731f, -0.850651f },
	{ -0.525731f, 0.000000f, -0.850651f }, { -0.442863f, 0.238856f, -0.864188f },
	{ -0.295242f, 0.000000f, -0.955423f }, { -0.162460f, 0.262866f, -0.951056f },
	{ 0.000000f, 0.000000f, -1.000000f }, { 0.295242f, 0.000000f, -0.955423f },
	{ 0.162460f, 0.262866f, -0.951056f }, { -0.442863f, -0.238856f, -0.864188f },
	{ -0.309017f, -0.500000f, -0.809017f }, { -0.162460f, -0.262866f, -0.951056f },
	{ 0.000000f, -0.850651f, -0.525731f }, { -0.147621f, -0.716567f, -0.681718f },
	{ 0.147621f, -0.716567f, -0.681718f }, { 0.000000f, -0.525731f, -0.850651f },
	{ 0.309017f, -0.500000f, -0.809017f }, { 0.442863f, -0.238856f, -0.864188f },
	{ 0.162460f, -0.262866f, -0.951056f }, { 0.238856f, -0.864188f, -0.442863f },
	{ 0.500000f, -0.809017f, -0.309017f }, { 0.425325f, -0.688191f, -0.587785f },
	{ 0.716567f, -0.681718f, -0.147621f }, { 0.688191f, -0.587785f, -0.425325f },
	{ 0.587785f, -0.425325f, -0.688191f }, { 0.000000f, -0.955423f, -0.295242f },
	{ 0.000000f, -1.000000f, 0.000000f }, { 0.262866f, -0.951056f, -0.162460f },
	{ 0.000000f, -0.850651f, 0.525731f }, { 0.000000f, -0.955423f, 0.295242f },
	{ 0.238856f, -0.864188f, 0.442863f }, { 0.262866f, -0.951056f, 0.162460f },
	{ 0.500000f, -0.809017f, 0.309017f }, { 0.716567f, -0.681718f, 0.147621f },
	{ 0.525731f, -0.850651f, 0.000000f }, { -0.238856f, -0.864188f, -0.442863f },
	{ -0.500000f, -0.809017f, -0.309017f }, { -0.262866f, -0.951056f, -0.162460f },
	{ -0.850651f, -0.525731f, 0.000000f }, { -0.716567f, -0.681718f, -0.147621f },
	{ -0.716567f, -0.681718f, 0.147621f }, { -0.525731f, -0.850651f, 0.000000f },
	{ -0.500000f, -0.809017f, 0.309017f }, { -0.238856f, -0.864188f, 0.442863f },
	{ -0.262866f, -0.951056f, 0.162460f }, { -0.864188f, -0.442863f, 0.238856f },
	{ -0.809017f, -0.309017f, 0.500000f }, { -0.688191f, -0.587785f, 0.425325f },
	{ -0.681718f, -0.147621f, 0.716567f }, { -0.442863f, -0.238856f, 0.864188f },
	{ -0.587785f, -0.425325f, 0.688191f }, { -0.309017f, -0.500000f, 0.809017f },
	{ -0.147621f, -0.716567f, 0.681718f }, { -0.425325f, -0.688191f, 0.587785f },
	{ -0.162460f, -0.262866f, 0.951056f }, { 0.442863f, -0.238856f, 0.864188f },
	{ 0.162460f, -0.262866f, 0.951056f }, { 0.309017f, -0.500000f, 0.809017f },
	{ 0.147621f, -0.716567f, 0.681718f }, { 0.000000f, -0.525731f, 0.850651f },
	{ 0.425325f, -0.688191f, 0.587785f }, { 0.587785f, -0.425325f, 0.688191f },
	{ 0.688191f, -0.587785f, 0.425325f }, { -0.955423f, 0.295242f, 0.000000f },
	{ -0.951056f, 0.162460f, 0.262866f }, { -1.000000f, 0.000000f, 0.000000f },
	{ -0.850651f, 0.000000f, 0.525731f }, { -0.955423f, -0.295242f, 0.000000f },
	{ -0.951056f, -0.162460f, 0.262866f }, { -0.864188f, 0.442863f, -0.238856f },
	{ -0.951056f, 0.162460f, -0.262866f }, { -0.809017f, 0.309017f, -0.500000f },
	{ -0.864188f, -0.442863f, -0.238856f }, { -0.951056f, -0.162460f, -0.262866f },
	{ -0.809017f, -0.309017f, -0.500000f }, { -0.681718f, 0.147621f, -0.716567f },
	{ -0.681718f, -0.147621f, -0.716567f }, { -0.850651f, 0.000000f, -0.525731f },
	{ -0.688191f, 0.587785f, -0.425325f }, { -0.587785f, 0.425325f, -0.688191f },
	{ -0.425325f, 0.688191f, -0.587785f }, { -0.425325f, -0.688191f, -0.587785f },
	{ -0.587785f, -0.425325f, -0.688191f }, { -0.688191f, -0.587785f, -0.425325f }
};

//==============================================================

int		Q_rand(int *seed) {
	*seed = (69069 * *seed + 1);
	return *seed;
}

// this isn't a real cheap function to call!
int DirToByte(vec3_t dir) {
	int		i, best;
	float	d, bestd;

	if (!dir) {
		return 0;
	}

	bestd = 0;
	best = 0;
	for (i = 0; i<NUMVERTEXNORMALS; i++)
	{
		d = DotProduct(dir, bytedirs[i]);
		if (d > bestd)
		{
			bestd = d;
			best = i;
		}
	}

	return best;
}

void vectoangles(const vec3_t value1, vec3_t angles) {
	float	forward;
	float	yaw, pitch;

	if (value1[1] == 0 && value1[0] == 0) {
		yaw = 0;
		if (value1[2] > 0) {
			pitch = 90;
		}
		else {
			pitch = 270;
		}
	}
	else {
		if (value1[0]) {
			yaw = (atan2(value1[1], value1[0]) * 180 / M_PI);
		}
		else if (value1[1] > 0) {
			yaw = 90;
		}
		else {
			yaw = 270;
		}
		if (yaw < 0) {
			yaw += 360;
		}

		forward = sqrt(value1[0] * value1[0] + value1[1] * value1[1]);
		pitch = (atan2(value1[2], forward) * 180 / M_PI);
		if (pitch < 0) {
			pitch += 360;
		}
	}

	angles[PITCH] = -pitch;
	angles[YAW] = yaw;
	angles[ROLL] = 0;
}

/*
=================
AnglesToAxis
=================
*/
void AnglesToAxis(const vec3_t angles, vec3_t axis[3]) {
	vec3_t	right;

	// angle vectors returns "right" instead of "y axis"
	AngleVectors(angles, axis[0], right, axis[2]);
	VectorSubtract(vec3_origin, right, axis[1]);
}

void LerpVector(vec3_t from, vec3_t to, float lerp, vec3_t out)
{
	out[0] = from[0] + (to[0] - from[0]) * lerp;
	out[1] = from[1] + (to[1] - from[1]) * lerp;
	out[2] = from[2] + (to[2] - from[2]) * lerp;
}

/*
=================
AngleSubtract

Always returns a value from -180 to 180
=================
*/
float	AngleSubtract(float a1, float a2) {
	float	a;

	a = a1 - a2;
	while (a > 180) {
		a -= 360;
	}
	while (a < -180) {
		a += 360;
	}
	return a;
}

void AnglesSubtract(vec3_t v1, vec3_t v2, vec3_t v3) {
	v3[0] = AngleSubtract(v1[0], v2[0]);
	v3[1] = AngleSubtract(v1[1], v2[1]);
	v3[2] = AngleSubtract(v1[2], v2[2]);
}

float	AngleMod(float a) {
	a = (360.0 / 65536) * ((int)(a*(65536 / 360.0)) & 65535);
	return a;
}

/*
=================
AngleNormalize360

returns angle normalized to the range [0 <= angle < 360]
=================
*/
float AngleNormalize360(float angle) {
	return (360.0 / 65536) * ((int)(angle * (65536 / 360.0)) & 65535);
}

/*
=================
AngleNormalize180

returns angle normalized to the range [-180 < angle <= 180]
=================
*/
float AngleNormalize180(float angle) {
	angle = AngleNormalize360(angle);
	if (angle > 180.0) {
		angle -= 360.0;
	}
	return angle;
}

float AngleDelta(float angle1, float angle2) {
	return AngleNormalize180(angle1 - angle2);
}

float RadiusFromBounds(const vec3_t mins, const vec3_t maxs) {
	vec3_t	corner;

	for (int i = 0; i<3; i++) {
		float a = fabs(mins[i]);
		float b = fabs(maxs[i]);
		corner[i] = a > b ? a : b;
	}

	return VectorLength(corner);
}

void AddPointToBounds(const vec3_t v, vec3_t mins, vec3_t maxs) {
	if (v[0] < mins[0]) {
		mins[0] = v[0];
	}
	if (v[0] > maxs[0]) {
		maxs[0] = v[0];
	}

	if (v[1] < mins[1]) {
		mins[1] = v[1];
	}
	if (v[1] > maxs[1]) {
		maxs[1] = v[1];
	}

	if (v[2] < mins[2]) {
		mins[2] = v[2];
	}
	if (v[2] > maxs[2]) {
		maxs[2] = v[2];
	}
}

vec_t VectorNormalize(vec3_t v) {
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);

	if (length) {
		ilength = 1 / length;
		v[0] *= ilength;
		v[1] *= ilength;
		v[2] *= ilength;
	}

	return length;
}

vec_t VectorNormalize2(const vec3_t v, vec3_t out) {
	float	length, ilength;

	length = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	length = sqrt(length);

	if (length)
	{
		ilength = 1 / length;
		out[0] = v[0] * ilength;
		out[1] = v[1] * ilength;
		out[2] = v[2] * ilength;
	}
	else {
		VectorClear(out);
	}

	return length;
}

void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up) {
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = angles[YAW] * (M_PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH] * (M_PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL] * (M_PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward[0] = cp*cy;
		forward[1] = cp*sy;
		forward[2] = -sp;
	}
	if (right)
	{
		right[0] = (-1 * sr*sp*cy + -1 * cr*-sy);
		right[1] = (-1 * sr*sp*sy + -1 * cr*cy);
		right[2] = -1 * sr*cp;
	}
	if (up)
	{
		up[0] = (cr*sp*cy + -sr*-sy);
		up[1] = (cr*sp*sy + -sr*cy);
		up[2] = cr*cp;
	}
}

// Returns a float min <= x < max (exclusive; will get max - 0.00001; but never max)

float flrand(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max);
	return dist(eng);
}

// Returns an integer min <= x <= max (ie inclusive)

int irand(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max);
	return dist(eng);
}
