// This standalone executable does not link or launch the game.
#include "helpers/string.hpp"
#include "d/d_save.h"
#include <array>
#include <bit>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <type_traits>
static unsigned be16(const unsigned char* p){return (unsigned(p[0])<<8)|p[1];}
static unsigned be32(const unsigned char* p){return (unsigned(p[0])<<24)|(unsigned(p[1])<<16)|(unsigned(p[2])<<8)|p[3];}
int main(int argc,char** argv){
 static_assert(std::is_trivially_copyable_v<dSv_save_c>);
 static_assert(sizeof(dSv_save_c)==0x958);
 if(argc!=2)return 1;
 unsigned count=0;
 for(const auto& file:std::filesystem::recursive_directory_iterator(argv[1])){
  if(file.path().extension()!=".bin")continue;
  std::ifstream f(file.path(),std::ios::binary);
  std::array<unsigned char,sizeof(dSv_save_c)> bytes{};
  if(!f.read(reinterpret_cast<char*>(bytes.data()),bytes.size()))return 2;
  auto save=std::bit_cast<dSv_save_c>(bytes);
  if(save.mPlayer.mPlayerStatusA.getLife()!=be16(bytes.data()+2)||
     save.mPlayer.mPlayerStatusA.getMaxLife()!=be16(bytes.data())||
     save.mPlayer.mPlayerStatusA.getRupee()!=be16(bytes.data()+4)||
     save.mPlayer.mPlayerStatusA.getOil()!=be16(bytes.data()+8))return 3;
  if(std::memcmp(save.mPlayer.mPlayerReturnPlace.mName,bytes.data()+0x58,8)||
     save.mPlayer.mPlayerReturnPlace.getPlayerStatus()!=bytes[0x60]||
     save.mPlayer.mPlayerReturnPlace.getRoomNo()!=static_cast<signed char>(bytes[0x61]))return 4;
  for(unsigned stage=0;stage<32;stage++)
   for(unsigned word=0;word<4;word++)
    if(save.mSave[stage].mBit.mSwitch[word]!=be32(bytes.data()+0x1f0+stage*32+8+word*4))return 5;
  if(save.mMiniGame.getRaceGameTime()!=be32(bytes.data()+0x94c))return 6;
  if(std::bit_cast<unsigned>(save.mPlayer.mPlayerStatusB.getTime())!=be32(bytes.data()+0x34))return 7;
  count++;
 }
 if(count!=343)return 8;
 std::cout<<count<<" distinct practice saves: native big-endian fields and layout verified\n";
}
