//--------------------------------------------------------------------
// 文件名:		language.h
// 内  容:		特殊语种越南文和泰文支持
// 说  明:		
// 创建日期:	2009年12月22日
// 创建人:		杨利平
// 版权所有:	苏州蜗牛电子有限公司
//--------------------------------------------------------------------

#ifndef _language_h
#define _language_h
//--------------------------------------------------------------------
namespace FxLanguage
{
    // 越南文 begin---------------------------------------------------
    // 理解2009_12_22：
    //    只有单元音才能再跟变音符
    //    不存在两个连续的变音符 

    // 越南文单元音,对应Unicode代码
    const wchar_t g_wcsVietnameseSYan[] = { 
        0x61,   // a
        0x65,   // e
        0x69,   // i
        0x6f,   // o
        0x75,   // u
        0x79,   // y

        0x41,   // A
        0x45,   // E
        0x49,   // I
        0x4f,   // O
        0x55,   // U
        0x59,   // Y

        0xea,   // ê	
        0x1b0,  // ư
        0x1a1,  // ơ
        0xe2,   // â
        0x103,  // ă
        0xf4,   // ô

        0xca,   // Ê
        0x1af,  // Ư
        0x1a0,  // Ơ
        0xc2,   // Â
        0x102,  // Ă
        0xd4,   // Ô

    };

    // 理解
    // 变音符号9个
    //       音调符5个,会与单元音合成,紧跟在单元音后面，但不能同时跟两个及以上
    //       变音符4个用于添加母音ăâđơ, 不参于合成

    // 越南文音调符5个,,对应Unicode代码
    const wchar_t g_wcsVietnamesVoiceLevel[] = {
        0x300,  // à
        0x309,  // ả
        0x303,  // ã
        0x301,  // á
        0x323,  // ạ
    };

    // 检查字符是否是可以是越南文单元音字符
    inline bool IsVietnameseSYan(wchar_t ch)
    {
        for (size_t i = 0; i < 24; ++i)
        {
            if (g_wcsVietnameseSYan[i] == ch)
            {
                return true;
            }
        }

        return false;
    }
    // 检查字符是否是越南文音调符
    inline bool IsVietnameseVoiceLevel(wchar_t ch)
    {
        for (size_t i = 0; i < 5; ++i)
        {
            if (g_wcsVietnamesVoiceLevel[i] == ch)
            {
                return true;
            }
        }

        return false;
    }

    // 越南文音调输入处理规则：
    //   当发现输入符为音调符时，检查之前输入的字符是否是元音，
    //   是元音则合成字符，如果不是元音则认为非法不处理
    // 越南文 end-----------------------------------------------------

    // 泰文 begin-----------------------------------------------------
    inline bool IsThaiCharacter(wchar_t ch)
    {
        if (ch >= 0x0E00 && ch < 0x0E7F)
        {
            return true;
        }

        return false;
    }

    // 辅音CONS（Consonants）：总共44个字符
    inline bool IsThaiCONS(wchar_t ch)
    {
        if (ch == 0x0E24 || ch == 0x0E26)
        {
            return false;
        }
        else if (ch >= 0x0E01 && ch <= 0x0E2E)
        {
            return true;
        }

        return false;
    }

    // 前置元音LV（Leading Vowels）
    inline bool IsThaiLV(wchar_t ch)
    {
        if (ch >= 0x0E40 && ch <= 0x0E44)
        {
            return true;
        }

        return false;
    }

    // 后置元音FV（Following Vowels）
    // FV1
    inline bool IsThaiFV1(wchar_t ch)
    {
        if (ch == 0x0E30 ||
            ch == 0x0E32 ||
            ch == 0x0E33)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiFV2(wchar_t ch)
    {
        if (ch == 0x0E45)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiFV3(wchar_t ch)
    {
        if (ch == 0x0E24 ||
            ch == 0x0E26)
        {
            return true;
        }

        return false;
    }

    // 下元音BV（Below Vowels）
    inline bool IsThaiBV1(wchar_t ch)
    {
        if (ch == 0x0E38)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiBV2(wchar_t ch)
    {
        if (ch == 0x0E39) //ู
        {
            return true;
        }

        return false;
    }

    // 上元音AV（Above Vowels）
    inline bool IsThaiAV1(wchar_t ch)
    {
        if (ch == 0x0E34)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiAV2(wchar_t ch)
    {
        if (ch == 0x0E31 ||
            ch == 0x0E36)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiAV3(wchar_t ch)
    {
        if (ch == 0x0E35 ||
            ch == 0x0E37)
        {
            return true;
        }

        return false;
    }

    // 音调 TONE（Tonemarks）
    inline bool IsThaiTONE(wchar_t ch)
    {
        if (ch >= 0x0E48 && ch <= 0x0E4B)
        {
            return true;
        }

        return false;
    }

    // 读音符-D（Diacritics）：分为上读音符和下读音符
    // 上读音符 AD（Above Diacritics）
    inline bool IsThaiAD1(wchar_t ch)
    {
        if (ch == 0x0E4C ||
            ch == 0x0E4D)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiAD2(wchar_t ch)
    {
        if (ch == 0x0E47)
        {
            return true;
        }

        return false;
    }

    inline bool IsThaiAD3(wchar_t ch)
    {
        if (ch == 0x0E4E)
        {
            return true;
        }

        return false;
    }

    // 下读音符 BD（Below Diacritics）
    inline bool IsThaiBD(wchar_t ch)
    {
        if (ch == 0x0E3A)
        {
            return true;
        }

        return false;
    }

    // 非构成音 NON：如数字或特殊音
    inline bool IsThaiNONE(wchar_t ch)
    {
        if (ch >= 0x0E00 && ch < 0x0E7F &&
            !IsThaiCONS(ch) &&
            !IsThaiLV(ch) &&
            !IsThaiFV1(ch) &&
            !IsThaiFV2(ch) &&
            !IsThaiFV3(ch) &&
            !IsThaiBV1(ch) &&
            !IsThaiBV2(ch) &&
            !IsThaiAV1(ch) &&
            !IsThaiAV2(ch) &&
            !IsThaiAV3(ch) &&
            !IsThaiTONE(ch) &&
            !IsThaiAD1(ch) &&
            !IsThaiAD2(ch) &&
            !IsThaiAD3(ch) &&
            !IsThaiBD(ch)
            )
        {
            return true;
        }

        return false;
    }

    // 泰文最多有三个Unicode合成一个显示单位
    // 泰文第一个合成字符必须紧跟CONS
    // 检查是否是泰文第一个合成字符
    inline bool IsThaiCompose1(wchar_t ch)
    {
        if (IsThaiAV1(ch) || 
            IsThaiAV2(ch) ||
            IsThaiAV3(ch) ||
            IsThaiBV1(ch) ||
            IsThaiBV2(ch) ||
            IsThaiAD3(ch) ||
            ch == 0x0E33) // 这个FV1的后置元音必须进行组合
        {
            return true;
        }

        return false;
    }

    // 检查是否是泰文的第二个合成字符
    // 泰文第二个合成字符必须紧跟AV/BV
    inline bool IsThaiCompose2(wchar_t ch)
    {
        if (IsThaiTONE(ch) || 
            IsThaiBD(ch) ||
            IsThaiAD1(ch) ||
            IsThaiAD2(ch))
        {
            return true;
        }

        return false;
    }
    // 泰文 end-------------------------------------------------------
}
//--------------------------------------------------------------------
#endif //_language_h