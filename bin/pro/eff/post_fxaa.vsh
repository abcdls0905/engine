vsin vec2 iPos;
vsin vec2 iUV;

vsout vec2 v_vec2UV0;

void main(){
  v_vec2UV0 = iUV;
  gl_Position = vec4(iPos, 0.0,1.0);
}