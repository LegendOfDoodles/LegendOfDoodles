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
struct VS_EFFECT_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_EFFECT_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

VS_EFFECT_OUTPUT VSTexturedEffect(VS_EFFECT_INPUT input)
{
	VS_EFFECT_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxEffectObject), gmtxView), gmtxProjection);

	uint row = 0;	// x
	uint col = 0;	// y

	float2 newTexPos = input.uv;

	uint width, height, numMips;
	gtxtTexture.GetDimensions(0, width, height, numMips);

	row = width / 128;
	col = height / 128;

	//// u_Time == Animaiton Time (재생 시간) -> 끊기는 현상 발생?
	uint newtime = uint(floor(animationTime));
	uint u = newtime - uint(row * floor(1.0f * newtime / row)); //mod(newtime, row);
	uint v = newtime / row;

	newTexPos.x = (input.uv.x / row) + ((u / float(row)));
	newTexPos.y = (input.uv.y / col) + ((v / float(col)));

	output.uv = newTexPos.xy;

	return(output);
}

PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT PSTexturedEffect(VS_EFFECT_OUTPUT input)
{
	PS_MULTIPLE_RENDER_TARGETS_OUTPUT_DEFAULT output;

	output.color = gtxtTexture.Sample(wrapSampler, input.uv);
	output.normal = float4(0, 0, 0, 0);
	output.roughMetalFresnel = float4(0, 0, 0, 0);

	return (output);
}
