#version 450

in vec4 v_Color;
in vec2 v_TextureCoord;
flat in int v_TextureSlotIndex;

out vec4 o_Color;

uniform sampler2D u_Textures[32];

void main()
{
  o_Color = v_Color;
  switch(v_TextureSlotIndex)
  {
    case  0: o_Color *= texture(u_Textures[ 0], v_TextureCoord); break;
    case  1: o_Color *= texture(u_Textures[ 1], v_TextureCoord); break;
    case  2: o_Color *= texture(u_Textures[ 2], v_TextureCoord); break;
    case  3: o_Color *= texture(u_Textures[ 3], v_TextureCoord); break;
    case  4: o_Color *= texture(u_Textures[ 4], v_TextureCoord); break;
    case  5: o_Color *= texture(u_Textures[ 5], v_TextureCoord); break;
    case  6: o_Color *= texture(u_Textures[ 6], v_TextureCoord); break;
    case  7: o_Color *= texture(u_Textures[ 7], v_TextureCoord); break;
    case  8: o_Color *= texture(u_Textures[ 8], v_TextureCoord); break;
    case  9: o_Color *= texture(u_Textures[ 9], v_TextureCoord); break;
    case 10: o_Color *= texture(u_Textures[10], v_TextureCoord); break;
    case 11: o_Color *= texture(u_Textures[11], v_TextureCoord); break;
    case 12: o_Color *= texture(u_Textures[12], v_TextureCoord); break;
    case 13: o_Color *= texture(u_Textures[13], v_TextureCoord); break;
    case 14: o_Color *= texture(u_Textures[14], v_TextureCoord); break;
    case 15: o_Color *= texture(u_Textures[15], v_TextureCoord); break;
    case 16: o_Color *= texture(u_Textures[16], v_TextureCoord); break;
    case 17: o_Color *= texture(u_Textures[17], v_TextureCoord); break;
    case 18: o_Color *= texture(u_Textures[18], v_TextureCoord); break;
    case 19: o_Color *= texture(u_Textures[19], v_TextureCoord); break;
    case 20: o_Color *= texture(u_Textures[20], v_TextureCoord); break;
    case 21: o_Color *= texture(u_Textures[21], v_TextureCoord); break;
    case 22: o_Color *= texture(u_Textures[22], v_TextureCoord); break;
    case 23: o_Color *= texture(u_Textures[23], v_TextureCoord); break;
    case 24: o_Color *= texture(u_Textures[24], v_TextureCoord); break;
    case 25: o_Color *= texture(u_Textures[25], v_TextureCoord); break;
    case 26: o_Color *= texture(u_Textures[26], v_TextureCoord); break;
    case 27: o_Color *= texture(u_Textures[27], v_TextureCoord); break;
    case 28: o_Color *= texture(u_Textures[28], v_TextureCoord); break;
    case 29: o_Color *= texture(u_Textures[29], v_TextureCoord); break;
    case 30: o_Color *= texture(u_Textures[30], v_TextureCoord); break;
    case 31: o_Color *= texture(u_Textures[31], v_TextureCoord); break;
  }
}
