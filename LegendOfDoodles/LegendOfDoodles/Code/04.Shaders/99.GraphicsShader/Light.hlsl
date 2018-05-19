//--------------------------------------------------------------------------------------
#define MAX_LIGHTS			8 
#define MAX_MATERIALS		1

#define POINT_LIGHT			1
#define SPOT_LIGHT			2
#define DIRECTIONAL_LIGHT	3

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES
//#define _WITH_REFLECT

static float PI = 3.1415926535f;

struct MATERIAL
{
    float4 m_cAlbedo;
    float m_cSepcularPower;
    float m_cRoughness;
    float m_cMetalic;
    float m_cRefractiveIndex;
};

struct LIGHT
{
    float4 m_cAlbedo;
    float3 m_vPosition;
    float m_fFalloff;
    float3 m_vDirection;
    float m_fTheta; //cos(m_fTheta)
    float3 m_vAttenuation;
    float m_fPhi; //cos(m_fPhi)
    bool m_bEnable;
    int m_nType;
    float m_fRange;
    float padding;
};

cbuffer cbLights : register(b3)
{
    LIGHT gLights[MAX_LIGHTS];
    float4 gcGlobalAmbientLight;
};

cbuffer cbMaterial : register(b4)
{
    MATERIAL gMaterials;
};

// OrenNayar Diffuse Function
float4 OrenNayarDiffuse(float3 vLight, float3 vNormal, float3 vCamera, float roughness)
{
    float3 L = normalize(vLight);
    float3 N = vNormal;
    float3 V = normalize(vCamera);

    half LN = dot(L, N);
    half VN = dot(V, N);
    half result = saturate(LN);
    half soft_rim = saturate(1 - VN / 2);
    half fakey = pow(1 - result * soft_rim, 2);
    half fakey_magic = 0.62;
    fakey = fakey_magic - fakey * fakey_magic;

    return lerp(result, fakey, roughness);
}

// Cook-Torrance Specular Function
float4 CookTorranceSpecular(float3 vLight, float3 vNormal, float3 vCamera, float roughness)
{
    float3 L = normalize(vLight);
    float3 N = vNormal;
    float3 V = normalize(vCamera);
    float3 H = normalize(L + V);

    float NV = dot(N, V);
    float NH = dot(N, H);
    float VH = dot(V, H);
    float NL = dot(N, L);
    float LH = dot(L, H);

    float a = acos(NH);
    float e = -((a / roughness) * (a / roughness));
    float D = 1.0f * exp(e);

    float x = 2 * NH / VH;
    float G = min(1, min(x * NV, x * NL));

    float n = 20.0f;
    float g = sqrt(n * n + VH * VH - 1);
    float gpc = g + VH;
    float gmc = g - VH;
    float cgpc = VH * gpc - 1;
    float cgmc = VH * gmc + 1;
    float F = 0.5f * gmc * gmc * (1 + cgpc * cgmc / (cgmc * cgmc)) / (gpc * gpc);

    return max(0, (F * D * G) / (NV * NL));
}

float4 CookTorranceSpecular(float3 vLight, float3 vNormal, float3 vCamera, float roughness, float fresnel)
{
    float3 L = normalize(vLight);
    float3 N = vNormal;
    float3 V = normalize(vCamera);
    float3 H = normalize(L + V);

    float NV = dot(N, V);
    float NH = dot(N, H);
    float VH = dot(V, H);
    float NL = dot(N, L);
    float LH = dot(L, H);

    float a = acos(NH);
    float e = -((a / roughness) * (a / roughness));
    float D = 1.0f * exp(e);

    float x = 2 * NH / VH;
    float G = min(1, min(x * NV, x * NL));

    return max(0, (fresnel * D * G) / (NV * NL));
}

float4 DirectionalLight(int nIndex, float3 vNormal, float3 vCamera, float4 baseColor, float4 roughnessMetallicFresnel)
{
    float3 vToLight = -gLights[nIndex].m_vDirection;

    return gLights[nIndex].m_cAlbedo * ((1 - roughnessMetallicFresnel.g) * baseColor * OrenNayarDiffuse(vToLight, vNormal, vCamera, roughnessMetallicFresnel.r) +
				roughnessMetallicFresnel.g * baseColor * CookTorranceSpecular(vToLight, vNormal, vCamera, roughnessMetallicFresnel.r, roughnessMetallicFresnel.b));
}

float4 PointLight(int nIndex, float3 vPosition, float3 vNormal, float3 vCamera, float4 baseColor, float4 roughnessMetallicFresnel)
{
    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= gLights[nIndex].m_fRange)
    {
        vToLight /= fDistance;

        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        if (fAttenuationFactor != 0)
            return gLights[nIndex].m_cAlbedo * ((1 - roughnessMetallicFresnel.g) * baseColor * OrenNayarDiffuse(vToLight, vNormal, vCamera, roughnessMetallicFresnel.r) +
						roughnessMetallicFresnel.g * baseColor * CookTorranceSpecular(vToLight, vNormal, vCamera, roughnessMetallicFresnel.r, roughnessMetallicFresnel.b)) * fAttenuationFactor;
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 SpotLight(int nIndex, float3 vPosition, float3 vNormal, float3 vCamera, float4 baseColor, float4 roughnessMetallicFresnel)
{
    float3 vToLight = gLights[nIndex].m_vPosition - vPosition;
    float fDistance = length(vToLight);
    if (fDistance <= gLights[nIndex].m_fRange)
    {
        vToLight /= fDistance;

#ifdef _WITH_THETA_PHI_CONES
        float fAlpha = max(dot(normalize(-vToLight), gLights[nIndex].m_vDirection), 0.0f);
        float fSpotFactor = pow(max(((fAlpha - gLights[nIndex].m_fPhi) / (gLights[nIndex].m_fTheta - gLights[nIndex].m_fPhi)), 0.0f), gLights[nIndex].m_fFalloff);
#else
	float fSpotFactor = pow(max(dot(-vToLight, gLights[i].m_vDirection), 0.0f), gLights[i].m_fFalloff);
#endif
        float fAttenuationFactor = 1.0f / dot(gLights[nIndex].m_vAttenuation, float3(1.0f, fDistance, fDistance * fDistance));

        if (fSpotFactor != 0 && fAttenuationFactor != 0)
            return gLights[nIndex].m_cAlbedo * ((1 - roughnessMetallicFresnel.g) * baseColor * OrenNayarDiffuse(vToLight, vNormal, vCamera, roughnessMetallicFresnel.r) +
			roughnessMetallicFresnel.g * baseColor * CookTorranceSpecular(vToLight, vNormal, vCamera, roughnessMetallicFresnel.r, roughnessMetallicFresnel.b)) * fAttenuationFactor * fSpotFactor;
    }
    return (float4(0.0f, 0.0f, 0.0f, 0.0f));
}

float4 Lighting(float3 vPosition, float3 vNormal, float4 albedo, float4 baseColor, float4 roughnessMetallicFresnel)
{
    float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
    float3 vCamera = vCameraPosition - vPosition;
    float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (gLights[i].m_bEnable)
        {
            if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
            {
                cColor += DirectionalLight(i, vNormal, vCamera, baseColor, roughnessMetallicFresnel);
            }
            else if (gLights[i].m_nType == POINT_LIGHT)
            {
                cColor += PointLight(i, vPosition, vNormal, vCamera, baseColor, roughnessMetallicFresnel);
            }
            else if (gLights[i].m_nType == SPOT_LIGHT)
            {
                cColor += SpotLight(i, vPosition, vNormal, vCamera, baseColor, roughnessMetallicFresnel);
            }
        }
    }
    cColor += (gcGlobalAmbientLight * baseColor);
    cColor *= albedo;
    cColor.a = albedo.a;

    return (cColor);
}