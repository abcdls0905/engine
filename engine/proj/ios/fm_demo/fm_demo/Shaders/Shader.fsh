//
//  Shader.fsh
//  fm_demo
//
//  Created by SnailResearch on 13-5-17.
//  Copyright (c) 2013年 SnailResearch. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
