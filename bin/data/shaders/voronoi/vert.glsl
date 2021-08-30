#version 150

in vec4 position;
in vec4 instanceColor;
uniform mat4 modelViewProjectionMatrix;
uniform samplerBuffer tex;
out vec3 id;
out vec4 color;
out vec4 pos;

void main(){
    int index = gl_InstanceID*4;
    
    mat4 transformMatrix = mat4( 
        texelFetch(tex, index),
        texelFetch(tex, index+1),
        texelFetch(tex, index+2), 
        texelFetch(tex, index+3)
    );
	
	id = vec3(float(gl_InstanceID));
	pos = vec4(transformMatrix[3][0], transformMatrix[3][1], transformMatrix[3][2], transformMatrix[3][3]);
	color = instanceColor;
	gl_Position = modelViewProjectionMatrix * transformMatrix * position;
}