
#include "nv_hhdds.h"

#ifdef RENDER_ES_3_0
#include <GLES3/gl3.h>
#else
#include <GLES2/gl2.h>
#endif

static int LoadTextureFromDDSData(GLenum target, NvS32 startLevel, const NVHHDDSImage *pImage, NvBool useMipmaps )
{
    NvS32 w, h, level, m;
    NvS32 powerOfTwo, mipLevelsToLoad, maxLevel;
    GLenum baseTarget = (target == GL_TEXTURE_2D) ? GL_TEXTURE_2D : GL_TEXTURE_CUBE_MAP;

    if ( pImage == NULL ) {
        return 0;
    }

    w = pImage->width;
    h = pImage->height;
    level = startLevel;

    /* Need to detect if the image has a non-pow-2 dimension */
    powerOfTwo = ((w & (w - 1)) == 0) && ((h & (h - 1)) == 0);

    /* Only pow-2 images that are tagged for using mipmaps get them */
    mipLevelsToLoad = ((powerOfTwo && useMipmaps) ? pImage->numMipmaps : 1);

    maxLevel = level + mipLevelsToLoad;

    for ( m = 0; level < maxLevel; level++, m++ ) {
        if (pImage->compressed) {
            glCompressedTexImage2D( target, m, pImage->format, w, h,
                                    0, pImage->size[level],
                                    pImage->data[level] );
        } else {
            glTexImage2D( target, m, pImage->format, w, h, 0,
                          pImage->format, pImage->componentFormat,
                          pImage->data[level]);
        }

        w = (w == 1) ? w : w >> 1;
        h = (h == 1) ? h : h >> 1;
    }

    if (mipLevelsToLoad > 1)
    {
        glTexParameterf(baseTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    }
    else
    {
        glTexParameterf(baseTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameterf(baseTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* For now, we clamp everyone - apps wanting wrapping for a texture can set it manually */
    glTexParameterf(baseTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(baseTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return 1;
}

GLuint NvCreateTextureFromDDSEx(const char* filename, NvBool flipVertical, NvBool useMipmaps, NvS32* width, NvS32* height, NvBool* alpha, NvBool *isCubeMap)
{
    // Clear the GL error before this function, since at the end of this 
    // function, we test the error and report it.  We do not want to report
    // an error that happened at some random time before.  If we want to
    // catch those, we need more general/comprehensive handling.  But having
    // this function print a GL error for something that happened in other 
    // random code is confusing, especially to non-rail developers.
    // Some other code, like NVBitfont, prints error messages at the top of
    // the function and print a message that implies that the error was there
    // at the time of call.  That may make sense as an optional setting down
    // the road
    glGetError();

    GLuint tex = 0;
    NVHHDDSImage *img = NVHHDDSLoad(filename, flipVertical ? 1 : 0);
    if (img)
    {
        if(isCubeMap)
        {
            *isCubeMap = img->cubemap ? NV_TRUE:NV_FALSE;
        }
        if (width)
            *width = img->width;
        if (height)
            *height = img->height;
        if (alpha)
            *alpha = img->alpha ? NV_TRUE : NV_FALSE;

        glGenTextures(1, &tex);
        if (!img->cubemap)
        {
            glBindTexture(GL_TEXTURE_2D, tex);
            LoadTextureFromDDSData(GL_TEXTURE_2D, 0, img, useMipmaps);
        }
        else
        {
            NvS32 baseLevel = 0;
            glBindTexture(GL_TEXTURE_CUBE_MAP, tex);

            LoadTextureFromDDSData(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, baseLevel, img, useMipmaps);
            baseLevel += img->numMipmaps ? img->numMipmaps : 1;

            LoadTextureFromDDSData(GL_TEXTURE_CUBE_MAP_POSITIVE_X, baseLevel, img, useMipmaps);
            baseLevel += img->numMipmaps ? img->numMipmaps : 1;

            LoadTextureFromDDSData(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, baseLevel, img, useMipmaps);
            baseLevel += img->numMipmaps ? img->numMipmaps : 1;

            LoadTextureFromDDSData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, baseLevel, img, useMipmaps);
            baseLevel += img->numMipmaps ? img->numMipmaps : 1;

            LoadTextureFromDDSData(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, baseLevel, img, useMipmaps);
            baseLevel += img->numMipmaps ? img->numMipmaps : 1;

            LoadTextureFromDDSData(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, baseLevel, img, useMipmaps);
            baseLevel += img->numMipmaps ? img->numMipmaps : 1;
        }

        GLint err = glGetError();
        if (err)
            NVLogError("NvCreateTextureFromDDSEx error", "");

        NVHHDDSFree(img);
    }

    return tex;
}

