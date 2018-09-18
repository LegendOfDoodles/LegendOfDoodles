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

///////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_INPUT
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_TEXTURED_OUTPUT VSSprite(VS_TEXTURED_INPUT input)
{
    VS_TEXTURED_OUTPUT output;

    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGaugeObject), gmtxView), gmtxProjection);
    output.uv = input.uv;

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTextured(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;
    output.color = gtxtTexture.Sample(wrapSampler, input.uv);
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSLogo(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    int curImage = 1 - floor(input.uv.x + (1.f - CurrentHP));

    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, curImage));
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSButtons(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    int curImage = floor(CurrentHP * 2.f);

    output.color = gtxtTextures.Sample(wrapSampler, float3(input.uv, curImage));
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSCards(VS_TEXTURED_OUTPUT input)
{
    PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

    int curImage = floor(CurrentHP + 0.5f);
    int cardType = curImage / 4;
    curImage -= 4 * cardType;

    float2 newUV = float2(input.uv.x / 2.f + 0.5f * cardType, input.uv.y);

    output.color = gtxtTextures.Sample(wrapSampler, float3(newUV, curImage));
    output.normal = float4(0, 0, 0, 0);
    output.roughMetalFresnel = float4(0, 0, 0, 0);

    return (output);
}