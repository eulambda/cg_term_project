#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;
  
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 viewPos;

void main()
{
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    if(dot(viewDir, norm) >0){
        FragColor =  vec4(color1, 1.0);
    }
    else{
        FragColor =  vec4(color2, 1.0);
    }

} 