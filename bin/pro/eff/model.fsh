uniform sampler2D tex_Diffuse;
uniform sampler2D tex_Bump;
uniform sampler2D tex_SpecularMap;
uniform samplerCube tex_ReflectionMap;
uniform sampler2D tex_LightMap;
uniform sampler2D tex_EmissiveMap;
uniform sampler2D tex_FilterMap;
uniform sampler2D tex_KeepoutMap;
uniform sampler2D tex_Shadow;

varying mediump vec4 oTex0;
varying mediump vec3 oNormal;

#ifdef FILTERMAP
	varying mediump vec2 oTexFilter;
#endif

#if defined NORMALMAP
varying mediump vec3 oTangent;
varying mediump vec3 oBinormal;
#endif

varying mediump vec4 oDiffuse;
varying mediump vec4 oViewToVertex;

varying mediump float oClipDistance;

#ifdef SHADOWMAP
varying mediump vec4 oTexCoordShadow;
#endif

#if defined FOGEXP || defined FOGLINEAR || defined HEIGHT_FOG
varying mediump vec4 oFog;
#endif

// PS Constants
uniform mediump vec4 c_FallOffParam;
uniform mediump vec4 c_MaterialAmbient;
uniform mediump vec4 c_MaterialAmbientEX;
uniform mediump vec4 c_SubSurfaceParam;

uniform mediump float c_fBumpScale;

//scene
uniform mediump vec3 c_vLightDir;
uniform mediump vec4 c_LightAmbient;
uniform mediump vec4 c_LightDiffuse;

//µ„π‚‘¥
uniform mediump vec4 c_PointLightPosition;
uniform mediump vec4 c_PointLightDiffuse;

uniform mediump mat4 c_mtxViewInverse;
uniform mediump vec4 c_MaterialDiffuse;
uniform mediump vec4 c_MaterialEmissive;
uniform mediump vec4 c_MaterialSpecular;
uniform mediump vec4 c_AppendColor;

uniform mediump float c_fSpecularPower;
uniform mediump float c_fAlphaRefValue;

uniform mediump vec3 c_vLightMapColorScale;

uniform mediump vec4 c_FilterParam;

mediump vec4 lerp(mediump vec4 u, mediump vec4 v, mediump float t)
{
	return u + (v-u)*t;
}

mediump vec3 lerp(mediump vec3 u, mediump vec3 v, mediump float t)
{
	return u + (v-u)*t;
}

mediump vec3 normal_calc(mediump vec3 map_normal, mediump float bump_scale)
{
	mediump vec3 v = vec3(0.5, 0.5, 1.0);
	map_normal = lerp(v, map_normal, bump_scale);
	map_normal = (map_normal * 2.0) - 1.0;
	return map_normal;
}

void main ()
{
    mediump vec4 shadow_Inten = vec4(1.0, 1.0, 1.0, 1.0);
	#ifdef SHADOWMAP
        mediump vec2 tex_uv = (vec2(0.5, 0.5) * (oTexCoordShadow.xy / oTexCoordShadow.w)) + vec2(0.5, 0.5);
        mediump float  z = oTexCoordShadow.z;
        shadow_Inten = texture2D(tex_Shadow, tex_uv);
	#endif
	
    mediump vec4 crTexDiffuse = texture2D(tex_Diffuse, oTex0.xy);

    mediump vec3 nor = normalize(oNormal);
    mediump vec3 light_dir = c_vLightDir;
    mediump vec4 diffuse_color = vec4(1, 1, 1, 1);
    #ifdef SHADOWMAP
        diffuse_color.xyz = max(dot(light_dir, nor), 0.0) * c_LightDiffuse.xyz * shadow_Inten.x;
    #else 
        diffuse_color.xyz = max(dot(light_dir, nor), 0.0) * c_LightDiffuse.xyz;
    #endif

    mediump vec3 specular_material = vec3(2.0, 2.0, 2.0);
    mediump float temp = 10.0;
    mediump float gloss = max(temp, 0.000001);
	mediump float specular_inten = pow(max(dot(reflect(normalize(oViewToVertex.xyz), nor), light_dir), 0.000001), gloss);
    #ifdef SHADOWMAP
        mediump vec3 specular_color = specular_inten * specular_material.xyz * c_LightDiffuse.xyz * shadow_Inten.x;
    #else 
        mediump vec3 specular_color = specular_inten * specular_material.xyz * c_LightDiffuse.xyz;
    #endif
    diffuse_color.xyz += specular_color.xyz;
    
    //gl_FragColor = crTexDiffuse;
    mediump vec4 lumine = vec4(0.6, 0.6, 0.6, 1.0);
    gl_FragColor = crTexDiffuse*(lumine + diffuse_color);
}