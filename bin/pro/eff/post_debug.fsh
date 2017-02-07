//#define DEPTH 1
//#define DECODE_DEPTH 1

varying mediump vec2 oUV;
varying mediump vec3 oPos;

uniform mediump vec2  c_uvScaleRT;

uniform mediump vec4  c_DepthParam;

uniform mediump mat4  c_matInvProj;
 
uniform mediump sampler2D tex_color;
uniform mediump sampler2D tex_depth;

 
mediump float DecodeFloatRGBA(mediump vec4 rgba ) 
{
	return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}
 


#ifdef DEPTH

//camera.fFarZ / (camera.fFarZ * camera.fNearZ),(camera.fFarZ - camera.fNearZ) / (camera.fFarZ * camera.fNearZ) 
mediump float getlinerdepth(in mediump sampler2D sdepth,in mediump vec2 UV)
{
   mediump float zvalue = texture2D( sdepth , UV ).x;
   zvalue =  1.0 / (c_DepthParam.x -zvalue*c_DepthParam.y );
   return zvalue;
}
 

mediump vec4 GetViewPos(in mediump sampler2D sdepth,in mediump vec2 UV)
{
   mediump float zvalue = texture2D( sdepth , UV ).x;
   mediump vec4 ScreenPos = vec4( UV * vec2( 2.0,2.0 ) - vec2( 1.0,1.0),zvalue,1.0);
   mediump vec4 ViewPos = ScreenPos * c_matInvProj;
   ViewPos.xyz /= ViewPos.w;
   return ViewPos;
}


#endif
 
void main()
{
   mediump vec2 UV = oUV;
#ifdef DECODE_DEPTH
   mediump float depth = DecodeFloatRGBA( texture2D( tex_color , UV ).xyzw );
   gl_FragColor = vec4(depth,depth,depth,0.0); ;
   return;
#endif  
   

#ifdef DEPTH
   UV = UV*c_uvScaleRT;
   mediump float zvalue  = getlinerdepth(tex_depth , UV )/c_DepthParam.w;
   mediump vec4 oColor = vec4(zvalue,zvalue,zvalue,1.0);
#else      
   UV = UV*c_uvScaleRT;
   mediump vec4 oColor = texture2D( tex_color , UV ) ;
#endif
   gl_FragColor = oColor ;
}