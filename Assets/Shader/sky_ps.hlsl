
// https://threejs.org/examples/webgl_shaders_sky.html


cbuffer CB : register(b1)
{
	float3 cameraPos;
	float luminance;
	float mieDirectionalG;
}



static const float pi = 3.141592653589793238462643383279502884197169;
static const float n = 1.0003;
static const float N = 2.545E25;
static const float rayleighZenithLength = 8400.0;
static const float mieZenithLength = 1250.0;
static const float3 up = float3(0.0, 1.0, 0.0);
static const float sunAngularDiameterCos = 0.999956676946448443553574619906976478926848692873900859324;
static const float THREE_OVER_SIXTEENPI = 0.05968310365946075;
static const float ONE_OVER_FOURPI = 0.07957747154594767;



static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float whiteScale = 1.0748724675633854;

float3 Uncharted2Tonemap(float3 x)
{
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}



float rayleighPhase(float cosTheta)
{
	return THREE_OVER_SIXTEENPI * (1.0 + pow(cosTheta, 2.0));
}

float hgPhase(float cosTheta, float g)
{
	float g2 = pow((g), 2.0);
	float inverse = 1.0 / pow(abs(1.0 - 2.0 * g * cosTheta + g2), 1.5);
	return ONE_OVER_FOURPI * ((1.0 - g2) * inverse);
}

struct VS_OUTPUT {
	float4 position : SV_POSITION;

	float3 vWorldPosition : TEXCOORD0;
	float3 vSunDirection : TEXCOORD1;
	float vSunfade : TEXCOORD2;
	float3 vBetaR : TEXCOORD3;
	float3 vBetaM : TEXCOORD4;
	float vSunE : TEXCOORD5;
};

float4 PS(in VS_OUTPUT input) : SV_Target
{


	float3 vWorldPosition = input.vWorldPosition;
	float3 vSunDirection = input.vSunDirection;
    float vSunfade = input.vSunfade;
    float3 vBetaR = input.vBetaR;
    float3 vBetaM = input.vBetaM;
    float vSunE = input.vSunE;



	float zenithAngle = acos(max(0.0, dot(up, normalize(vWorldPosition - cameraPos))));



	float inverse = 1.0 / (cos(zenithAngle) + 0.15 * pow(abs(93.885 - ((zenithAngle * 180.0) / pi)), -1.253));


	float sR = rayleighZenithLength * inverse;
	float sM = mieZenithLength * inverse;
	float3 Fex = exp(-(vBetaR * sR + vBetaM * sM));
	float cosTheta = dot(normalize(vWorldPosition - cameraPos), vSunDirection);
	float rPhase = rayleighPhase(cosTheta * 0.5 + 0.5);
	float3 betaRTheta = vBetaR * rPhase;
	float mPhase = hgPhase(cosTheta, mieDirectionalG);
	float3 betaMTheta = vBetaM * mPhase;


	float3 Lin = pow(
	
	abs(
	
	vSunE * ((betaRTheta + betaMTheta) / (vBetaR + vBetaM)) * (1.0 - Fex)
		
		
	)
	, float3(1.5, 1.5, 1.5));


	Lin *= lerp(


		float3(1.0, 1.0, 1.0),

		pow((vSunE * ((betaRTheta + betaMTheta) / (vBetaR + vBetaM)) * Fex), float3(1.0 / 2.0, 1.0 / 2.0, 1.0 / 2.0)),

		clamp(pow(1.0 - dot(up, vSunDirection), 5.0), 0.0, 1.0)

	);


	float3 direction = normalize(vWorldPosition - cameraPos);
	float theta = acos(direction.y); // elevation --> y-axis, [-pi/2, pi/2]
	float phi = atan2(direction.z, direction.x); // azimuth --> x-axis [-pi/2, pi/2]
	float2 uv = float2(phi, theta) / float2(2.0 * pi, pi) + float2(0.5, 0.0);
	float3 L0 = float3(0.1, 0.1, 0.1) * Fex;
	float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta);
	L0 += (vSunE * 19000.0 * Fex) * sundisk;
	float3 texColor = (Lin + L0) * 0.04 + float3(0.0, 0.0003, 0.00075);
	float3 curr = Uncharted2Tonemap( ( log2( 2.0 / pow( luminance, 4.0 ) ) ) * texColor );
	float3 color = curr * whiteScale;

	color = texColor;

	float vv = 1.0 / (1.2 + (1.2 * vSunfade));

	float3 retColor = pow(abs(color), float3(vv, vv, vv));

	float4 result;


	result.rgb = color * vv;// float3(rayleighZenithLength, rayleighZenithLength, rayleighZenithLength);
	result.a = 1.0;





	return result;
}





