#version 330 core
in  vec2  TexCoords;
out vec4  color;
  
uniform sampler2D scene;
uniform float transition;

void main()
{
    color =  texture(scene, TexCoords) * vec4(transition,transition,transition,1.0);
}