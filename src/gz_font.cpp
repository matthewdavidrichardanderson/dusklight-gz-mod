// Higher-resolution original font outlines with unchanged GZ layout metrics.
#include "gz_font.hpp"
#include "foreground.hpp"
#include "presentation.hpp"
#include "font_pixels.hpp"
#include "mods/svc/resource.h"
#include "helpers/gx_helper.h"
#include <array>
#include <vector>
namespace gz {
struct Font{FontData data;uint64_t texture=0;};
static std::array<Font,9> fonts;
static int loadedFont=-1;
static bool drawing=false;
static std::vector<std::array<float,2>> vertices;
static std::vector<uint32_t> vertexColors;
static unsigned primitive=0,lineWidth=16;
ModResult initGzFont(){
 const int next=fontChoice();if(fonts[next].texture){loadedFont=next;return MOD_OK;}
 ResourceBuffer buffer=RESOURCE_BUFFER_INIT;
 static const char* names[]={"consola","calamity-bold","lib-sans","lib-sans-bold","lib-serif","lib-serif-bold","press-start-2p","comic-sans","triforce"};
 const auto path=std::string("fonts/")+names[next]+".fnt";
 auto r=svc_resource->load(mod_ctx,path.c_str(),&buffer);if(r!=MOD_OK)return r;
 FontData data;std::span bytes(static_cast<const unsigned char*>(buffer.data),buffer.size);
 if(!decodeFont(bytes,data)){svc_resource->free(mod_ctx,&buffer);return MOD_ERROR;}
 auto rgba=fontPixels(data,bytes.subspan(data.textureOffset));
 svc_resource->free(mod_ctx,&buffer);
 if(rgba.empty())return MOD_ERROR;
 // The native GX adapter retains pixel storage for the texture lifetime.
 const auto texture=foregroundTexture(data.width,data.height,rgba.data());if(!texture)return MOD_ERROR;
 fonts[next]={std::move(data),texture};loadedFont=next;return MOD_OK;
}
void shutdownGzFont(){for(auto& font:fonts)font={};loadedFont=-1;drawing=false;}
void beginGzDraw(){
 drawing=beginForeground();if(!drawing)return;
 if(loadedFont!=fontChoice()&&initGzFont()!=MOD_OK&&loadedFont<0)drawing=false;
}
// Triforce digits use oversized display capitals; reduce them around the
// same baseline, including their advance so alignment and measurement agree.
static float glyphScale(unsigned char c){return loadedFont==8&&c>='0'&&c<='9'?.85f:1.f;}
static void drawText(const std::string& text,float x,float y,uint32_t color,float size){
 if(!drawing||loadedFont<0)return;
 const auto& font=fonts[loadedFont];const auto& f=font.data;
 const float baseScale=size/f.baseSize;
 for(unsigned char c:text){
  if(c>=f.glyphs.size())c='?';
  const auto& g=f.glyphs[c];
  const float scale=baseScale*glyphScale(c);
  foregroundQuad(font.texture,x+g.offset*scale,y-f.ascender*scale,x+(g.width+g.offset)*scale,y+f.descender*scale,g.minX,g.minY,g.maxX,g.maxY,color);
  x+=g.width*scale;
 }
}
void drawGzText(const std::string& text,float x,float y,uint32_t color,float size){
 if(on("drop_shadows"))drawText(text,x+1,y+1,0x000000ff,size);drawText(text,x,y,color,size);
}
float gzTextWidth(const std::string& text,float size){
 if(loadedFont<0)return 0;const auto& f=fonts[loadedFont].data;
 float width=0;for(unsigned char c:text){if(c>=f.glyphs.size())c='?';width+=f.glyphs[c].width*size/f.baseSize*glyphScale(c);}return width;
}
void drawGzTextPlain(const std::string& text,float x,float y,uint32_t color,float size){drawText(text,x,y,color,size);}
void beginGzShape(unsigned count,unsigned kind,unsigned width){
 primitive=kind;lineWidth=width;vertices.clear();vertexColors.clear();vertices.reserve(count);vertexColors.reserve(count);
}
void gzShapeVertex(float x,float y,uint32_t color){vertices.push_back({x,y});vertexColors.push_back(color);}
void endGzShape(){
 if(!drawing)return;
 auto line=[&](size_t a,size_t b){foregroundLine(vertices[a][0],vertices[a][1],vertices[b][0],vertices[b][1],vertexColors[a],float(lineWidth)/6);};
 auto triangle=[&](size_t a,size_t b,size_t c){foregroundTriangle(vertices[a][0],vertices[a][1],vertices[b][0],vertices[b][1],vertices[c][0],vertices[c][1],vertexColors[a]);};
 if(primitive==GX_LINES){for(size_t i=0;i+1<vertices.size();i+=2)line(i,i+1);}
 else if(primitive==GX_LINESTRIP){for(size_t i=1;i<vertices.size();i++)line(i-1,i);}
 else if(primitive==GX_TRIANGLEFAN){for(size_t i=2;i<vertices.size();i++)triangle(0,i-1,i);}
 else if(primitive==GX_TRIANGLESTRIP){for(size_t i=2;i<vertices.size();i++)triangle(i-2,i-1,i);}
 else if(primitive==GX_TRIANGLES){for(size_t i=0;i+2<vertices.size();i+=3)triangle(i,i+1,i+2);}
}
void endGzDraw(){drawing=false;}
}
