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
  
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 viewPos;
uniform vec3 ambientColor;
uniform float ambientStrength;
uniform float specularStrength;
uniform float specularPower;
uniform CameraLight camera_lights[NR_CAMERA_LIGHTS];

vec3 CalcCameraLight(CameraLight light, vec3 viewDir, vec3 norm) {
    // diffuse 
    vec3 lightDir = normalize(viewPos + light.offset - FragPos);
    float dotNormLight = dot(norm, lightDir);

    float diff = abs(dotNormLight);
    
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
    if(dot(viewDir, norm) >0){
        FragColor =  vec4(result * color1, 1.0);
    }
    else{
        FragColor =  vec4(result * color2, 1.0);
    }

} 