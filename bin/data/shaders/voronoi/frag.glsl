#version 150

in vec4 color;
in vec4 pos;
in vec3 id;
out vec4 outColor;
uniform int mode;
uniform int interpolate;
uniform vec2 u_resolution;



void main(){
	outColor = vec4(1.0);
	if(id.x != interpolate)
	{
		if(mode == 1)
		{
			float curDepth = distance(vec4(pos.x, pos.y, pos.z, pos.w), gl_FragCoord.xyzw);
			vec4 curColor = vec4(1 - vec3((curDepth / u_resolution.x * 1.5)), 1.0);
			//outColor = curColor + color;
			outColor = curColor * 0.9 + color * 0.1;
		}
		else
		{
			outColor = vec4(id / 255., 1.);
		}
	}
	else outColor = vec4(1.0, 0., 0., 1.);
}