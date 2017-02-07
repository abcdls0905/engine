//
//  Shader.fsh
//  fm_main
//
//  Created by SnailResearch on 13-3-14.
//  Copyright (c) 2013年 SnailResearch. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
