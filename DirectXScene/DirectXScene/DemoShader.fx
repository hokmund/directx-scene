cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 vLightDirs[2];
    float4 vLightColors[2];
}

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Norm : NORMAL;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT)0;
    output.Pos = mul(input.Pos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Norm = mul(input.Norm, World);

    return output;
}

float4 PS(PS_INPUT input) : SV_Target
{
    float4 finalColor = 0;

    for (int i = 0; i < 2; i++)
    {
        finalColor += saturate(dot(vLightDirs[i], input.Norm) * vLightColors[i]);
    }

    finalColor.a = 1;

    return finalColor;
}