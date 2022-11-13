#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec3 fragPosIn;
in vec3 normalIn;
in vec2 texCoord;
uniform sampler2D ourTexture;
uniform vec3 ambient; //环境光照

struct SunLight             //平行光
{    
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight           //点光源
{
    vec3 position;

    vec3 diffuse;
    vec3 specular;
    vec3 intensity;
    float k0, k1, k2;
};

struct Material
{
    sampler2D diffuse;      //漫反射贴图
    sampler2D specular;     //镜面反射贴图
    float shininess;        //反光度
};


uniform Material material;
uniform PointLight pointLight;
uniform SunLight sunLight;
uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec3 ambient)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, texCoord)));
 
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, texCoord)));
 
    float dis = length(light.position - fragPos);
    float attenuation = 1.0f / (light.k0 + light.k1 * dis + light.k2 * (dis * dis));
 
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient + diffuse + specular;
}


vec3 CalcSunLight(SunLight light, vec3 normal, vec3 viewDir,vec3 ambient)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, texCoord)));
 
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * vec3(texture(material.specular, texCoord)));
 
    return ambient + diffuse + specular;
}

void main()
{
    vec3 ambient = ambient * texture(material.diffuse, texCoord).rgb;
    vec3 viewDir = normalize(viewPos - fragPosIn);
    vec3 normal = normalize(normalIn);
    vec3 result = CalcSunLight(sunLight, normal, viewDir,ambient);
	result += CalcPointLight(pointLight, normal, fragPosIn, viewDir,ambient);
    FragColor = vec4(result, 1.0f);
    //FragColor = texture(ourTexture, texCoord);
};
