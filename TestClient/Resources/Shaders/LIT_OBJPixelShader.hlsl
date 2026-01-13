#include "LIT_OBJHeader.hlsli"

//	2パス目出力用シェーダー（予定）
//	現時点では、GBuffer画像を出力するだけ

//アルベドテクスチャ
Texture2D<float4> albedoTex : register(t0);
//法線テクスチャ
Texture2D<float4> normalTex : register(t1);
//ワールド座標テクスチャ
Texture2D<float4> worldTex  : register(t2);
//深度情報テクスチャ
Texture2D<float4> LdepthTex : register(t3);
//サンプラーは変更なし
SamplerState smp : register(s0);

//ピクセルのz値をライト空間の座標に変換する

float3 Diffuse(float3 lightDirection, float3 lightColor, float3 normal)
{
    // ピクセルの法線とライトの方向の内積を計算する
    float t = dot(normal, lightDirection) * -1.0f;

    // 内積の値を0以上の値にする
    t = max(0.0f, t);

    // 拡散反射光を計算する
    return lightColor * t;
}


float3 Specular(float3 lightDirection, float3 lightColor, float3 worldPos, float3 normal)
{
    // 反射ベクトルを求める
    float3 refVec = reflect(lightDirection, normal);

    // 光が当たったサーフェイスから視点に伸びるベクトルを求める
    float3 toEye = eyePos - worldPos;
    toEye = normalize(toEye);

    // 鏡面反射の強さを求める
    float t = dot(refVec, toEye);

    // 鏡面反射の強さを0以上の数値にする
    t = max(0.0f, t);

    // 鏡面反射の強さを絞る
    t = pow(t, 5.0f);

    // 鏡面反射光を求める
    return lightColor * t;
}


//並行光源の計算
float3 DirectionalShadeing(float4 lStatus, float3 normal, float3 worldPos, float3 color, uint useSpecular)
{
    float3 result;

    float t = max(0.0f, dot(normal, lStatus.xyz) * -1.0f);
    result = color * t;
    
    //スペキュラ
    if (useSpecular)
    {
        float3 toEye = normalize(eyePos - worldPos);
        float3 r = reflect(lStatus.xyz, normal);
        t = max(0.0f, dot(toEye, r));
        t = pow(t, 5.0f);
        result += color * t;
    }
    
    return result * lStatus.w;
}


float3 PointLightShadeing(float3 worldPos, float3 normal, PointLight point_light)
{
    //ポイントライトの方向を計算
    float3 point_light_dir = worldPos - point_light.pos;
    point_light_dir = normalize(point_light_dir);
    
    float3 diffuse = Diffuse(point_light_dir, point_light.rgb, normal);
    float3 specPoint = Specular(point_light_dir, point_light.rgb, worldPos, normal);

    //距離を計算
    float dist = length(worldPos - point_light.pos);
    //減衰
    float affect = 1.0f - 1.0f / point_light.power * dist;
    if(affect < 0.0f)
    {
        affect = 0.0f;
    }
    affect *= pow(affect, 3.0f);
    
    diffuse *= affect;
    specPoint *= affect;

    return diffuse + specPoint;
}

float3 SpotLightShading(float3 worldPos, float3 normal, SpotLight spot_light)
{

    float3 ligDir = worldPos - spot_light.pos;

    ligDir = normalize(ligDir);

    float3 diffSpotLight = Diffuse(
        ligDir, // ライトの方向
        spot_light.rgb, // ライトのカラー
        normal // サーフェイスの法線
    );


    float3 specSpotLight = Specular(
        ligDir, // ライトの方向
        spot_light.rgb, // ライトのカラー
        worldPos, // サーフェイスのワールド座標
        normal     // サーフェイスの法線
    );

    float3 distance = length(worldPos - spot_light.pos);

    float affect = 1.0f - 1.0f / spot_light.power * distance;

    if (affect < 0.0f)
    {
        affect = 0.0f;
    }

    affect = pow(affect, 3.0f);

    // step-10 影響率を乗算して影響を弱める
    diffSpotLight *= affect;
    specSpotLight *= affect;

    // step-11 入射光と射出方向の角度を求める
    // dot()を利用して内積を求める
    float angle = dot(ligDir, spot_light.spDirection);

    angle = clamp(angle, -1.0f, 1.0f);
    
    // dot()で求めた値をacos()に渡して角度を求める
    angle = abs(acos(angle));

    // step-12 角度による影響率を求める
    // 角度に比例して小さくなっていく影響率を計算する
    affect = 1.0f - 1.0f / spot_light.spAngle * angle;

    // 影響率がマイナスにならないように補正をかける
    if (affect < 0.0f)
    {
        affect = 0.0f;
    }

    // 影響の仕方を指数関数的にする。今回のサンプルでは0.5乗している
    affect = pow(affect, 0.5f);

    // step-13 角度による影響率を反射光に乗算して、影響を弱める
    diffSpotLight *= affect;
    specSpotLight *= affect;
    
    
    return diffSpotLight + specSpotLight;
}



float4 main(VSOutput input) : SV_TARGET
{
    //アルベド情報を取得
    float4 albedo = albedoTex.Sample(smp, input.uv);
    //法線情報取得
    float3 normal = normalTex.Sample(smp, input.uv).rgb;
    //ワールド座標取得
    float3 worldPos = worldTex.Sample(smp, input.uv).xyz;
    //深度情報取得
    float4 depth = LdepthTex.Sample(smp, input.uv);
    //スペキュラ強度取得
    float specPower = normalTex.Sample(smp, input.uv).a;
    
    //法線情報を復元
    normal = (normal * 2.0f) - 1.0f;

    float3 color = float3(1.0f, 1.0f, 1.0);
    
    float4 resultColor = albedo;
    
    float3 lig = float3(0, 0, 0);
    
    //並行光源の計算結果
    float3 result_light = float3(0, 0, 0);
    for (int i = 0; i < 4; i++)
    {
        if (lightStatus[i].isActive)
        {
            float3 l = float3(0, 0, 0);
            float3 d = float3(0, 0, 0);
            result_light += DirectionalShadeing(lightStatus[i].lightDir, normal, worldPos, color, lightStatus[i].isUseSpecular);
        }
    }
    
    //ポイントライト
    for (int k = 0; k < 256;k++)
    {
        if(pointlight_status[k].power > 0)
        {
            result_light += PointLightShadeing(worldPos, normal, pointlight_status[k]);
        }
    }
    
    //スポットライト
    for (int m = 0; m < 256;m++)
    {
        if(spotlight_status[m].power > 0.0f)
        {
            result_light += SpotLightShading(worldPos, normal, spotlight_status[m]);
        }
    }
    
    resultColor.xyz *= result_light;
    
    //拡散反射を計算
    //float3 lig = 0.0f;
    //float t = max(0.0f, dot(normal, lightStatus[0].xyz) * -1.0f);
    //lig = color * t;
    
    ////スペキュラ
    //float3 toEye = normalize(eyePos - worldPos);
    //float3 r = reflect(lightStatus[0].xyz, normal);
    //t = max(0.0f, dot(toEye, r));
    //t = pow(t, 5.0f);
    //lig += color * t;
    
    //resultColor = albedo;
    //resultColor.xyz *= lig;

    return resultColor;
}