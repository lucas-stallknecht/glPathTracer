#version 330 core

out vec4 FragColor;
in vec3 ourColor;

uniform vec4 u_col;

void main()
{
    FragColor = vec4(ourColor.rgb + u_col.rgb, 1.0);
}
