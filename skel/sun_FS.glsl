// Put the names of all group members here
// Daniel Vitor Ruiz - Matrikelnummer: 21983193 - Benutzerkennung: yx17upyj
// Joao Victor Tozatti Risso - Matrikelnummer: 21983403 - Benutzerkennung: ih94yxad

varying float height;
varying vec3 v_normal;
varying vec3 v_viewingVec;

void main() {

 	/**
		HINT: You can use your own sun-shaders from previous tasks!
	*/

    //gl_FragColor = vec4(1.0,1.0,0.0,0.0);

    float weight = clamp( dot( normalize(v_viewingVec), normalize(v_normal) ), 0.0, 1.0);
    float alpha = 1.0 - weight;
  	gl_FragColor = vec4( vec3(1.0,height,0.0) + alpha * vec3(1.0, 0.0, 0.0)  ,1.0);
}
