#ifndef TI_SEM_RAY_MATH_H
#define TI_SEM_RAY_MATH_H

#include <math.h>

#define PI32 (3.14159265359f)

union v3f
{
	struct
	{
		f32 X;
		f32 Y;
		f32 Z;
	};

	struct
	{
		f32 R;
		f32 G;
		f32 B;
	};

	f32 E[3];
};

struct quaternion
{
	f32 X;
	f32 Y;
	f32 Z;
	f32 W;
};

struct m44
{
	f32 E[4][4];
};

struct basis
{
	v3f X;
	v3f Y;
	v3f Z;
};

internal smi
Min(smi A, smi B)
{
	return (A < B) ? A : B;
}

internal smi
Min(smi A, smi B, smi C)
{
	return Min(Min(A, B), C);
}

internal f32
Sqrt(f32 Value)
{
	f32 Result = sqrtf(Value);

	return Result;
}

internal f32
InvSqrt(f32 Value)
{
	f32 Result = 1.0f / sqrtf(Value);

	return Result;
}

internal f32
Sin(f32 Value)
{
	f32 Result = sinf(Value);

	return Result;
}

internal f32
Cos(f32 Value)
{
	f32 Result = cosf(Value);

	return Result;
}

internal f32
Tan(f32 Value)
{
	f32 Result = tanf(Value);

	return Result;
}

internal f32
DegToRad(f32 Angle)
{
	f32 Result = (PI32 / 180.0f) * Angle;

	return Result;
}

internal f32
RadToDeg(f32 Angle)
{
	f32 Result = (180.0f / PI32) * Angle;

	return Result;
}

// -- v3f

internal v3f
V3F(f32 X, f32 Y, f32 Z)
{
	v3f Result;

	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;

	return Result;
}

internal v3f
V3F(f32 Value)
{
	v3f Result;

	Result.X = Value;
	Result.Y = Value;
	Result.Z = Value;

	return Result;
}

internal v3f
operator-(v3f Value)
{
	v3f Result;
	
	Result.X = -Value.X;
	Result.Y = -Value.Y;
	Result.Z = -Value.Z;

	return Result;
}

internal v3f
operator+(v3f A, v3f B)
{
	v3f Result;

	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	Result.Z = A.Z + B.Z;

	return Result;
}

internal v3f
operator-(v3f A, v3f B)
{
	v3f Result;

	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;
	Result.Z = A.Z - B.Z;

	return Result;
}

internal v3f
operator*(v3f Vector, f32 Scalar)
{
	v3f Result;

	Result.X = Vector.X * Scalar;
	Result.Y = Vector.Y * Scalar;
	Result.Z = Vector.Z * Scalar;

	return Result;
}

internal v3f
operator*(f32 Scalar, v3f Vector)
{
	v3f Result;

	Result.X = Scalar * Vector.X;
	Result.Y = Scalar * Vector.Y;
	Result.Z = Scalar * Vector.Z;

	return Result;
}

internal v3f
operator/(v3f Vector, f32 Scalar)
{
	v3f Result;

	Result.X = Vector.X / Scalar;
	Result.Y = Vector.Y / Scalar;
	Result.Z = Vector.Z / Scalar;

	return Result;
}

internal v3f
operator+=(v3f &A, v3f B)
{
	A = A + B;

	return A;
}

internal v3f
operator-=(v3f &A, v3f B)
{
	A = A - B;

	return A;
}

internal v3f
operator/=(v3f &Vector, f32 Scalar)
{
	Vector = Vector / Scalar;

	return Vector;
}

internal f32
Inner(v3f A, v3f B)
{
	f32 Result =
		A.X * B.X +
		A.Y * B.Y +
		A.Z * B.Z;

	return Result;
}

internal v3f
Cross(v3f A, v3f B)
{
	v3f Result;

	Result.X = A.Y * B.Z - A.Z * B.Y;
	Result.Y = A.Z * B.X - A.X * B.Z;
	Result.Z = A.X * B.Y - A.Y * B.X;

	return Result;
}

internal v3f
Hadamard(v3f A, v3f B)
{
	v3f Result;

	Result.X = A.X * B.X;
	Result.Y = A.Y * B.Y;
	Result.Z = A.Z * B.Z;

	return Result;
}

internal f32
LengthSqr(v3f Value)
{
	f32 Result = Inner(Value, Value);

	return Result;
}

internal f32
Length(v3f Value)
{
	f32 LenSqr = LengthSqr(Value);
	f32 Result = Sqrt(LenSqr);

	return Result;
}

internal v3f
NOZ(v3f Value)
{
	v3f Result;

	f32 LenSqr = LengthSqr(Value);

	if (LenSqr != 0.0f)
	{
		f32 InvLen = InvSqrt(LenSqr);

		Result = Value * InvLen;
	}
	else
	{
		Result = V3F(0.0f);
	}

	return Result;
}

// -- quaternion

internal quaternion
Quaternion(f32 X, f32 Y, f32 Z, f32 W)
{
	quaternion Result;

	Result.X = X;
	Result.Y = Y;
	Result.Z = Z;
	Result.W = W;

	return Result;
}

internal quaternion
QuaternionIdentity()
{
	quaternion Result = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);

	return Result;
}

internal quaternion
QuaternionFromEuler(f32 X, f32 Y, f32 Z)
{
	quaternion Result;

	f32 CosZ = Cos(0.5f * Z);
	f32 SinZ = Sin(0.5f * Z);
	f32 CosY = Cos(0.5f * Y);
	f32 SinY = Sin(0.5f * Y);
	f32 CosX = Cos(0.5f * X);
	f32 SinX = Sin(0.5f * X);

	Result.W = CosZ * CosY * CosX + SinZ * SinY * SinX;
	Result.X = CosZ * CosY * SinX - SinZ * SinY * CosX;
	Result.Y = SinZ * CosY * SinX + CosZ * SinY * CosX;
	Result.Z = SinZ * CosY * CosX - CosZ * SinY * SinX;

	return Result;
}

internal quaternion
QuaternionFromEuler(v3f Angles)
{
	quaternion Result = QuaternionFromEuler(Angles.X, Angles.Y, Angles.Z);

	return Result;
}

internal quaternion
QuaternionLookAt(v3f Dir, v3f Up)
{
	quaternion Result;

	// TODO

	return Result;
}

internal quaternion
operator-(quaternion A)
{
	quaternion Result;

	Result.W = -A.W;
	Result.X = -A.X;
	Result.Y = -A.Y;
	Result.Z = -A.Z;

	return Result;
}

internal quaternion
operator+(quaternion A, quaternion B)
{
	quaternion Result;

	Result.W = A.W + B.W;
	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;
	Result.Z = A.Z + B.Z;

	return Result;
}

internal quaternion
operator*(quaternion A, f32 Scalar)
{
	quaternion Result;

	Result.W = A.W * Scalar;
	Result.X = A.X * Scalar;
	Result.Y = A.Y * Scalar;
	Result.Z = A.Z * Scalar;

	return Result;
}

internal quaternion
operator*(f32 Scalar, quaternion A)
{
	quaternion Result;

	Result.W = A.W * Scalar;
	Result.X = A.X * Scalar;
	Result.Y = A.Y * Scalar;
	Result.Z = A.Z * Scalar;

	return Result;
}

internal quaternion
operator/(quaternion A, f32 Scalar)
{
	quaternion Result;

	Result.W = A.W / Scalar;
	Result.X = A.X / Scalar;
	Result.Y = A.Y / Scalar;
	Result.Z = A.Z / Scalar;

	return Result;
}

internal f32
Inner(quaternion A, quaternion B)
{
	f32 Result =
		A.X * B.X +
		A.Y * B.Y +
		A.Z * B.Z +
		A.W * B.W;

	return Result;
}

// -- m44

internal m44
M44Identity()
{
	m44 Result =
	{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return Result;
}

internal m44
operator+(m44 A, m44 B)
{
	m44 Result;

	for (s32 R = 0;
		R < 4;
		++R)
	{
		for (s32 C = 0;
			C < 4;
			++C)
		{
			Result.E[R][C] = A.E[R][C] + B.E[R][C];
		}
	}

	return Result;
}

internal m44
operator-(m44 A, m44 B)
{
	m44 Result;

	for (s32 R = 0;
		R < 4;
		++R)
	{
		for (s32 C = 0;
			C < 4;
			++C)
		{
			Result.E[R][C] = A.E[R][C] - B.E[R][C];
		}
	}

	return Result;
}

internal m44
operator*(m44 A, m44 B)
{
	m44 Result = {};

	for (u32 Row = 0;
		Row < 4;
		++Row)
	{
		for (u32 Column = 0;
			Column < 4;
			++Column)
		{
			for (u32 Index = 0;
				Index < 4;
				++Index)
			{
				Result.E[Row][Column] += A.E[Row][Index] * B.E[Index][Column];
			}
		}
	}

	return Result;
}

internal m44
Transpose(m44 Value)
{
	m44 Result;

	for (u32 Row = 0;
		Row < 4;
		++Row)
	{
		for (u32 Column = 0;
			Column < 4;
			++Column)
		{
			Result.E[Row][Column] = Value.E[Column][Row];
		}
	}

	return Result;
}

internal m44
M44Translation(f32 X, f32 Y, f32 Z)
{
	m44 Result =
	{
		1.0f, 0.0f, 0.0f,    X,
		0.0f, 1.0f, 0.0f,    Y,
		0.0f, 0.0f, 1.0f,    Z,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	return Result;
}

internal m44
M44Translation(v3f Translation)
{
	m44 Result = M44Translation(Translation.X, Translation.Y, Translation.Z);

	return Result;
}

internal m44
M44FromEuler(f32 X, f32 Y, f32 Z)
{
	f32 SinX = Sin(X);
	f32 SinY = Sin(Y);
	f32 SinZ = Sin(Z);

	f32 CosX = Cos(X);
	f32 CosY = Cos(Y);
	f32 CosZ = Cos(Z);

	m44 Result =
	{
		CosZ * CosY,  CosZ * SinY * SinX - SinZ * CosX,  CosZ * SinY * CosX + SinZ * SinX,  0.0f,
		SinZ * CosY,  SinZ * SinY * SinX + CosZ * CosX,  SinZ * SinY * CosX - CosZ * SinX,  0.0f,
			  -SinY,                       CosY * SinX,                       CosY * CosX,  0.0f,
			   0.0f,                              0.0f,                              0.0f,  1.0f
	};

	return Result;
}

internal m44
M44FromEuler(v3f Angles)
{
	m44 Result = M44FromEuler(Angles.X, Angles.Y, Angles.Z);

	return Result;
}

internal m44
M44FromQuaternion(quaternion Quaternion)
{
	f32 Y2 = 2.0f * Quaternion.Y * Quaternion.Y;
	f32 Z2 = 2.0f * Quaternion.Z * Quaternion.Z;
	f32 X2 = 2.0f * Quaternion.X * Quaternion.X;

	f32 XY = 2.0f * Quaternion.X * Quaternion.Y;
	f32 XZ = 2.0f * Quaternion.X * Quaternion.Z;

	f32 WX = 2.0f * Quaternion.W * Quaternion.X;
	f32 WY = 2.0f * Quaternion.W * Quaternion.Y;
	f32 WZ = 2.0f * Quaternion.W * Quaternion.Z;

	f32 YZ = 2.0f * Quaternion.Y * Quaternion.Z;

	m44 Result =
	{
		1.0f - Y2 - Z2,         XY - WZ,         XZ + WY,  0.0f,
			   XY + WZ,  1.0f - X2 - Z2,         YZ - WX,  0.0f,
			   XZ - WY,         YZ + WX,  1.0f - X2 - Y2,  0.0f,
				  0.0f,            0.0f,            0.0f,  1.0f,
	};

	return Result;
}

// -- basis

internal basis
BasisFromQuaternion(quaternion value)
{
	basis Result;

	f32 Y2 = 2.0f * value.Y * value.Y;
	f32 Z2 = 2.0f * value.Z * value.Z;
	f32 X2 = 2.0f * value.X * value.X;

	f32 XY = 2.0f * value.X * value.Y;
	f32 XZ = 2.0f * value.X * value.Z;

	f32 WX = 2.0f * value.W * value.X;
	f32 WY = 2.0f * value.W * value.Y;
	f32 WZ = 2.0f * value.W * value.Z;

	f32 YZ = 2.0f * value.Y * value.Z;

	Result.X = V3F(1.0f - Y2 - Z2,        XY + WZ,        XZ - WY);
	Result.Y = V3F(       XY - WZ, 1.0f - X2 - Z2,        YZ + WX);
	Result.Z = V3F(       XZ + WY,        YZ - WX, 1.0f - X2 - Y2);

	return Result;
}

#endif
