precision mediump float;
precision mediump int;

attribute vec2 iPos;
attribute vec2 iUV;

varying vec2 oUV;

void main()
{
  oUV = iUV;
  gl_Position = vec4(iPos, 0.0,1.0);
}