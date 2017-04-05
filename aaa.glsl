// https://threejs.org/examples/webgl_shaders_sky.html
precision highp float;
precision highp int;
#define saturate(a) clamp( a, 0.0, 1.0 )
float4 LinearToLinear( in float4 value ) {
return value;
}
float4 GammaToLinear( in float4 value, in float gammaFactor ) {
return float4( pow( value.xyz, vec3( gammaFactor ) ), value.w );
}
float4 LinearToGamma( in float4 value, in float gammaFactor ) {
return float4( pow( value.xyz, vec3( 1.0 / gammaFactor ) ), value.w );
}
float4 sRGBToLinear( in float4 value ) {
return float4( mix( pow( value.rgb * 0.9478672986 + vec3( 0.0521327014 ), vec3( 2.4 ) ), value.rgb * 0.0773993808, vec3( lessThanEqual( value.rgb, vec3( 0.04045 ) ) ) ), value.w );
}
float4 LinearTosRGB( in float4 value ) {
return float4( mix( pow( value.rgb, vec3( 0.41666 ) ) * 1.055 - vec3( 0.055 ), value.rgb * 12.92, vec3( lessThanEqual( value.rgb, vec3( 0.0031308 ) ) ) ), value.w );
}
float4 RGBEToLinear( in float4 value ) {
return float4( value.rgb * exp2( value.a * 255.0 - 128.0 ), 1.0 );
}
float4 LinearToRGBE( in float4 value ) {
float maxComponent = max( max( value.r, value.g ), value.b );
float fExp = clamp( ceil( log2( maxComponent ) ), -128.0, 127.0 );
return float4( value.rgb / exp2( fExp ), ( fExp + 128.0 ) / 255.0 );
}
float4 RGBMToLinear( in float4 value, in float maxRange ) {
return float4( value.xyz * value.w * maxRange, 1.0 );
}
float4 LinearToRGBM( in float4 value, in float maxRange ) {
float maxRGB = max( value.x, max( value.g, value.b ) );
float M      = clamp( maxRGB / maxRange, 0.0, 1.0 );
M            = ceil( M * 255.0 ) / 255.0;
return float4( value.rgb / ( M * maxRange ), M );
}
float4 RGBDToLinear( in float4 value, in float maxRange ) {
return float4( value.rgb * ( ( maxRange / 255.0 ) / value.a ), 1.0 );
}
float4 LinearToRGBD( in float4 value, in float maxRange ) {
float maxRGB = max( value.x, max( value.g, value.b ) );
float D      = max( maxRange / maxRGB, 1.0 );
D            = min( floor( D ) / 255.0, 1.0 );
return float4( value.rgb * ( D * ( 255.0 / maxRange ) ), D );
}
float4 emissiveMapTexelToLinear( float4 value ) { return LinearToLinear( value ); }
float4 linearToOutputTexel( float4 value ) { return LinearToLinear( value ); }
varying vec3 vWorldPosition;
varying vec3 vSunDirection;
varying float vSunfade;
varying vec3 vBetaR;
varying vec3 vBetaM;
varying float vSunE;
uniform float luminance;
uniform float mieDirectionalG;
uniform vec3 cameraPos;
const float pi = 3.141592653589793238462643383279502884197169;
const float n = 1.0003;
const float N = 2.545E25;
const float rayleighZenithLength = 8.4E3;
const float mieZenithLength = 1.25E3;
const vec3 up = vec3(0.0, 1.0, 0.0);
const float sunAngularDiameterCos = 0.999956676946448443553574619906976478926848692873900859324;
const float THREE_OVER_SIXTEENPI = 0.05968310365946075;
const float ONE_OVER_FOURPI = 0.07957747154594767;
float rayleighPhase( float cosTheta )
{
return THREE_OVER_SIXTEENPI * ( 1.0 + pow( cosTheta, 2.0 ) );
}
float hgPhase( float cosTheta, float g )
{
float g2 = pow( g, 2.0 );
float inverse = 1.0 / pow( 1.0 - 2.0 * g * cosTheta + g2, 1.5 );
return ONE_OVER_FOURPI * ( ( 1.0 - g2 ) * inverse );
}
const float A = 0.15;
const float B = 0.50;
const float C = 0.10;
const float D = 0.20;
const float E = 0.02;
const float F = 0.30;
const float whiteScale = 1.0748724675633854;
vec3 Uncharted2Tonemap( vec3 x )
{
return ( ( x * ( A * x + C * B ) + D * E ) / ( x * ( A * x + B ) + D * F ) ) - E / F;
}
void main() 
{
float zenithAngle = acos( max( 0.0, dot( up, normalize( vWorldPosition - cameraPos ) ) ) );
float inverse = 1.0 / ( cos( zenithAngle ) + 0.15 * pow( 93.885 - ( ( zenithAngle * 180.0 ) / pi ), -1.253 ) );
float sR = rayleighZenithLength * inverse;
float sM = mieZenithLength * inverse;
vec3 Fex = exp( -( vBetaR * sR + vBetaM * sM ) );
float cosTheta = dot( normalize( vWorldPosition - cameraPos ), vSunDirection );
float rPhase = rayleighPhase( cosTheta * 0.5 + 0.5 );
vec3 betaRTheta = vBetaR * rPhase;
float mPhase = hgPhase( cosTheta, mieDirectionalG );
vec3 betaMTheta = vBetaM * mPhase;
vec3 Lin = pow( vSunE * ( ( betaRTheta + betaMTheta ) / ( vBetaR + vBetaM ) ) * ( 1.0 - Fex ), vec3( 1.5 ) );
Lin *= mix( vec3( 1.0 ), pow( vSunE * ( ( betaRTheta + betaMTheta ) / ( vBetaR + vBetaM ) ) * Fex, vec3( 1.0 / 2.0 ) ), clamp( pow( 1.0 - dot( up, vSunDirection ), 5.0 ), 0.0, 1.0 ) );
vec3 direction = normalize( vWorldPosition - cameraPos );
float theta = acos( direction.y ); // elevation --> y-axis, [-pi/2, pi/2]
float phi = atan( direction.z, direction.x ); // azimuth --> x-axis [-pi/2, pi/2]
float2 uv = float2( phi, theta ) / float2( 2.0 * pi, pi ) + float2( 0.5, 0.0 );
vec3 L0 = vec3( 0.1 ) * Fex;
float sundisk = smoothstep( sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta );
L0 += ( vSunE * 19000.0 * Fex ) * sundisk;
vec3 texColor = ( Lin + L0 ) * 0.04 + vec3( 0.0, 0.0003, 0.00075 );
vec3 curr = Uncharted2Tonemap( ( log2( 2.0 / pow( luminance, 4.0 ) ) ) * texColor );
vec3 color = curr * whiteScale;
vec3 retColor = pow( color, vec3( 1.0 / ( 1.2 + ( 1.2 * vSunfade ) ) ) );
gl_FragColor.rgb = retColor;
gl_FragColor.a = 1.0;
}