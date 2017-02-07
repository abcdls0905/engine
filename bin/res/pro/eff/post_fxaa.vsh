precision mediump float;
precision mediump int;

attribute vec2 iPos;
attribute vec2 iUV;

varying vec2 v_vec2UV0;

void main(){
  v_vec2UV0 = iUV;
  gl_Position = vec4(iPos, 0.0,1.0);
}