//#define WAVELESS 1
//#define USE_REFLECTION 1
//#define USE_REFRACTION 1
//#define USE_DEPTHMAP 1
//#define SUN_REFLECTION 1
//#define BORDER 1
//#define UNDERWATER 1
//#define WHITEWATER 1
//#define FOGLINEAR 1
//#define FOGEXP 1
//#define USE_INTZ 1


// cbPerObject
uniform mediump mat4 c_Detail1Param;
uniform mediump mat4 c_Detail2Param;
uniform mediump mat4 c_Detail3Param;
uniform mediump mat4 c_Detail4Param;
uniform mediump vec3 c_vChunkPos;
uniform mediump vec4 c_ZoneOffsetAndSize;

uniform mediump float c_fBaseSpecularLevel;
uniform mediump float c_fBaseSpecularPower;
uniform mediump float c_fBaseHeightScale;
uniform mediump vec4  c_BlendSpecularLevel;
uniform mediump vec4  c_BlendSpecularPower;
uniform mediump vec4  c_BlendHeightScale;


// cbPerGroup
uniform mediump mat4  c_mtxViewProj;
uniform mediump float c_fTime;
uniform mediump vec4  c_ViewPosition;
uniform mediump vec4  c_WaveParam0;
uniform mediump vec4  c_WaveParam1;
uniform mediump vec4  c_WaveParam2;
uniform mediump vec4  c_WaveParam3;
uniform mediump vec4  c_WaveParam4;
uniform mediump vec4  c_WaveParam5;
uniform mediump vec4  c_WaveParam6;
uniform mediump vec4  c_WaveParam7;
uniform mediump float c_NormalmapScale0;
uniform mediump vec2  c_NormalmapSpeed0;
uniform mediump float c_NormalmapScale1;
uniform mediump vec2  c_NormalmapSpeed1;
uniform mediump float c_NormalmapScale2;
uniform mediump vec2  c_NormalmapSpeed2;
uniform mediump float c_NormalmapScale3;
uniform mediump vec2  c_NormalmapSpeed3;
uniform mediump vec3  c_SunDirection;
uniform mediump vec4  c_UnderWater;
uniform mediump vec4  c_vReferPos; 
uniform mediump vec3  c_vLightDir; 
uniform mediump vec4  c_OceanColor; 
uniform mediump float c_DistanceScale;
uniform mediump float c_ReflectionInten;
uniform mediump float c_RefractionInten;
uniform mediump float c_ReflectionWeight;
uniform mediump float c_RefractionWeight;
uniform mediump vec4  c_SunColorStart;
uniform mediump vec4  c_SunColorEnd; 
uniform mediump float c_SunLevel;
uniform mediump vec4  c_SkyColor;
uniform mediump vec4  c_DepthParam; 
uniform mediump float c_ReflectionSunInten;
uniform mediump vec4  c_FogColor;
uniform mediump vec4  c_FogParam;
uniform mediump vec4  c_FogExpParam;
uniform mediump vec2  c_PixelSize;


uniform mediump vec2  c_RefractUVScale;
uniform mediump vec2  c_DepthUVScale;
//uniform mediump vec2  c_ReflectUVScale;

varying mediump vec4 oNormalMapCoord0;
varying mediump vec4 oNormalMapCoord1;
varying mediump vec4 oViewToVertex;
varying mediump vec3 oSunToVertex;
varying mediump vec4 oFovCoord;
varying mediump vec4 oWorldPos;

varying mediump vec3 oNormal;

varying mediump vec4 oPos;

uniform sampler2D tex_SeaReflect;
uniform sampler2D tex_WaterNormalmap1;
uniform sampler2D tex_WaterNormalmap2;
uniform sampler2D tex_WaterNormalmap3;
uniform sampler2D tex_WaterNormalmap4;
uniform sampler2D tex_SceneRefract;
uniform mediump sampler2D tex_Depthmap;
uniform sampler2D tex_WhiteWater;

uniform sampler2D tex_ReflectDepth;

#if defined(USE_DEPTHMAP)
/*
mediump float getlinerdepth(in sampler2D sdepth,in mediump vec2 UV)
{
   mediump float zvalue = texture2D( sdepth , UV ).x;
   return zvalue =  1.0 / (c_DepthParam.x -zvalue*c_DepthParam.y );
}
*/

//DecodeDepthTexture
mediump float DecodeFloatRGBA(mediump vec4 rgba ) 
{
	return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}

mediump float getlinerdepth(in mediump sampler2D sdepth,in mediump vec2 UV)
{
  return DecodeFloatRGBA(texture2D( sdepth , UV ))*1024.0;
}

#endif


void main()
{
mediump vec4 oColor = vec4(1.0,0.0,0.0,0.0);

mediump float ViewDepth = oViewToVertex.y;
mediump vec2 tex_uv = oPos.xy/oPos.w;

mediump vec4 iViewToVertex = oViewToVertex;
iViewToVertex.xyz = normalize(oViewToVertex.xyz);

// 水面扰动
mediump vec4 pixel_normal0 = texture2D(tex_WaterNormalmap1, oNormalMapCoord0.xy) * 2.0 - 1.0;
mediump vec4 pixel_normal1 = texture2D(tex_WaterNormalmap2, oNormalMapCoord0.zw) * 2.0 - 1.0;


#if defined(WAVELESS)	
	mediump vec3 pixel_normal  = normalize(pixel_normal0.xyz + pixel_normal1.xyz);
#else
	mediump vec4 pixel_normal2 = texture2D(tex_WaterNormalmap3,oNormalMapCoord1.xy) * 2.0 - 1.0;
	mediump vec4 pixel_normal3 = texture2D(tex_WaterNormalmap4,oNormalMapCoord1.zw) * 2.0 - 1.0;
	mediump vec3 pixel_normal  = (pixel_normal0.xyz + pixel_normal1.xyz) + (pixel_normal2.xyz + pixel_normal3.xyz);      
	pixel_normal  = normalize(pixel_normal);      
#endif

mediump vec3 normalbias = pixel_normal.xzy;
normalbias.y = 0.0;

mediump vec3 worldNormal = normalize(oNormal + normalbias);
mediump vec3 ReflectionNormal = normalize(normalbias * c_ReflectionInten + vec3(0.0, 1.0, 0.0));   

mediump	float nearcam = 1.0;
mediump	float bias = 1.0;

#if defined(USE_DEPTHMAP)
	nearcam = clamp(oFovCoord.z * 16.0,0.0,1.0);
	bias = nearcam; 

	#if !defined(UNDERWATER)
	mediump vec2  depthuv = tex_uv;
	depthuv.y = 1.0 -depthuv.y;
	depthuv = depthuv;// *c_DepthUVScale;
	mediump float Depth = getlinerdepth(tex_Depthmap,depthuv);
	bias = clamp((Depth - oFovCoord.w) * 0.25 * nearcam,0.0,1.0);
	
	Depth = getlinerdepth(tex_Depthmap,depthuv + worldNormal.xz * c_RefractionInten * bias );
	bias = clamp((Depth - oFovCoord.w) * 0.25 * nearcam,0.0,1.0);
	Depth = getlinerdepth(tex_Depthmap,depthuv + worldNormal.xz * c_RefractionInten * bias );

	mediump float DepthInWater = Depth - oFovCoord.w;
	
	mediump float waterDepth = (Depth - oFovCoord.w) * abs(iViewToVertex.y);
	mediump float border = waterDepth * 8.0;
	#endif
#endif
  
#if defined(UNDERWATER)
	mediump vec3 RefractDir = refract(iViewToVertex.xyz, ReflectionNormal, 1.0 + 0.05 * clamp(ViewDepth,0.0,1.0));

	#if defined(USE_REFRACTION)
	mediump vec4 crRefract = texture2D(tex_SceneRefract, tex_uv + worldNormal.xz * c_RefractionInten * bias);  
	#else
	mediump vec4 crRefract = vec4(0.0, 0.0, 0.0, 0.0); 
	#endif
	
	#if defined(USE_REFLECTION)
	mediump vec2 reflect_uv = tex_uv;
	reflect_uv.y = 1.0 - reflect_uv.y;
	
		#if defined(USE_DEPTHMAP)
		 mediump vec3 ReflectDir = reflect(iViewToVertex.xyz, ReflectionNormal);
		 ReflectDir.y = -ReflectDir.y;
		 mediump float zvalue = 100.0;
		 mediump vec4 ReflectTexcoord = vec4(ReflectDir * zvalue + oWorldPos.xyz, 1.0)*c_mtxViewProj;
		 reflect_uv += (ReflectTexcoord.xy / ReflectTexcoord.w - oFovCoord.xy / oFovCoord.w) * 0.5;
		 mediump vec4 crReflect = texture2D(tex_SeaReflect, reflect_uv);
		#else
		 mediump vec4 crReflect = texture2D(tex_SeaReflect, reflect_uv + ReflectionNormal.xz * bias * 0.1); 
		#endif
	#else
	  mediump vec4 crReflect = vec4(c_SkyColor.xyz, 1.0);
	#endif
	
	mediump float lrp = (1.0 - dot(-RefractDir.xyz, ReflectionNormal));
	mediump float lerpvalue = clamp(c_ReflectionWeight + pow(lrp, 6.0),0.0,1.0);
    oColor = crRefract * (1.0 -lerpvalue) + crReflect * lerpvalue;
#else
    #if defined(USE_REFRACTION)
     mediump vec2 refract_uv = tex_uv;
     refract_uv.y = 1.0 - refract_uv.y;
     refract_uv *= c_RefractUVScale;
     mediump vec4 crRefract = texture2D(tex_SceneRefract, refract_uv + worldNormal.xz * c_RefractionInten * bias);
    #else
     mediump vec4 crRefract = vec4(0.0, 0.0, 0.0, 0.0);
    #endif
    
    #if defined(USE_REFLECTION)
     mediump vec2 reflect_uv = tex_uv;
     
	 #if defined(USE_DEPTHMAP)
		  mediump vec3 ReflectDir = reflect(iViewToVertex.xyz, ReflectionNormal);
		  ReflectDir.y = -ReflectDir.y;
     
		 mediump float zvalue = 100.0;
		 mediump vec4 ReflectTexcoord = vec4(ReflectDir * zvalue + oWorldPos.xyz, 1.0)*c_mtxViewProj;
		 reflect_uv += (ReflectTexcoord.xy / ReflectTexcoord.w - oFovCoord.xy / oFovCoord.w) * 0.5;
		 mediump vec4 crReflect = texture2D(tex_SeaReflect, reflect_uv);

     #else
         mediump vec4 crReflect = texture2D(tex_SeaReflect, reflect_uv + ReflectionNormal.xz * bias * 0.1);
         
     #endif
     
    #else
     mediump vec4 crReflect = vec4(c_SkyColor.xyz, 1.0);
    #endif

    
 	// 雾效开
	mediump vec4  OceanColor = vec4(c_OceanColor.xyz, 1.0);
	mediump vec4  FogColor = vec4(0.0, 0.0, 0.0, 1.0);
	mediump float fogvalue;   
    
	#if defined(FOGEXP)
		fogvalue = exp(- iViewToVertex.w * c_FogExpParam.w);
		FogColor.xyz = c_FogExpParam.xyz*(1.0-fogvalue) + FogColor.xyz*fogvalue;
		FogColor.w *= fogvalue;
	#endif
	
	#if defined(FOGLINEAR)
		fogvalue = clamp((iViewToVertex.w - c_FogParam.x) * c_FogParam.y,0.0,1.0);
		FogColor.xyz = c_FogColor.xyz * fogvalue;
		FogColor.w = 1.0 - fogvalue;
	#endif
	
	#if defined(REFLECT)
		#if defined(FOGLINEAR) || defined(FOGEXP)
			FogColor.w += 0.0000001;
			crRefract = (crRefract - FogColor) / FogColor.w;
			crReflect = clamp((crReflect - FogColor) / FogColor.w,0.0,1.0);
		#endif
	#endif	
	
	#if defined(USE_DEPTHMAP)
		mediump float factorRefraction = clamp(1.0 / exp(waterDepth * c_RefractionWeight),0.0,1.0);
		mediump float waterfog = clamp(1.0 / exp(DepthInWater * c_RefractionWeight),0.0,1.0);

		oColor = c_OceanColor * (1.0 - pow(exp(-waterDepth * c_RefractionWeight
			* 2.0), DepthInWater / waterDepth))
			+ crRefract * factorRefraction * waterfog;
	#else
		mediump float tempdepth = clamp((oWorldPos.y - oWorldPos.w) * 0.5,0.0,1.0) * 10.0;
		mediump float lerptemp1 =  exp(-c_RefractionWeight * tempdepth);
		oColor = OceanColor*(1.0 - lerptemp1) + crRefract * lerptemp1;
	#endif	
	
	// 入视角大时反射强，小时反射大
	mediump float lrp = 1.0 - dot(- iViewToVertex.xyz, worldNormal);

    mediump float lerptemp2 = clamp(c_ReflectionWeight + pow(lrp, 6.0),0.0,1.0) * nearcam;
	oColor = oColor*(1.0 - lerptemp2) + crReflect*lerptemp2; 
	
	// 反射阳光
	#if defined(SUN_REFLECTION)
		// 反射阳光
		mediump vec3 sunNormal = normalize(vec3(0.0, 0.5, 0.0) + normalbias * c_ReflectionSunInten);
		mediump vec3 ViewToVertexReflectionSun = reflect(normalize(iViewToVertex.xyz), sunNormal);
		mediump float suninten = max(dot(ViewToVertexReflectionSun, normalize(oSunToVertex)), 0.001);
		suninten = pow(suninten, c_SunLevel);
		mediump vec4 sun = c_SunColorEnd*(1.0 - suninten) + c_SunColorStart*suninten;
		oColor += sun * suninten * nearcam;	
	#endif	
	
	#if defined(WHITEWATER) && defined(USE_DEPTHMAP)
		mediump vec4 WhiteWater  = texture2D(tex_WhiteWater, oWorldPos.xz * 0.5 + worldNormal.xz * 0.2);
		mediump vec4 WhiteWater2 = texture2D(tex_WhiteWater, oWorldPos.xz * 2.0 + ReflectionNormal.xz * 0.2);
		WhiteWater.w = clamp(1.0 - border * 0.03,0.0,1.0) - 0.15;
		WhiteWater.w *= WhiteWater.w;
		WhiteWater.w = (1.0 - float(WhiteWater.w < WhiteWater.x * WhiteWater2.x)) * WhiteWater.w * WhiteWater.x;
		WhiteWater.w *= 1.0 - clamp((iViewToVertex.w - c_FogParam.x) / c_FogParam.y,0.0,1.0);
		oColor.xyz = oColor.xyz*(1.0-WhiteWater.w) + WhiteWater.w;
	#endif	

	//岸边过渡
	#if defined(BORDER)
		#if defined(USE_DEPTHMAP)
		    mediump float lerptemp3 = clamp(border,0.0,1.0);
			oColor = crRefract*(1.0 - lerptemp3) + oColor* lerptemp3;
		#else
		    mediump float lerptemp3 = clamp((oWorldPos.y - oWorldPos.w) * 8.0,0.0,1.0);
			oColor = crRefract*(1.0-lerptemp3) +  oColor*lerptemp3;
		#endif
	#endif	
	
	#if defined(FOGLINEAR) || defined(FOGEXP)
		#if defined(USE_DEPTHMAP)
			oColor.xyz = FogColor.xyz + oColor.xyz * FogColor.w;
		#else
			oColor.xyz = FogColor.xyz * oColor.w + oColor.xyz * FogColor.w;
		#endif
	#endif      
#endif
 
gl_FragColor = oColor ;

}
 