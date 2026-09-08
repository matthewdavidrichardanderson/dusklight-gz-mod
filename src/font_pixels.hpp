#pragma once
#include "font_data.hpp"
#include <array>
namespace gz {
// Matches Aurora texture_convert.cpp: GX 8x8 CMPR tiles, big-endian endpoints,
// 3:5 interpolation, transparent midpoint; I8 uses 8x4 tiles.
inline std::vector<unsigned char> fontPixels(const FontData& f,std::span<const unsigned char> encoded){
 if(encoded.size()!=size_t(f.width)*f.height/(f.format==1?2:1))return {};
 std::vector<unsigned char> pixels(size_t(f.width)*f.height*4);size_t offset=0;
 auto put=[&](unsigned x,unsigned y,const std::array<unsigned char,4>& c){
  for(unsigned k=0;k<4;k++)pixels[(size_t(y)*f.width+x)*4+k]=c[k];
 };
 if(f.format==3){
  for(unsigned y=0;y<f.height;y++)for(unsigned x=0;x<f.width;x++)put(x,y,{255,255,255,encoded[offset++]});
 }else if(f.format==2){
  for(unsigned y=0;y<f.height;y+=4)for(unsigned x=0;x<f.width;x+=8)
   for(unsigned yy=0;yy<4;yy++)for(unsigned xx=0;xx<8;xx++){
    const auto v=encoded[offset++];put(x+xx,y+yy,{v,v,v,v});
   }
 }else{
  auto rgb=[](unsigned c){
   unsigned r=(c>>11)&31,g=(c>>5)&63,b=c&31;
   return std::array<unsigned char,4>{static_cast<unsigned char>((r<<3)|(r>>2)),static_cast<unsigned char>((g<<2)|(g>>4)),static_cast<unsigned char>((b<<3)|(b>>2)),255};
  };
  for(unsigned y=0;y<f.height;y+=8)for(unsigned x=0;x<f.width;x+=8)
   for(unsigned by=0;by<8;by+=4)for(unsigned bx=0;bx<8;bx+=4){
    const auto* p=encoded.data()+offset;offset+=8;
    const unsigned a=(unsigned(p[0])<<8)|p[1],b=(unsigned(p[2])<<8)|p[3];
    std::array<std::array<unsigned char,4>,4> colors{rgb(a),rgb(b)};
    for(unsigned k=0;k<3;k++){
     colors[2][k]=a>b?(3*colors[1][k]+5*colors[0][k])>>3:(colors[0][k]+colors[1][k])>>1;
     colors[3][k]=a>b?(3*colors[0][k]+5*colors[1][k])>>3:colors[2][k];
    }
    colors[2][3]=255;colors[3][3]=a>b?255:0;
    for(unsigned yy=0;yy<4;yy++)for(unsigned xx=0;xx<4;xx++)
     put(x+bx+xx,y+by+yy,colors[(p[4+yy]>>(6-2*xx))&3]);
   }
 }
 return pixels;
}
}
