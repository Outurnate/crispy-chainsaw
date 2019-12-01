$input v_color0, v_texcoord0

#include <bgfx_shader.sh>

SAMPLER2D(s_texColor, 0);

uniform vec4 blendColor;

void main()
{
	vec4 samp = texture2D(s_texColor, v_texcoord0.xy);
	gl_FragColor = vec4((samp.rgb * samp.a) + (blendColor.rgb * (samp.a - 1.0f)), min(samp.a, blendColor.a));
}