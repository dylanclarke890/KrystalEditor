#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in int textureSlotIndex;

out vec4 v_Color;
out vec2 v_TextureCoord;
flat out int v_TextureSlotIndex;

uniform mat4 u_Transform;

void main()
{
	gl_Position = u_Transform * vec4(position, 1.0);
  v_Color = color;
  v_TextureCoord = textureCoord;
  v_TextureSlotIndex = textureSlotIndex;
}