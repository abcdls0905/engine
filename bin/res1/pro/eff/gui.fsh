precision mediump float;
precision mediump int;

uniform sampler2D tex_BaseMap;
uniform sampler2D tex_PaletteMap;
varying highp vec4 oDiffuse;
varying mediump vec2 oTexCoord;
uniform mediump float c_fAlphaValue;
uniform mediump vec3 c_outline_color;
uniform mediump float c_fLuminance;
	
void main()
{
	mediump vec4  output_color = vec4(0,0,0,0);
#ifdef USE_PICTUREA8
	#ifdef USE_OUTLINE
		highp float alpha_map = texture2D(tex_BaseMap, oTexCoord).w;
		highp float color = clamp((alpha_map - 0.5) * 2.0, 0.0, 1.0);
		highp float alpha = clamp(alpha_map * 2.0, 0.0, 1.0);
		output_color.w = alpha;
		//gl_FragColor.w = alpha;
		if(alpha == 1.0)
		{
			if(color > 0.5)
			{
				output_color.xyz = color * oDiffuse.xyz;
			}
			else
			{
				output_color.xyz = (1.0-color)*c_outline_color.xyz;
			}
		}
		else
		{
			output_color.xyz = c_outline_color.xyz;
		}
	#else
		output_color.w = texture2D(tex_BaseMap, oTexCoord).w; 
		output_color.xyz = oDiffuse.xyz;
	#endif	
		output_color.w = output_color.w * oDiffuse.w;
#else
    output_color = oDiffuse;
	#ifdef USE_PICTURE 
		#ifdef USE_OUTLINE
			highp float alpha_map = texture2D(tex_BaseMap, oTexCoord).x;
			highp float color = clamp((alpha_map - 0.5) * 2.0, 0.0, 1.0);
			highp float alpha = clamp(alpha_map * 2.0, 0.0, 1.0);

			output_color.w = alpha;
			if(alpha == 1.0)
			{
				if(color > 0.5)
				{
					output_color.xyz = color * oDiffuse.xyz * texture2D(tex_BaseMap, oTexCoord).y ;
				}
				else
				{
					output_color.xyz = (1.0-color)*c_outline_color.xyz;
				}
			}
			else
			{
				output_color.xyz = color*c_outline_color.xyz;
			}
		#else
			#ifdef USE_PALETTE
			
				output_color *= texture2D(tex_PaletteMap, vec2(texture2D(tex_BaseMap, vec2(oTexCoord.x, oTexCoord.y)).w , 0.0));
			#else
				output_color *= texture2D(tex_BaseMap, vec2(oTexCoord.x, oTexCoord.y));
			#endif
		#endif
	#endif
#endif

#ifdef USE_FADE
	output_color.w = output_color.w * c_fAlphaValue;    
#endif

#ifdef USE_LUMINANCE
	output_color.xyz = output_color.xyz * c_fLuminance;
#endif

#ifdef USE_BLACKWHITE
	mediump float brightness = dot(output_color.xyz, vec3(0.2125, 0.7154, 0.0721));
	output_color.xyz = vec3(brightness, brightness, brightness);
#endif

	gl_FragColor = output_color;
}