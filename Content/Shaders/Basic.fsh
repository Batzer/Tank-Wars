#version 330 core

out vec4 colorF;

void main()
{
    colorF = vec4(gl_FragCoord.z / 1280.0, gl_FragCoord.y / 720.0, 1.0, 1.0);
}