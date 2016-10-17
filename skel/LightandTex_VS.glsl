// Put the names of all group members here
// Daniel Vitor Ruiz - Matrikelnummer: 21983193 - Benutzerkennung: yx17upyj
// Joao Victor Tozatti Risso - Matrikelnummer: 21983403 - Benutzerkennung: ih94yxad

uniform float Time;  // updated each frame by the application

uniform mat4 MVP;  // updated each draw call
uniform mat4 MV;  // updated each draw call
uniform mat3 NormalMatrix;  // updated each draw call

uniform vec4 LightSource;  // updated each draw call

attribute vec4 mypos;
attribute vec4 mynormal;
attribute vec2 mytexture;

varying vec4 lightVector;
varying vec4 viewVec;
varying vec3 N;
varying vec4 P;
varying vec2 TexCoord;

void main() {
	//TODO: Compute view vector, normal and vertex position
	P = MV * mypos;
	viewVec = normalize(P);
	N = normalize( NormalMatrix * mynormal.xyz );

  TexCoord= mytexture;
	// transform vertex with modelview and projection matrix
  gl_Position = MVP * mypos;

}
