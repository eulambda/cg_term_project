#version 330 core
out vec4 FragColor;

#define NR_CAMERA_LIGHTS 3
struct CameraLight {
    float intensity;
    vec3 color;
    vec3 offset;
};

in vec3 Normal;  
in vec3 FragPos;
  
uniform vec3 objectColor;
uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float specularPower;
uniform CameraLight camera_lights[NR_CAMERA_LIGHTS];

vec3 CalcCameraLight(CameraLight light, vec3 viewDir, vec3 norm) {
    // diffuse 
    vec3 lightDir = normalize(viewPos + light.offset - FragPos);
    float is_facing = dot(viewDir, norm);
    float dotNormLight = dot(norm, lightDir);

    if (is_facing < 0) {
        dotNormLight = -dotNormLight; 
    }
    float diff = max(dotNormLight, 0.0);
    
    vec3 diffuse = diff * light.color;
    
    // specular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec;
    if (dotNormLight > 0)
        spec = pow(max(dot(viewDir, reflectDir), 0.0), specularPower);
    else spec = 0.0f; 
    vec3 specular = specularStrength * spec * light.color;
    
    return (diffuse + specular) * light.intensity;
}
void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    vec3 result = ambientStrength * ambientColor;
    for(int i=0;i<NR_CAMERA_LIGHTS;i++){
        result+=CalcCameraLight(camera_lights[i], viewDir, norm);
    }
    FragColor =  vec4(result * objectColor, 1.0);
} 