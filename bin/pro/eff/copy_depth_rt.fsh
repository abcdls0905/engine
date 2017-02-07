//#define ENCODE_RGB888
//#define ENCODE_RGBA8888
//#define ENCODE_R16
//#define ENCODE_R32

varying mediump vec2 oUV;

 
uniform mediump sampler2D tex_depth;

uniform mediump vec4  c_DepthParam;


mediump float getlinerdepth(in mediump sampler2D sdepth,in mediump vec2 UV)
{
   mediump float zvalue = texture2D( sdepth , UV ).x;
   zvalue =  1.0 / (c_DepthParam.x -zvalue*c_DepthParam.y );
   return zvalue;
}

mediump vec4 EncodeFloatRGBA(mediump float v ) 
{
	mediump vec4 enc = vec4(1.0, 255.0, 65025.0,16581375.0) * v;
	enc = fract(enc);
	enc -= enc.yzww * vec4( 1.0 / 255.0,1.0/255.0,1.0/255.0,0.0);
    return enc;
}


mediump float DecodeFloatRGBA(mediump vec4 rgba ) 
{
	return dot( rgba, vec4(1.0, 1.0/255.0, 1.0/65025.0, 1.0/16581375.0) );
}


void main()
{
   mediump vec2 UV = oUV;
   mediump float linear_z = getlinerdepth( tex_depth , UV );
 
   mediump vec4 color = vec4(0.0,0.0,0.0,0.0 );
   
   
#ifdef ENCODE_RGB888   
   linear_z = linear_z / 1024.0;
   color = EncodeFloatRGBA( linear_z ) ;
#endif

#ifdef ENCODE_RGBA8888
   linear_z = linear_z / 1024.0;
  
   color = EncodeFloatRGBA( linear_z ) ;
#endif

#ifdef ENCODE_R16
   color = vec4(linear_z,linear_z,linear_z,0.0); 
#endif

#ifdef ENCODE_R32
   color = vec4(linear_z,0.0,0.0,0.0); 
 // color = vec4(DecodeFloatRGBA( texture2D(tex_depth,UV) ),0.0,0.0,0.0)*1024.0;//Decode
#endif

   gl_FragColor =  color ;
}