#pragma once
#include <bit>
#include <cmath>
#include <cstdint>
#include <span>
#include <vector>
namespace gz {
struct Glyph{float offset,width,minX,minY,maxX,maxY;};
struct FontData{
 float baseSize=0,ascender=0,descender=0;
 uint32_t format=0,width=0,height=0;
 size_t textureOffset=0;
 std::vector<Glyph> glyphs;
};
inline uint32_t fontU32(const unsigned char* p){return (uint32_t(p[0])<<24)|(uint32_t(p[1])<<16)|(uint32_t(p[2])<<8)|p[3];}
inline float fontF32(const unsigned char* p){return std::bit_cast<float>(fontU32(p));}
inline bool decodeFont(std::span<const unsigned char> bytes,FontData& output){
 if(bytes.size()<36)return false;
 const auto* p=bytes.data();if(fontU32(p)!=0x464e5430)return false; // FNT0
 FontData f;f.baseSize=fontF32(p+4);f.ascender=fontF32(p+8);f.descender=fontF32(p+12);
 const uint32_t count=fontU32(p+16);
 if(count<128||count>256||!std::isfinite(f.baseSize)||f.baseSize<=0||
    !std::isfinite(f.ascender)||!std::isfinite(f.descender)||bytes.size()<36+count*24)return false;
 const size_t offset=20+count*24;
 if(fontU32(p+offset)!=0x54455830)return false; // TEX0
 f.format=fontU32(p+offset+4);f.width=fontU32(p+offset+8);f.height=fontU32(p+offset+12);
 if((f.format!=1&&f.format!=2&&f.format!=3)||!f.width||!f.height||f.width>4096||f.height>4096||
    f.width%8||f.height%(f.format==1?8:4))return false;
 f.textureOffset=offset+16;
 const size_t size=size_t(f.width)*f.height/(f.format==1?2:1);
 if(bytes.size()!=f.textureOffset+size)return false;
 for(uint32_t i=0;i<count;i++){
  const auto* g=p+20+i*24;
  Glyph v{fontF32(g),fontF32(g+4),fontF32(g+8),fontF32(g+12),fontF32(g+16),fontF32(g+20)};
  if(!std::isfinite(v.offset)||!std::isfinite(v.width)||!std::isfinite(v.minX)||!std::isfinite(v.minY)||
     !std::isfinite(v.maxX)||!std::isfinite(v.maxY))return false;
  f.glyphs.push_back(v);
 }
 output=std::move(f);return true;
}
}
