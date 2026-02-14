#version 300 es
precision mediump float;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture1;

void main()
{
    //Lighting calculation
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    vec3 norm = normalize(Normal);
    float diff = abs(dot(norm, lightDir));
    
    //Ambient + diffuse lighting
    vec3 ambient = vec3(0.3);
    vec3 diffuse = vec3(diff);
    vec3 lighting = ambient + diffuse;
    
    //Sample texture and apply lighting
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(texColor.rgb * lighting, texColor.a);
}
