#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D image;

uniform bool horizontal;
uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
    vec2 tex_offset = 1.0 / textureSize(image, 0); // gets size of single texel
	float weightSum = 0;
	vec3 result = vec3(0);
	vec3 tmpColor;
	 
	tmpColor = texture(image, TexCoords).rgb;
	if(tmpColor != vec3(0)){
		result += tmpColor * weight[0];
		weightSum += weight[0];
	}

    if(horizontal)
    {
         for(int i = 1; i < 5; ++i)
         {
			tmpColor = texture(image, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb;
			if(tmpColor != vec3(0)){
				result += tmpColor * weight[i];
				weightSum += weight[i];
			}

			tmpColor = texture(image, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb;
			if(tmpColor != vec3(0)){
				result += tmpColor * weight[i];
				weightSum += weight[i];
			}
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
			tmpColor = texture(image, TexCoords + vec2(0.0, tex_offset.y * i)).rgb;
			if(tmpColor != vec3(0)){
				result += tmpColor * weight[i];
				weightSum += weight[i];
			}

			tmpColor = texture(image, TexCoords - vec2(0.0, tex_offset.y * i)).rgb;
			if(tmpColor != vec3(0)){
				result += tmpColor * weight[i];
				weightSum += weight[i];
			}
        }
    }

    FragColor = vec4(result / weightSum, 1.0);
}