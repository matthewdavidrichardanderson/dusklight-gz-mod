#include "font_pixels.hpp"
#include <fstream>
#include <iterator>
#include <iostream>
#include <filesystem>
int main(int argc,char** argv){
 if(argc!=2)return 1;
 std::ifstream input(argv[1],std::ios::binary);
 std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(input)),{});
 gz::FontData decoded;
 if(!gz::decodeFont(bytes,decoded)||decoded.format!=3||decoded.width!=2048||decoded.height<=360||
    decoded.glyphs.size()!=256||decoded.baseSize!=26)return 2;
 if(gz::decodeFont(std::span(bytes).first(bytes.size()-1),decoded))return 3;
 auto broken=bytes;broken[0]=0;if(gz::decodeFont(broken,decoded))return 4;
 broken=bytes;broken[16]=0xff;if(gz::decodeFont(broken,decoded))return 5;
 broken=bytes;broken[6164+7]=4;if(gz::decodeFont(broken,decoded))return 6;
 unsigned fonts=0;
 for(const auto& file:std::filesystem::directory_iterator(std::filesystem::path(argv[1]).parent_path())){
  if(file.path().extension()!=".fnt")continue;
  std::ifstream f(file.path(),std::ios::binary);
  std::vector<unsigned char> blob((std::istreambuf_iterator<char>(f)),{});
  if(!gz::decodeFont(blob,decoded)){std::cerr<<file.path();return 7;}
  if(gz::fontPixels(decoded,std::span(blob).subspan(decoded.textureOffset)).size()!=size_t(decoded.width)*decoded.height*4)return 13;
  fonts++;
 }
 if(fonts!=9)return 8;
 gz::FontData cmpr;cmpr.format=1;cmpr.width=cmpr.height=8;
 std::vector<unsigned char> blocks(32);
 // Four 4x4 subtiles: red, green, blue, transparent midpoint.
 const unsigned endpoints[]={0xf800,0x07e0,0x001f,0};
 for(int block=0;block<4;block++){blocks[block*8]=endpoints[block]>>8;blocks[block*8+1]=endpoints[block];}
 blocks[28]=blocks[29]=blocks[30]=blocks[31]=0xff;
 auto rgba=gz::fontPixels(cmpr,blocks);
 auto channel=[&](int x,int y,int c){return rgba[(y*8+x)*4+c];};
 if(rgba.size()!=256||channel(0,0,0)!=255||channel(4,0,1)!=255||channel(0,4,2)!=255||channel(4,4,3)!=0)return 9;
 // GX CMPR gradient uses 5:3 weighting, not desktop DXT1's 2:1.
 blocks[4]=0xaa;rgba=gz::fontPixels(cmpr,blocks);
 if(channel(0,0,0)!=159||channel(0,0,3)!=255)return 10;
 gz::FontData i8;i8.format=2;i8.width=16;i8.height=4;
 std::vector<unsigned char> tiles(64,64);std::fill(tiles.begin()+32,tiles.end(),192);
 rgba=gz::fontPixels(i8,tiles);
 if(rgba[(0*16+7)*4+3]!=64||rgba[(0*16+8)*4+3]!=192||rgba[(3*16+15)*4]!=192)return 11;
 if(!gz::fontPixels(cmpr,std::span(blocks).first(31)).empty())return 12;

 gz::FontData coverage;coverage.format=3;coverage.width=8;coverage.height=4;
 std::vector<unsigned char> alpha(32,64);rgba=gz::fontPixels(coverage,alpha);
 if(rgba.size()!=128||rgba[0]!=255||rgba[1]!=255||rgba[2]!=255||rgba[3]!=64)return 14;
 std::cout<<"All nine GZ font resources decoded. ";
 std::cout<<"HD coverage and original CMPR/I8 decoded; truncated and invalid resources rejected\n";
}
