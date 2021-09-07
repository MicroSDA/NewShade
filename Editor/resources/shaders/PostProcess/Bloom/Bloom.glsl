#type compute
#version 460 core
layout(local_size_x = 16, local_size_y = 16) in;
/* Input data*/
layout (		    binding = 0) uniform sampler2D u_sInputF;
layout (		    binding = 1) uniform sampler2D u_sInputS;

layout (rgba16f,	binding = 2) uniform restrict image2D  u_Input;
layout (rgba16f, 	binding = 3) uniform restrict image2D  u_Output; 


#define EPSILON 1.0e-4

vec4 DownsampleBox4(vec2 uv, vec2 texelSize, float lod)
{
	vec4 offset  = texelSize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0);
	
    vec4 color 	 = textureLod(u_sInputF, uv + offset.xy, lod);
    color 		+= textureLod(u_sInputF, uv + offset.zy, lod);
    color 		+= textureLod(u_sInputF, uv + offset.xw, lod);
    color 		+= textureLod(u_sInputF, uv + offset.zw, lod);

    return  color * (1.0 / 4.0);
}

vec4 DownsampleBox13(vec2 uv, vec2 texelSize, float lod)
{
	
	vec4 A = textureLod(u_sInputF, uv + texelSize * vec2(-1.0, -1.0), lod);
    vec4 B = textureLod(u_sInputF, uv + texelSize * vec2( 0.0,  0.0), lod);
    vec4 C = textureLod(u_sInputF, uv + texelSize * vec2( 1.0, -1.0), lod);
    vec4 D = textureLod(u_sInputF, uv + texelSize * vec2(-0.5, -0.5), lod);
    vec4 E = textureLod(u_sInputF, uv + texelSize * vec2( 0.5, -0.5), lod);
    vec4 F = textureLod(u_sInputF, uv + texelSize * vec2(-1.0,  0.0), lod);

    vec4 G = textureLod(u_sInputF, uv, lod				   	             );

    vec4 H = textureLod(u_sInputF, uv + texelSize * vec2( 1.0,  0.0), lod);
    vec4 I = textureLod(u_sInputF, uv + texelSize * vec2(-0.5,  0.5), lod);
    vec4 J = textureLod(u_sInputF, uv + texelSize * vec2( 0.5,  0.5), lod);
    vec4 K = textureLod(u_sInputF, uv + texelSize * vec2(-1.0,  1.0), lod);
    vec4 L = textureLod(u_sInputF, uv + texelSize * vec2( 0.0,  1.0), lod);
    vec4 M = textureLod(u_sInputF, uv + texelSize * vec2( 1.0,  1.0), lod);

    vec2 div = (1.0 / 4.0) * vec2(0.5, 0.125);

    vec4 color  = (D + E + I + J) * div.x;
		 color += (A + B + G + F) * div.y;
    	 color += (B + C + H + G) * div.y;
    	 color += (F + G + L + K) * div.y;
    	 color += (G + H + M + L) * div.y;

    return color;
}

vec4 UpsampleBox4(vec2 uv, vec2 texelSize, float lod, vec4 scale)
{
	vec4 offset  = texelSize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0) * (scale * 0.5);
	
    vec4 color 	 = textureLod(u_sInputF, uv + offset.xy, lod);
    color 		+= textureLod(u_sInputF, uv + offset.zy, lod);
    color 		+= textureLod(u_sInputF, uv + offset.xw, lod);
    color 		+= textureLod(u_sInputF, uv + offset.zw, lod);

    return  color * (1.0 / 4.0);
}

vec4 UpsampleTent(vec2 uv, vec2 texelSize, float lod, vec4 scale)
{
	vec4 offset  = texelSize.xyxy * vec4(1.0, 1.0, -1.0, 1.0) * scale;
	
	vec4 color   = textureLod(u_sInputF, uv - offset.xy, lod);
	color 		+= textureLod(u_sInputF, uv - offset.wy, lod) * 2.0;
    color 		+= textureLod(u_sInputF, uv - offset.zy, lod);
	
    color 		+= textureLod(u_sInputF, uv + offset.zw, lod) * 2.0;
	color 		+= textureLod(u_sInputF, uv 		   , lod) * 4.0;
	color 		+= textureLod(u_sInputF, uv + offset.xw, lod) * 2.0;
		
	color 		+= textureLod(u_sInputF, uv + offset.zy, lod);
	color 		+= textureLod(u_sInputF, uv + offset.wy, lod) * 2.0;
	color 		+= textureLod(u_sInputF, uv + offset.xy, lod);
	
	return color * (1.0 / 16.0);
}

vec4 QThreshold(vec4 color, vec3 curve, float threshold, float exposure)
{
	color *= exposure;
	float br =  max(max(color.r, color.g), color.b);
    // Under-threshold part: quadratic curve
    float rq = clamp(br - curve.x, 0.0, curve.y);
    rq = curve.z * rq * rq;
    // Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);

    return color;
}


subroutine void Stage(int scale);
subroutine (Stage)
void HDR(int scale)
{
	// Если прибовляем 1 к скейлу то уменьшается на 1 от оригинала
	// если не прибавлять 1 то растянеть уменьшеную копию
	// + 0.5 к семплер пос сохраняет позицию после рескйейла так что нужно!
	// пока что не понятно gl_GlobalInvocationID.xy от чего оно зависит, скорой всего от бинда картинки и вседа реферит на оригинальный сайз но это не точно
	// если хотим записать в мип мап ниже то + 1 к скейлу в обоих вариантах
	ivec2 samplerPos = ivec2(gl_GlobalInvocationID.xy) / int(pow(1.0, scale + 1)); // если + 1 то уменьшаяем, скейл по фпктическому разммеру, ели -1 то увеличиваем
	vec2 uv 		 = (samplerPos + 0.5) / vec2(textureSize(u_sInputF, scale + 1));// если + 1 то уменьшаяем, скейл по дате, ели -1 то увеличиваем
	vec2 texelSize   = 1.0 / textureSize(u_sInputF, scale).xy; // тут вроде прибовлять не надо иначе качество падает
	//vec4 color       = textureLod(u_sInputF, uv, scale);
	vec4 color       = DownsampleBox4(uv, texelSize.xy, scale);
	color 		     = QThreshold(color, vec3(0.2126, 0.7152, 0.0722), 0.2, 1.0);
	
	/* не забудь про мемори бариер, без него мерцание !*/
	/*vec4 texelValue= texelFetch(texture2D, ivec2(gl_FragCoord.xy), 0);*/ // можно еще попрбовать это
	
	imageStore(u_Output, samplerPos, color);
};
subroutine (Stage)
void DBox4(int scale)
{
	ivec2 samplerPos = ivec2(gl_GlobalInvocationID.xy) / int(pow(1.0, scale + 1));
	vec2 uv 		 = (samplerPos + 0.5) / vec2(textureSize(u_sInputF, scale + 1));
	vec2 texelSize   = 1.0 / textureSize(u_sInputF, scale).xy;
	vec4 color       = DownsampleBox4(uv, texelSize.xy, scale);
	
	imageStore(u_Output, samplerPos, vec4(color.rgb,1.0));
};
subroutine (Stage)
void DBox13(int scale)
{
	ivec2 samplerPos = ivec2(gl_GlobalInvocationID.xy) / int(pow(2.0, scale));
	vec2 uv 		 = (samplerPos  + 0.5) / vec2(textureSize(u_sInputF, scale));
	vec2 texelSize   = 1.0 / textureSize(u_sInputF, scale).xy;
	vec4 color       = DownsampleBox13(uv, texelSize.xy, scale);
	
	imageStore(u_Output, samplerPos, vec4(color.rgb,1.0));
};
subroutine (Stage)
void UBox4(int scale)
{
	// Вот шас вроде работает как есть, если там + 1 на уменьшение то тут - 1 на увеличение и вроде работает, искажений нету
	// Тут меняли
// Старнно что не вытягивает имейдж из дода ниже, как будно прошлая запись ушла в не куда а там нету не чего 
// присутствует только лод -1 который  выше, а тикущего по скейлу 1 нету
	ivec2 samplerPos = ivec2(gl_GlobalInvocationID.xy) / int(pow(2.0,   scale-1)); // - 1 важно
	vec2 uv 		 = (samplerPos + 0.5) / vec2(textureSize(u_sInputF, scale-1));// - 1 важно
	vec2 texelSize   = 1.0 / textureSize(u_sInputF, scale).xy;
	vec4 first       = UpsampleBox4(uv, texelSize, scale, vec4(1)); // если увеличивать тут то изображение буде раздваиватся
	vec4 second      = textureLod(u_sInputF, uv, scale-1); // тут скейлы оставлять, и выше тоже, грузим имейдж потому что там бинд имейдаж
	//vec4 second      = imageLoad(u_Output,   samplerPos); // тут скейлы оставлять, и выше тоже, грузим имейдж потому что там бинд имейдаж+
	//vec4 first       = textureLod(u_sInputF, uv, scale);
	// Тут комбинируется размазанная + нет версии с одного мипа
	imageStore(u_Output, samplerPos, first+ second); // не пытайся взять тут нулевой мип, его нету просто, это второй атачмент и внем по 0 пусто, потому что мы писали сразу в 1 мип
	
	// last combine staдия должна быть другой
};
subroutine (Stage)
void UTent(int scale)
{
	ivec2 samplerPos = ivec2(gl_GlobalInvocationID.xy) / int(pow(2.0,   scale -1));
	vec2 texelSize   = 1.0 / textureSize(u_sInputF, scale).xy;
	vec2 uv 		 = (samplerPos + 0.5) / vec2(textureSize(u_sInputF, scale));
	
	vec4 first       = UpsampleTent(uv, texelSize,  scale, vec4(1));
	vec4 second      = textureLod(u_sInputF, uv, scale);
	imageStore(u_Output, samplerPos, first + second);
};
subroutine (Stage)
void Combine(int scale)
{
	ivec2 samplerPos = ivec2(gl_GlobalInvocationID.xy);
	vec4 first  	 = imageLoad(u_Input,  samplerPos);
	vec4 second 	 = imageLoad(u_Output, samplerPos);
	imageStore(u_Output, samplerPos, first + second);
};

subroutine uniform Stage s_Stage;
uniform int	u_Scale;
void main()
{
	s_Stage(u_Scale);
}
