// Put the names of all group members here
// Daniel Vitor Ruiz - Matrikelnummer: 21983193 - Benutzerkennung: yx17upyj
// Joao Victor Tozatti Risso - Matrikelnummer: 21983403 - Benutzerkennung: ih94yxad

varying vec4 viewVec;
varying vec3 N;
varying vec4 P;
varying vec2 TexCoord;

uniform vec4 LightSource;  // updated each draw call
uniform vec4 Color;  // updated each draw call
uniform sampler2D Texture;

void main() {
	//TODO add phong lighting
	vec4 TextureColor = texture2D(Texture, TexCoord);
	float s = 10.0;
	vec3 lightDirVector = normalize( LightSource.xyz - P.xyz);
	vec3 D_reflectanceVector = reflect( -lightDirVector , N );

 	if(Color.a==1.0){//not background
		// vec3 basediff = Color.rgb;
		vec3 basediff = TextureColor.rgb;
		if(Color.b==0.1){//saturn rings and shuttle
			basediff =  Color.rgb;
		}
		vec3 D_Idiff = basediff * max( dot( N, lightDirVector ), 0.0 );
		basediff = basediff * vec3( 0.15, 0.15, 0.15 );
		vec3 kdiff = vec3( 0.15, 0.15, 0.15 );
		D_Idiff = clamp( D_Idiff, 0.0, 1.0 );

		vec3 kspec = vec3( 0.5, 0.5, 0.5 );
		vec3 D_Ispec = kspec * pow( max( dot( D_reflectanceVector, viewVec.xyz ), 0.0 ), s );
		D_Ispec = clamp( D_Ispec, 0.0, 1.0 );

		gl_FragColor = vec4( basediff + kdiff + D_Idiff + D_Ispec , 1.0 );
	}
	else
	{//background
		gl_FragColor = vec4( TextureColor.rgb, 1.0 );
	}
}
