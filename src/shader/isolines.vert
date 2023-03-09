#version 330 core

layout (location = 0) in vec2 pos;

uniform float window_width;
uniform float window_height;

void main()
{
	gl_Position = vec4(2.0f * pos.x / window_width  - 1.0f,
					   2.0f * pos.y / window_height - 1.0f,
					   0.0f, 1.0f);
}