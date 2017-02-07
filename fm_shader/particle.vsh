/*
#define PAR_DEPTHMAP  1
#define PAR_BILLBOARD_Y 1
#define PAR_BILLBOARD_NONE 1
#define PAR_HIGHLIGHT 1
#define PAR_FOGUSE 1
#define PAR_FOGLINEAR 1
#define PAR_FOGEXP 1
#define PAR_GPU 1
*/

//@@ Input parameter:
uniform mediump mat4 c_mat4ViewProj;
uniform mediump vec4 c_ParticleColor;

#if defined(PAR_GPU)

	//@@ Update in every frame:
	uniform mediump vec3 c_vec3EmitterPos;
	uniform mediump vec4 c_vec4Time;
	uniform mediump vec4 c_vec4Speed;
	uniform mediump vec4 c_vec4Force;
	uniform mediump vec4 c_vec4UV_Bias;
	uniform mediump vec4 c_fTrack[144];
	uniform mediump vec3 c_vec3ViewPosition;
	uniform mediump vec4 c_vec4RotateAxis;

	//@@ GPU Particle input:
	attribute mediump vec4 iGpuPos;
	attribute mediump vec3 iGpuEmitterDir;
	attribute mediump vec2 iGpuUV;
	attribute mediump vec2 iGpuPosDelta;
	attribute mediump vec2 iGpuRandom0;
	attribute mediump vec2 iGpuRandom1;
	attribute mediump vec2 iGpuRandom2;

#else

	//@@ CPU Particle input:
	attribute mediump vec4 iCpuPos;
	attribute mediump vec4 iCpuColor;
	attribute mediump vec2 iCpuUV;

#endif

//@@ Output parameter:
varying mediump vec4 oScreenPos;
varying mediump vec2 oUV;
varying mediump vec4 oColor;

#if defined(PAR_DEPTHMAP) || defined(PAR_FOGLINEAR) || defined(PAR_FOGEXP)
	mediump vec4  g_oScreenPos;
#endif


#if defined(PAR_GPU)
void main(void)
{
	//@@ Temp variable:
	mediump vec4  g_oPosition;
	mediump vec4  g_oColor;
	mediump vec2  g_oUV;
	mediump float g_oFrame=0.0;

	//@@ GPU Particle calculate:
	{
		//@@ Temp variable:
		mediump vec4 oPosition = iGpuPos;
		mediump vec2 PosDelta  = iGpuPosDelta;
		mediump vec3 DevDir    = vec3(iGpuRandom0.xy,iGpuRandom1.x);
		mediump float Weight   = iGpuRandom1.y;
		mediump vec2 Rotate    = iGpuRandom2;

		//@@ Must macro, don't function:
		//#define sign_n(x) (((x)<-1)?-1:(((x)>1)?1:(x)))

		//@@ Macro output:
		mediump vec3 g_oTrackValue;
		mediump vec3 g_oTrackSpeed;

		g_oPosition = vec4(oPosition.xyz + c_vec3EmitterPos, 1.0);
		mediump float Partlicle_life = c_vec4Time.y; // c_vec4Time.y == m_fParticleLife;
		mediump float Particle_time = (c_vec4Time.x * 1000.0 - oPosition.w);// c_vec4Time.x == m_dTimeCounter;

		//@@ Current cycle time:
		Particle_time = mod(Particle_time, Partlicle_life);

		//@@ Hide before emit:
		PosDelta *= step(0.0, Particle_time);

		//@@ Hide after emit:
		PosDelta *= step(c_vec4Time.x * 1000.0 - Particle_time, c_vec4Time.w);

		//@@ "Wind power" and "Gravity":
		{
			mediump vec3 vec3OutWind = vec3(0.0,0.0,0.0);
			//TrackSpeed(48, Particle_time);
			{
				int key = 7 + (48);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 4);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 2);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 1);
				key = key +  int(Particle_time<c_fTrack[key].w) * -1;
				float key_time = c_fTrack[key+1].w - c_fTrack[key].w;
				float current_time = (Particle_time) - c_fTrack[key].w;
				vec3 current_speed = mix(c_fTrack[key].xyz, c_fTrack[key+1].xyz,
					current_time / key_time);
				g_oTrackSpeed = c_fTrack[key].xyz + current_speed;
				g_oTrackSpeed = g_oTrackSpeed * current_time * 0.5;
				g_oTrackSpeed = c_fTrack[key+16].xyz + g_oTrackSpeed;
			}
			mediump vec2 WindGravity = g_oTrackSpeed.xy;
			mediump vec3 Wind = c_vec4Force.xyz * WindGravity.x;
			mediump vec3 Gravity = vec3(0.0, c_vec4Force.w * Weight * WindGravity.y, 0.0);
			vec3OutWind = Wind - Gravity ;

			//@@ Append result:
			g_oPosition.xyz += vec3OutWind;
		}

		//@@ "Speed":
		{
			mediump vec3 vec3OutSpeed = vec3(0.0,0.0,0.0);
			mediump float speedvalue = length(iGpuEmitterDir);
			vec3OutSpeed = normalize(DevDir * c_vec4Speed.x + iGpuEmitterDir / speedvalue);
			//TrackSpeed(16, Particle_time);
			{
				int key = 7 + (16);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 4);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 2);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 1);
				key = key +  int(Particle_time<c_fTrack[key].w) * -1;
				float key_time = c_fTrack[key+1].w - c_fTrack[key].w;
				float current_time = (Particle_time) - c_fTrack[key].w;
				vec3 current_speed = mix(c_fTrack[key].xyz, c_fTrack[key+1].xyz,
					current_time / key_time);
				g_oTrackSpeed = c_fTrack[key].xyz + current_speed;
				g_oTrackSpeed = g_oTrackSpeed * current_time * 0.5;
				g_oTrackSpeed = c_fTrack[key+16].xyz + g_oTrackSpeed;
			}
			vec3OutSpeed *= g_oTrackSpeed.x * speedvalue;
			vec3OutSpeed += DevDir * c_vec4Speed.y * Particle_time;

			//@@ Append result:
			g_oPosition.xyz += vec3OutSpeed;
		}

		//@@ "Rotate" and "Size":
		vec2 Delta = vec2(0.0,0.0);// Save size;
		{
			//TrackSpeed(96, Particle_time);
			{
				int key = 7 + (96);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 4);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 2);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 1);
				key = key +  int(Particle_time<c_fTrack[key].w) * -1;
				float key_time = c_fTrack[key+1].w - c_fTrack[key].w;
				float current_time = (Particle_time) - c_fTrack[key].w;
				vec3 current_speed = mix(c_fTrack[key].xyz, c_fTrack[key+1].xyz,
					current_time / key_time);
				g_oTrackSpeed = c_fTrack[key].xyz + current_speed;
				g_oTrackSpeed = g_oTrackSpeed * current_time * 0.5;
				g_oTrackSpeed = c_fTrack[key+16].xyz + g_oTrackSpeed;
			}
			mediump float angle = Rotate.x - Rotate.y * g_oTrackSpeed.x;
			mediump vec2 rotate;
			rotate.x = sin(angle);
			rotate.y = cos(angle);
			//TrackValue(80, Particle_time);
			{
				int key = 7 + (80);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 4);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 2);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 1);
				key = key +  int(Particle_time<c_fTrack[key].w) * -1;
				g_oTrackValue = mix(c_fTrack[key].xyz, c_fTrack[key+1].xyz,
					((Particle_time) - c_fTrack[key].w) / (c_fTrack[key+1].w - c_fTrack[key].w));
			}
			Delta = PosDelta * g_oTrackValue.x;
			g_oPosition.w = abs(Delta.x);
			Delta = Delta.x * vec2(rotate.y, rotate.x) + Delta.y * vec2(-rotate.x, rotate.y);
			g_oColor = vec4(1.0, 1.0, 1.0, 1.0);
			//TrackValue(0, Particle_time);
			{
				int key = 7 + (0);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 4);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 2);
				key = key + (int(sign(Particle_time-c_fTrack[key].w)) * 1);
				key = key +  int(Particle_time<c_fTrack[key].w) * -1;
				g_oTrackValue = mix(c_fTrack[key].xyz, c_fTrack[key+1].xyz,
					((Particle_time) - c_fTrack[key].w) / (c_fTrack[key+1].w - c_fTrack[key].w));
			}
			g_oColor.w = g_oTrackValue.x;
		}


		#if defined(PAR_BILLBOARD_NONE)
			g_oPosition.xz += Delta;
		#endif

		//@@ Extend particle:
		{
			vec3 ViewDir = g_oPosition.xyz - c_vec3ViewPosition.xyz;
			ViewDir = normalize(ViewDir);

			#if defined(PAR_BILLBOARD_Y)
				mediump vec3 x_dir = cross(ViewDir, c_vec4RotateAxis.xyz);
				x_dir = normalize(x_dir);
				g_oPosition.xyz += Delta.x * x_dir;
				g_oPosition.xyz += Delta.y * c_vec4RotateAxis.xyz;
			#endif

			#if !defined(PAR_BILLBOARD_NONE) && !defined(PAR_BILLBOARD_Y)
				mediump vec3 x_dir = cross(ViewDir, c_vec4RotateAxis.xyz);
				x_dir = normalize(x_dir);
				mediump vec3 y_dir = cross(x_dir, ViewDir);
				g_oPosition.xyz += Delta.x * x_dir ;
				g_oPosition.xyz += Delta.y * y_dir ;
			#endif
		}

		g_oFrame = floor(Particle_time * 0.03);
	}


	g_oPosition.w = 1.0;
	g_oColor = g_oColor * c_ParticleColor;
	g_oUV = iGpuUV;
	g_oUV.x = g_oUV.x + g_oFrame * c_vec4UV_Bias.x;
	mediump float fParticleSize = g_oPosition.w;
	g_oPosition *= c_mat4ViewProj;


	#if defined(PAR_DEPTHMAP) || defined(PAR_FOGLINEAR) || defined(PAR_FOGEXP)
		g_oScreenPos = g_oPosition;
	#endif


	#if defined(PAR_DEPTHMAP)
		g_oScreenPos.z = fParticleSize;
		g_oColor.a = g_oColor.a * ( clamp (g_oScreenPos.w / fParticleSize * 0.5, 0.0, 1.0));
	#else
		g_oColor.a = 1.0;
	#endif


	gl_Position = g_oPosition;
	oScreenPos = gl_Position;
	oColor = g_oColor;
	oUV = g_oUV;

}

#else

//@@ CPU Particle logic:
void main(void)
{
	gl_Position = vec4(iCpuPos.xyz, 1.0) * c_mat4ViewProj;
	oScreenPos = gl_Position;
	oColor = iCpuColor * c_ParticleColor;
	oUV = iCpuUV;
}

#endif

