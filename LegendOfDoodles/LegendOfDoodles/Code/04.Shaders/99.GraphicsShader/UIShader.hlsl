#include "./Common.hlsl"
#include "./Light.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////
//  다중 랜더 타겟 용  PS출력
struct PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT
{
    float4 color : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 roughMetalFresnel : SV_TARGET2;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
struct VS_GAUGE_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_GAUGE_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_GAUGE_OUTPUT VSTexturedGauge(VS_GAUGE_INPUT input)
{
    VS_GAUGE_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGaugeObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedNumber(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    float2 newUV = input.uv;

    newUV.x = (input.uv.x / 10.f) + ((float) CurrentHP / 10.f);

    output.color = gtxtTexture.Sample(wrapSampler, newUV);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    if (input.uv.x > CurrentHP)
        discard;

    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

// Icon Gague
PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedIconGauge(VS_GAUGE_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    if (1 - input.uv.y > CurrentHP)
        discard;

    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}
