
uniform MEDIUMP float c_fSampleEnvMapLOD;
uniform MEDIUMP samplerCube tex_CubeMap;

vec3 get_vect_xpos(float u, float v)
{
	return normalize(vec3(1.0, -v, -u)); // y up, z left
}

vec3 get_vect_xneg(float u, float v)
{
	return normalize(vec3(-1.0, -v, u)); // y up, z right
}

vec3 get_vect_ypos(float u, float v)
{
	return normalize(vec3(u, 1.0, v)); // z down, x right
}

vec3 get_vect_yneg(float u, float v)
{
	return normalize(vec3(u, -1.0, -v)); // z up, x right
}

vec3 get_vect_zpos(float u, float v)
{
	return normalize(vec3(u, -v, 1.0)); // y up, x right
}

vec3 get_vect_zneg(float u, float v)
{
	return normalize(vec3(-u, -v, -1.0)); // y up, x left
}

#define SIZE 32.0
#define HALF_SIZE (SIZE * 0.5)
#define HALF_SIZE_INV (1.0/HALF_SIZE)

vec4 calc_irradiance(vec3 normal)
{
	// test
	//return tex_CubeMap.SampleLevel(sam_PointClamp, normal, 7.0f);
	
	#ifdef DEFAULT_LOD
		float sample_lod = 7.0;
	#else
		float sample_lod = c_fSampleEnvMapLOD;
	#endif

	normal = normalize(normal);
	
	vec3 irradiance = vec3(0.0, 0.0, 0.0);
	float u;
	float v;
	float fu, fv;

	vec3 ligth_vect;
	float LightDotNormal;
	//float t = 0;

	// x+
	for (v = 0.5; v < SIZE; v++)
	{
		for (u = 0.5; u < SIZE; u++)
		{
			fu = u;
			fv = v;
			fv -= HALF_SIZE;
			fu -= HALF_SIZE;
			ligth_vect = get_vect_xpos(fu * HALF_SIZE_INV, fv * HALF_SIZE_INV);
			LightDotNormal = dot(normal, ligth_vect);
			
			if (LightDotNormal > 0.0)
			{
				//t += LightDotNormal;
				irradiance += texCubeBias(tex_CubeMap, ligth_vect, sample_lod).xyz * LightDotNormal;
			}
		}
	}
	
	// x-
	for (v = 0.5; v < SIZE; v++)
	{
		for (u = 0.5; u < SIZE; u++)
		{
			fu = u;
			fv = v;
			fv -= HALF_SIZE;
			fu -= HALF_SIZE;
			ligth_vect = get_vect_xneg(fu * HALF_SIZE_INV, fv * HALF_SIZE_INV);
			LightDotNormal = dot(normal, ligth_vect);
			
			if (LightDotNormal > 0.0)
			{
				//t += LightDotNormal;
				irradiance += texCubeBias(tex_CubeMap, ligth_vect, sample_lod).xyz * LightDotNormal;
			}
		}
	}

	// y+
	for (v = 0.5; v < SIZE; v++)
	{
		for (u = 0.5; u < SIZE; u++)
		{
			fu = u;
			fv = v;
			fv -= HALF_SIZE;
			fu -= HALF_SIZE;
			ligth_vect = get_vect_ypos(fu * HALF_SIZE_INV, fv * HALF_SIZE_INV);
			LightDotNormal = dot(normal, ligth_vect);
			
			if (LightDotNormal > 0.0)
			{
				//t += LightDotNormal;
				irradiance += texCubeBias(tex_CubeMap, ligth_vect, sample_lod).xyz * LightDotNormal;
			}
		}
	}

	// y-
	for (v = 0.5; v < SIZE; v++)
	{
		for (u = 0.5; u < SIZE; u++)
		{
			fu = u;
			fv = v;
			fv -= HALF_SIZE;
			fu -= HALF_SIZE;
			ligth_vect = get_vect_yneg(fu * HALF_SIZE_INV, fv * HALF_SIZE_INV);
			LightDotNormal = dot(normal, ligth_vect);
			
			if (LightDotNormal > 0.0)
			{
				//t += LightDotNormal;
				irradiance += texCubeBias(tex_CubeMap, ligth_vect, sample_lod).xyz * LightDotNormal;
			}
		}
	}

	// z+
	for (v = 0.5; v < SIZE; v++)
	{
		for (u = 0.5; u < SIZE; u++)
		{
			fu = u;
			fv = v;
			fv -= HALF_SIZE;
			fu -= HALF_SIZE;
			ligth_vect = get_vect_zpos(fu * HALF_SIZE_INV, fv * HALF_SIZE_INV);
			LightDotNormal = dot(normal, ligth_vect);
			
			if (LightDotNormal > 0.0)
			{
				//t += LightDotNormal;
				irradiance += texCubeBias(tex_CubeMap, ligth_vect, sample_lod).xyz * LightDotNormal;
			}
		}
	}
	
	// z-
	for (v = 0.5; v < SIZE; v++)
	{
		for (u = 0.5; u < SIZE; u++)
		{
			fu = u;
			fv = v;
			fv -= HALF_SIZE;
			fu -= HALF_SIZE;
			ligth_vect = get_vect_zneg(fu * HALF_SIZE_INV, fv * HALF_SIZE_INV);
			LightDotNormal = dot(normal, ligth_vect);
			
			if (LightDotNormal > 0.0)
			{
				//t += LightDotNormal;
				irradiance += texCubeBias(tex_CubeMap, ligth_vect, sample_lod).xyz * LightDotNormal;
			}
		}
	}

	irradiance /= SIZE * SIZE * 6.0 / 3.14;	// 经验公式，更准确的公式应该是计算每个像素的solid angle做为权重
	return vec4(irradiance, 1.0);
}


void main(void)
{
	vec4 oColor;

	oColor = calc_irradiance(
			get_vect_ypos(
				gl_FragCoord.x * HALF_SIZE_INV,
				gl_FragCoord.y * HALF_SIZE_INV)
			);
	
	OutputColor0(oColor);
}