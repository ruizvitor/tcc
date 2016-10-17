// Put the names of all group members here
// Daniel Vitor Ruiz - Matrikelnummer: 21983193 - Benutzerkennung: yx17upyj
// Joao Victor Tozatti Risso - Matrikelnummer: 21983403 - Benutzerkennung: ih94yxad

uniform float Time;  // updated each frame by the application

uniform mat4 MVP;  // updated each draw call
uniform mat4 MV;  // updated each draw call
uniform mat3 NormalMatrix;  // updated each draw call

varying float height; // handed to the fragment shader
varying vec3 v_normal;
varying vec3 v_viewingVec;

float rand(vec2 n)
{
  return 0.5 + 0.5 *
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main()
{
	/**
		HINT: You can use your own sun-shaders from previous tasks!
	*/

  float vertexAmplitude = 1.5 * rand(gl_Vertex.xz) + 1.5 * rand(gl_Vertex.xy);
  float vertexPhaseShift = 7.0 * rand(gl_Vertex.xy) + 13.0 * rand(gl_Vertex.xz);

  float posChange = vertexAmplitude * sin( 2.0 * radians(Time) + vertexPhaseShift ) + vertexAmplitude;

  vec4 vertexPhase = posChange * normalize( vec4( gl_Normal, 0.0 ) );
  vec4 vertexPosition = gl_Vertex + vertexPhase;

  // transform vertex normal to camera space
  v_normal = NormalMatrix * gl_Normal;

  // transform vertex to camera space
  vertexPosition = MV * vertexPosition;

  v_viewingVec = -vertexPosition.xyz;


  height=length(vertexPhase)*0.3;

  // transform vertex position to clip space
  gl_Position = MVP * (gl_Vertex + vertexPhase);

}
