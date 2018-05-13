#pragma once
#include <DirectXMath.h>

using namespace DirectX;

/// <summary>
/// ����: ���� ������ ����ȭ �Ѵ�.
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-09
/// </summary>

#define	PI		3.141593
#define	EPSILON		1.0e-10f

#define CLAMP(x, low, high) max(low, min(high, x))

static int Wrap(int x, int low, int high)
{
	assert(low < high);
	const int n = (x - low) % (high - low);
	return (n >= 0) ? (n + low) : (n + high);
}

static bool IsZeroFloat(float val)
{
	if (val < 0) val *= (-1);
	if (val < EPSILON)
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////
//2���� ������ ����
inline int RandInt(int x, int y)
{
	assert(y >= x && "<RandInt>: y is less than x");
	return rand() % (y - x + 1) + x;
}

inline float RandFloat()
{
	return ((rand()) / (RAND_MAX + 1.0f));
}

inline float RandInRange(float x, float y)
{
	return x + RandFloat()*(y - x);
}

inline bool   RandBool()
{
	if (RandFloat() > 0.5f) return true;

	else return false;
}

inline double RandomClamped()
{
	return RandFloat() - RandFloat();
}

inline float RandGaussian(float mean = 0.0f, float standard_deviation = 1.0f)
{
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do
		{
			x1 = 2.0f * RandFloat() - 1.0f;
			x2 = 2.0f * RandFloat() - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0f);

		w = sqrt((-2.0f * log(w)) / w);
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return(mean + y1 * standard_deviation);
}

////////////////////////////////////////////////////////////////////////
//2���� ������ ����
namespace Vector2
{
	inline XMFLOAT2 XMVectorToFloat2(XMVECTOR& xmvVector)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, xmvVector);
		return xmf2Result;
	}

	inline bool Equal(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2)
	{
		return(xmf2Vector1.x == xmf2Vector2.x && xmf2Vector1.y == xmf2Vector2.y);
	}

	inline XMFLOAT2 ScalarProduct(XMFLOAT2& xmf2Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT2 xmf2Result;
		if (bNormalize)
			XMStoreFloat2(&xmf2Result, XMVector2Normalize(XMLoadFloat2(&xmf2Vector)) * fScalar);
		else
			XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector) * fScalar);
		return(xmf2Result);
	}

	inline XMFLOAT2 Add(const XMFLOAT2& xmf2Vector1, const XMFLOAT2& xmf2Vector2)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) + XMLoadFloat2(&xmf2Vector2));
		return(xmf2Result);
	}

	inline XMFLOAT2 Add(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2, float fScalar = 1)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) + (XMLoadFloat2(&xmf2Vector2) * fScalar));
		return(xmf2Result);
	}

	inline XMFLOAT2 Subtract(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2, bool bNormalize = false)
	{
		XMFLOAT2 xmf2Result;
		if (bNormalize)
			XMStoreFloat2(&xmf2Result,
				XMVector2Normalize(XMLoadFloat2(&xmf2Vector1) - XMLoadFloat2(&xmf2Vector2)));
		else
			XMStoreFloat2(&xmf2Result, XMLoadFloat2(&xmf2Vector1) - XMLoadFloat2(&xmf2Vector2));
		return(xmf2Result);
	}

	inline float DotProduct(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMVector2Dot(XMLoadFloat2(&xmf2Vector1), XMLoadFloat2(&xmf2Vector2)));
		return(xmf2Result.x);
	}

	inline XMFLOAT2 CrossProduct(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2, bool bNormalize = true)
	{
		XMFLOAT2 xmf2Result;
		if (bNormalize)
			XMStoreFloat2(&xmf2Result,
				XMVector2Normalize(XMVector2Cross(XMLoadFloat2(&xmf2Vector1), XMLoadFloat2(&xmf2Vector2))));
		else
			XMStoreFloat2(&xmf2Result, XMVector2Cross(XMLoadFloat2(&xmf2Vector1), XMLoadFloat2(&xmf2Vector2)));
		return(xmf2Result);
	}

	inline XMFLOAT2 Normalize(const XMFLOAT2& xmf2Vector)
	{
		XMFLOAT2 m_xmf3Normal;
		XMStoreFloat2(&m_xmf3Normal, XMVector2Normalize(XMLoadFloat2(&xmf2Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(XMFLOAT2& xmf2Vector)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMVector2Length(XMLoadFloat2(&xmf2Vector)));
		return(xmf2Result.x);
	}

	inline float Distance(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMVector2Length(XMLoadFloat2(&xmf2Vector1) - XMLoadFloat2(&xmf2Vector2)));
		return(xmf2Result.x);
	}

	inline float DistanceSquare(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2)
	{
		float x = abs(xmf2Vector2.x - xmf2Vector1.x);
		float y = abs(xmf2Vector2.y - xmf2Vector1.y);
		return(x * x + y * y);
	}

	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector2AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}

	inline float Angle(XMFLOAT2& xmf2Vector1, XMFLOAT2& xmf2Vector2)
	{
		return(Angle(XMLoadFloat2(&xmf2Vector1), XMLoadFloat2(&xmf2Vector2)));
	}

	inline XMFLOAT2 TransformNormal(XMFLOAT2& xmf2Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMVector2TransformNormal(XMLoadFloat2(&xmf2Vector), xmmtxTransform));
		return(xmf2Result);
	}

	inline XMFLOAT2 TransformCoord(XMFLOAT2& xmf2Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT2 xmf2Result;
		XMStoreFloat2(&xmf2Result, XMVector2TransformCoord(XMLoadFloat2(&xmf2Vector), xmmtxTransform));
		return(xmf2Result);
	}

	inline XMFLOAT2 TransformCoord(XMFLOAT2& xmf2Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf2Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}

	inline bool IsZero(XMFLOAT2& xmf2Vector)
	{
		if (::IsZeroFloat(xmf2Vector.x) && ::IsZeroFloat(xmf2Vector.y))
		{
			return(true);
		}
		return(false);
	}
}

////////////////////////////////////////////////////////////////////////
//3���� ������ ����
namespace Vector3
{
	inline XMFLOAT3 XMVectorToFloat3(XMVECTOR& xmvVector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, xmvVector);
		return(xmf3Result);
	}

	inline bool Equal(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		return(xmf3Vector1.x == xmf3Vector2.x && xmf3Vector1.y == xmf3Vector2.y && xmf3Vector1.z == xmf3Vector2.z);
	}

	inline XMFLOAT3 ScalarProduct(XMFLOAT3& xmf3Vector, float fScalar, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)) * fScalar);
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector) * fScalar);
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(const XMFLOAT3& xmf3Vector1, const XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline XMFLOAT3 Add(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, float fScalar = 1)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) + (XMLoadFloat3(&xmf3Vector2) * fScalar));
		return(xmf3Result);
	}

	inline XMFLOAT3 Subtract(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = false)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result,
				XMVector3Normalize(XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2)));
		else
			XMStoreFloat3(&xmf3Result, XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2));
		return(xmf3Result);
	}

	inline float DotProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Dot(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline XMFLOAT3 CrossProduct(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2, bool bNormalize = true)
	{
		XMFLOAT3 xmf3Result;
		if (bNormalize)
			XMStoreFloat3(&xmf3Result,
				XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2))));
		else
			XMStoreFloat3(&xmf3Result, XMVector3Cross(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result);
	}

	inline XMFLOAT3 Normalize(const XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 m_xmf3Normal;
		XMStoreFloat3(&m_xmf3Normal, XMVector3Normalize(XMLoadFloat3(&xmf3Vector)));
		return(m_xmf3Normal);
	}

	inline float Length(XMFLOAT3& xmf3Vector)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector)));
		return(xmf3Result.x);
	}

	inline float Distance(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3Length(XMLoadFloat3(&xmf3Vector1) - XMLoadFloat3(&xmf3Vector2)));
		return(xmf3Result.x);
	}

	inline float DistanceSquare(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		float x = abs(xmf3Vector2.x - xmf3Vector1.x);
		float y = abs(xmf3Vector2.y - xmf3Vector1.y);
		float z = abs(xmf3Vector2.z - xmf3Vector1.z);
		return(x * x + y * y + z * z);
	}

	inline float Angle(XMVECTOR& xmvVector1, XMVECTOR& xmvVector2)
	{
		XMVECTOR xmvAngle = XMVector3AngleBetweenNormals(xmvVector1, xmvVector2);
		return(XMConvertToDegrees(acosf(XMVectorGetX(xmvAngle))));
	}

	inline float Angle(XMFLOAT3& xmf3Vector1, XMFLOAT3& xmf3Vector2)
	{
		return(Angle(XMLoadFloat3(&xmf3Vector1), XMLoadFloat3(&xmf3Vector2)));
	}

	inline XMFLOAT3 TransformNormal(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformNormal(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMMATRIX& xmmtxTransform)
	{
		XMFLOAT3 xmf3Result;
		XMStoreFloat3(&xmf3Result, XMVector3TransformCoord(XMLoadFloat3(&xmf3Vector), xmmtxTransform));
		return(xmf3Result);
	}

	inline XMFLOAT3 TransformCoord(XMFLOAT3& xmf3Vector, XMFLOAT4X4& xmmtx4x4Matrix)
	{
		return(TransformCoord(xmf3Vector, XMLoadFloat4x4(&xmmtx4x4Matrix)));
	}

	inline bool IsZero(XMFLOAT3& xmf3Vector)
	{
		if (::IsZeroFloat(xmf3Vector.x) && ::IsZeroFloat(xmf3Vector.y) && ::IsZeroFloat(xmf3Vector.z))
		{
			return(true);
		}
		return(false);
	}
}

////////////////////////////////////////////////////////////////////////
//4���� ������ ����
namespace Vector4
{
	inline XMFLOAT4 Add(const XMFLOAT4& xmf4Vector1, const XMFLOAT4& xmf4Vector2)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + XMLoadFloat4(&xmf4Vector2));
		return(xmf4Result);
	}

	inline XMFLOAT4 Add(XMFLOAT4& xmf4Vector1, XMFLOAT4& xmf4Vector2, float fScalar = 1)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, XMLoadFloat4(&xmf4Vector1) + (XMLoadFloat4(&xmf4Vector2) * fScalar));
		return(xmf4Result);
	}

	inline XMFLOAT4 Multiply(float fScalar, XMFLOAT4& xmf4Vector)
	{
		XMFLOAT4 xmf4Result;
		XMStoreFloat4(&xmf4Result, fScalar * XMLoadFloat4(&xmf4Vector));
		return(xmf4Result);
	}
}

////////////////////////////////////////////////////////////////////////
//����� ����
namespace Matrix4x4
{
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMFLOAT4X4& xmmtx4x4Matrix1, XMMATRIX& xmmtxMatrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMLoadFloat4x4(&xmmtx4x4Matrix1) * xmmtxMatrix2);
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Multiply(XMMATRIX& xmmtxMatrix1, XMFLOAT4X4& xmmtx4x4Matrix2)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, xmmtxMatrix1 * XMLoadFloat4x4(&xmmtx4x4Matrix2));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 Inverse(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixInverse(NULL, XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}
	inline XMFLOAT4X4 Lerp(XMFLOAT4X4& xmmtx4x4Matrix1, XMFLOAT4X4& xmmtx4x4Matrix2, float single)
	{
		XMFLOAT4X4 xmmtx4x4Result;

		XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix1._11));
		XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix1._21));
		XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix1._31));
		XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix1._41));

		XMVECTOR y1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix2._11));
		XMVECTOR y2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix2._21));
		XMVECTOR y3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix2._31));
		XMVECTOR y4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&xmmtx4x4Matrix2._41));

		x1 = XMVectorLerp(x1, y1, single);
		x2 = XMVectorLerp(x2, y2, single);
		x3 = XMVectorLerp(x3, y3, single);
		x4 = XMVectorLerp(x4, y4, single);

		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&xmmtx4x4Result._11), x1);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&xmmtx4x4Result._21), x2);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&xmmtx4x4Result._31), x3);
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&xmmtx4x4Result._41), x4);

		return(xmmtx4x4Result);
	}


	inline XMFLOAT4X4 Transpose(XMFLOAT4X4& xmmtx4x4Matrix)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixTranspose(XMLoadFloat4x4(&xmmtx4x4Matrix)));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 PerspectiveFovLH(float FovAngleY, float AspectRatio, float NearZ,
		float FarZ)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixPerspectiveFovLH(FovAngleY, AspectRatio, NearZ, FarZ));
		return(xmmtx4x4Result);
	}

	inline XMFLOAT4X4 LookAtLH(XMFLOAT3& xmf3EyePosition, XMFLOAT3& xmf3LookAtPosition,
		XMFLOAT3& xmf3UpDirection)
	{
		XMFLOAT4X4 xmmtx4x4Result;
		XMStoreFloat4x4(&xmmtx4x4Result,
			XMMatrixLookAtLH(
				XMLoadFloat3(&xmf3EyePosition),
				XMLoadFloat3(&xmf3LookAtPosition),
				XMLoadFloat3(&xmf3UpDirection)
			)
		);
		return(xmmtx4x4Result);
	}
}