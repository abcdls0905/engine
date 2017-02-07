uniform sampler2D tex_BaseMap;
varying highp vec4 oDiffuse;
varying mediump vec2 oTexCoord;
uniform mediump float c_fAlphaValue;
uniform mediump vec3 c_outline_color;

void main()
{
#ifdef USE_PICTUREA8
	#ifdef USE_OUTLINE
		highp float alpha_map = texture2D(tex_BaseMap, oTexCoord).a;
		highp float color = clamp((alpha_map - 0.5) * 2.0, 0.0, 1.0);
		highp float alpha = clamp(alpha_map * 2.0, 0.0, 1.0);

		gl_FragColor.a = alpha;
		if(alpha == 1.0)
		{
			if(color > 0.5)
			{
				gl_FragColor.rgb = color * oDiffuse.rgb;
			}
			else
			{
				gl_FragColor.rgb = (1.0-color)*c_outline_color.rgb;
			}
		}
		else
		{
			gl_FragColor.rgb = color*c_outline_color.rgb;
		}
	#else
		gl_FragColor.w = texture2D(tex_BaseMap, oTexCoord).w; 
		gl_FragColor.w *= oDiffuse.w;
		gl_FragColor.xyz = oDiffuse.xyz;
	#endif	
#else
    gl_FragColor = oDiffuse;
	#ifdef USE_PICTURE 
		#ifdef USE_OUTLINE
			highp float alpha_map = texture2D(tex_BaseMap, oTexCoord).r;
			highp float color = clamp((alpha_map - 0.5) * 2.0, 0.0, 1.0);
			highp float alpha = clamp(alpha_map * 2.0, 0.0, 1.0);

			gl_FragColor.a = alpha;
			if(alpha == 1.0)
			{
				if(color > 0.5)
				{
					gl_FragColor.rgb = color * oDiffuse.rgb * texture2D(tex_BaseMap, oTexCoord).g ;
				}
				else
				{
					gl_FragColor.rgb = (1.0-color)*c_outline_color.rgb;
				}
			}
			else
			{
				gl_FragColor.rgb = color*c_outline_color.rgb;
			}
		#else
			gl_FragColor *= texture2D(tex_BaseMap, vec2(oTexCoord.x, oTexCoord.y));
		#endif
	#endif
#endif

#ifdef USE_FADE
	gl_FragColor.w *= c_fAlphaValue;    
#endif

#ifdef USE_BLACKWHITE
	mediump float brightness = dot(gl_FragColor.xyz, vec3(0.2125, 0.7154, 0.0721));
	gl_FragColor.xyz = vec3(brightness, brightness, brightness);
#endif
}