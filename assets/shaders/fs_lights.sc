$input v_normal, v_color0

#include <bgfx_shader.sh>

uniform vec4 light0;

void main()
{
	vec3 lightDir = vec3(light0.xyz);
	float ndotl = dot(normalize(v_normal * -1.0), lightDir);
	float spec = 0.0;//pow(ndotl, 30.0);
	gl_FragColor = (v_color0 * 1.0) + vec4(pow(pow(v_color0.xyz, vec3_splat(2.2)) * ndotl + spec, vec3_splat(1.0/2.2)), 1.0);
}