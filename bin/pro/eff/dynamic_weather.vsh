attribute mediump vec3 iPos;
attribute mediump vec2 iUV;

uniform mediump mat4 c_mtxViewProj;
uniform mediump mat4 c_mtxViewProjPrev;

uniform mediump vec4 c_vPositionOffset;
uniform mediump vec4 c_vVelocity;
uniform mediump vec4 c_vAlpha;

uniform mediump vec4 c_vViewPosition;
uniform mediump vec4 c_vSizeScale;
uniform mediump vec4 c_vLighting;
uniform mediump vec4 c_vForward;

uniform mediump mat4 c_mOcclusionMatrix;

varying mediump vec2 oUV;
varying mediump vec4 oColor;
varying mediump vec3 oWorldPostion;


#define PARTICLE_BOX 30.0
 

void main()
{
  oUV = iUV;
  mediump vec3 position = iPos;

  
  // subtract the offset then fmod into (0.0, PARTICLE_BOX)
  position.xyz -= c_vPositionOffset.xyz;
  position.xyz = mod(position.xyz, PARTICLE_BOX);	// centre box on origin
	
  position.xyz -= (PARTICLE_BOX/2.0);
  // push along view vector so box is positioned more infront of camera
  position.xyz += c_vForward.xyz;
  
  // calculate colour based on distance from origin
  mediump float subtractColour = length(position.xyz) / PARTICLE_BOX;
  mediump	float distanceColourScale = 1.0 - subtractColour * subtractColour;
	
  // get world position
  mediump	vec4 worldPositionBottom = vec4(position.xyz + c_vViewPosition.xyz, 1.0);
  mediump	vec4 worldPositionTop = vec4(worldPositionBottom.xyz + c_vVelocity.xyz * c_vSizeScale.y, 1.0);
		
  // get projected positions of top and bottom of particle, and top of particle in previous frame
  mediump vec4 bottom = worldPositionBottom*c_mtxViewProj;		
  mediump vec4 top = worldPositionTop*c_mtxViewProj;
  mediump vec4 topPrev = worldPositionTop*c_mtxViewProjPrev;
	
  // get 2d vector in screenspace between top and bottom of particle
  mediump vec2 dir = (top.xy/top.w) - (bottom.xy/bottom.w);
  mediump vec2 dirPrev = (topPrev.xy/topPrev.w) - (bottom.xy/bottom.w);
	
  // calculate lengths
  mediump float len = length(dir);
  mediump float lenPrev = length(dirPrev);
	
  // scale colour depending on length
  mediump float lenColourScale = clamp(len/lenPrev,0.0,1.0);
	
  // get 2d vector perpendicular to velocity
  mediump vec2 dirPerp = normalize(vec2(-dirPrev.y, dirPrev.x));

  // choose either the top or bottom projected position using uv.y
  mediump vec4 oPosition =  topPrev*(1.0 - oUV.y) + bottom * oUV.y;
	
  // offset the position of each side of the particle using uv.x 
  oPosition.xy += (0.5 - oUV.x) * dirPerp * c_vSizeScale.x;
	
  // get the final colour including the lighting, distance and length scales, and per instance alpha
  oColor = c_vLighting * distanceColourScale * lenColourScale * c_vAlpha.x;
	
  oWorldPostion = worldPositionBottom.xyz;  
	
  gl_Position = oPosition;
}