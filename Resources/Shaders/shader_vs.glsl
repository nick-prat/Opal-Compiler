#version 330

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Color;

out vec4 fColor;

uniform mat4 gMVP;

void main()
{
    gl_Position = gMVP * vec4(Position, 1.0);
    fColor = vec4(Color, 1.0);
    //gl_Position = vec4(Position, 1.0);
    //gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
}
