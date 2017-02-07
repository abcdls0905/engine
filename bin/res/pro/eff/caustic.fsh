//#define SHADOW 1

//要求硬件支持3D纹理采样
#extension GL_OES_texture_3D : enable
// cbPerObject
uniform mediump mat4 c_mtxViewInverse;
uniform mediump mat4 c_mtxProjInverse;
uniform mediump vec2 c_HalfPixelSize;
uniform mediump float c_fWaterHeight;
uniform mediump float c_fTimes;
uniform mediump vec2 c_BrightAndFocus;
uniform mediump vec3 c_vScaling;
uniform mediump vec4 c_LightColor;
uniform mediump vec3 c_vLightDir;

varying mediump vec2 oTex0;
varying mediump vec4 oViewScale;

uniform sampler2D tex_DepthMap;
uniform sampler3D tex_CaustMap;
uniform sampler2D tex_Shadow;
uniform sampler2D tex_NoiseMap;
uniform sampler2D tex_Normal;

void main()
{
mediump float depth = texture2D( tex_DepthMap,oTex0).x * 1024.0;
mediump vec3 normal = normalize(texture2D(tex_Normal, oTex0).xyz);
mediump vec3 view_pos = oViewScale.xyz * depth / oViewScale.z;
mediump vec4 world_pos = vec4(view_pos, 1.0) * c_mtxViewInverse;
mediump float brightness = clamp((c_fWaterHeight - world_pos.y) / c_BrightAndFocus.y,0.0,1.0) 
		* c_BrightAndFocus.x;
		
mediump vec3 caust_uv = vec3(world_pos.xz * c_vScaling.xz, c_fTimes);
	// 削弱序列化噪声
mediump vec3 Noise = texture2D(tex_NoiseMap, caust_uv.xy * 0.0625).xyz * 4.0;		
    caust_uv.z += Noise.x;
	
	mediump vec4 caust_color = texture3D(tex_CaustMap, caust_uv, 0.0);
	
#if defined(SHADOW)
brightness *= texture2D(tex_Shadow, oTex0, log(2048.0 / depth)).x;
#endif	
	mediump float inten = max(0.0, (dot(normal, c_vLightDir) + 1.0) * 0.5);
	mediump vec4 oColor = inten * caust_color * c_LightColor * brightness;

gl_FragColor = oColor * 0.5;
}