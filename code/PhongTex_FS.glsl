// varying vec4 viewVec;
// varying vec3 N;
// varying vec4 P;
// varying vec2 TexCoord;
//
// uniform vec4 LightSource;  // updated each draw call
// uniform vec4 Color;  // updated each draw call
// uniform sampler2D Texture;
//
// void main()
// {
// 	//phong lighting
// 	vec4 TextureColor = texture2D(Texture, TexCoord);
// 	float s = 10.0;
// 	vec3 lightDirVector = normalize( LightSource.xyz - P.xyz);
// 	vec3 D_reflectanceVector = reflect( -lightDirVector , N );
//
// 	vec3 basediff = Color.rgb;
// 	// vec3 basediff = TextureColor.rgb;
//
// 	vec3 D_Idiff = basediff * max( dot( N, lightDirVector ), 0.0 );
// 	//basediff = basediff * vec3( 0.15, 0.15, 0.15 );
// 	vec3 kdiff = vec3( 0.15, 0.15, 0.15 );
// 	D_Idiff = clamp( D_Idiff, 0.0, 1.0 );
//
// 	vec3 kspec = vec3( 0.5, 0.5, 0.5 );
// 	vec3 D_Ispec = kspec * pow( max( dot( D_reflectanceVector, viewVec.xyz ), 0.0 ), s );
// 	D_Ispec = clamp( D_Ispec, 0.0, 1.0 );
//
// 	gl_FragColor = vec4( basediff + kdiff + D_Idiff + D_Ispec , 1.0 );
//
// }

#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Ouput data
out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;

void main(){

	// Light emission properties
	// You probably want to put them as uniforms
	vec3 LightColor = vec3(1,1,1);
	float LightPower = 50.0f;

	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	// Distance to the light
	float distance = length( LightPosition_worldspace - Position_worldspace );

	// Normal of the computed fragment, in camera space
	vec3 n = normalize( Normal_cameraspace );
	// Direction of the light (from the fragment to the light)
	vec3 l = normalize( LightDirection_cameraspace );
	// Cosine of the angle between the normal and the light direction,
	// clamped above 0
	//  - light is at the vertical of the triangle -> 1
	//  - light is perpendicular to the triangle -> 0
	//  - light is behind the triangle -> 0
	float cosTheta = clamp( dot( n,l ), 0,1 );

	// Eye vector (towards the camera)
	vec3 E = normalize(EyeDirection_cameraspace);
	// Direction in which the triangle reflects the light
	vec3 R = reflect(-l,n);
	// Cosine of the angle between the Eye vector and the Reflect vector,
	// clamped to 0
	//  - Looking into the reflection -> 1
	//  - Looking elsewhere -> < 1
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	color =
		// Ambient : simulates indirect lighting
		MaterialAmbientColor +
		// Diffuse : "color" of the object
		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
		// Specular : reflective highlight, like a mirror
		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);

}
