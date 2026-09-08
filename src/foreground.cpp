// GZ is part of the game framebuffer, after JFWDisplay's final fade.
#include "foreground.hpp"
#include "presentation.hpp"
#include "presentation_layout.hpp"
#include "helpers/gx_helper.h"
#include "m_Do/m_Do_graphic.h"
#include <algorithm>
#include <memory>
#include <unordered_map>
#include <cstring>
#include "mods/svc/resource.h"
namespace gz {
struct Texture {
 std::vector<unsigned char> pixels;
 GXTexObjRAII object;
};
static std::vector<std::unique_ptr<Texture>> textures;
static uint64_t blank=0;
static std::unordered_map<std::string,uint64_t> resources;
ModResult initForeground(){const uint32_t white=0xffffffff;blank=foregroundTexture(1,1,&white);return MOD_OK;}
void shutdownForeground(){AuroraGXSync();textures.clear();resources.clear();blank=0;}
static uint64_t texture(unsigned width,unsigned height,const void* rgba,GXTexFmt format){
 auto tex=std::make_unique<Texture>();
 const auto* bytes=static_cast<const unsigned char*>(rgba);
 tex->pixels.assign(bytes,bytes+size_t(width)*height*4);
 GXInitTexObj(&tex->object,tex->pixels.data(),width,height,format,GX_CLAMP,GX_CLAMP,GX_FALSE);
 GXInitTexObjLOD(&tex->object,GX_LINEAR,GX_LINEAR,0,0,0,GX_FALSE,GX_FALSE,GX_ANISO_1);
 textures.push_back(std::move(tex));return textures.size();
}
uint64_t foregroundTexture(unsigned width,unsigned height,const void* rgba){return texture(width,height,rgba,GX_TF_RGBA8_PC);}
uint64_t foregroundConsoleTexture(unsigned width,unsigned height,const void* rgba){return texture(width,height,rgba,GX_TF_RGBA8);}
uint64_t foregroundResourceTexture(const char* path){
 if(auto it=resources.find(path);it!=resources.end())return it->second;
 ResourceBuffer buffer=RESOURCE_BUFFER_INIT;uint64_t id=0;
 if(svc_resource->load(mod_ctx,path,&buffer)==MOD_OK){
  const auto* bytes=static_cast<const unsigned char*>(buffer.data);
  if(bytes&&buffer.size>=16&&std::memcmp(bytes,"TEX0",4)==0){
   auto read=[&](unsigned i){return (uint32_t(bytes[i])<<24)|(uint32_t(bytes[i+1])<<16)|(uint32_t(bytes[i+2])<<8)|bytes[i+3];};
   const auto width=read(8),height=read(12);
   if(read(4)==0&&width&&height&&width<=1024&&height<=1024&&buffer.size==16+size_t(width)*height*4)
    id=foregroundConsoleTexture(width,height,bytes+16);
  }
  svc_resource->free(mod_ctx,&buffer);
 }
 if(!id)svc_log->error(mod_ctx,(std::string("GZ icon unavailable: ")+path).c_str());
 resources.emplace(path,id);return id;
}
bool beginForeground(){
 // Aurora maps logical GX viewports independently in X/Y. Fit in actual
 // render pixels instead, so game aspect changes cannot stretch GZ.
 u32 renderWidth=0,renderHeight=0;AuroraGetRenderSize(&renderWidth,&renderHeight);
 if(!renderWidth||!renderHeight)return false;
 const auto viewport=overlayViewport(float(renderWidth),float(renderHeight));
 GXSetViewportRender(viewport.left,viewport.top,viewport.width,viewport.height,0,1);
 GXSetScissorRender(0,0,renderWidth,renderHeight);
 Mtx44 projection;
 MTXOrtho(projection,0,CanvasHeight,0,viewport.canvasWidth,-1,1);
 GXSetProjection(projection,GX_ORTHOGRAPHIC);
 Mtx identity;MTXIdentity(identity);GXLoadPosMtxImm(identity,GX_PNMTX0);GXSetCurrentMtx(GX_PNMTX0);
 GXClearVtxDesc();
 GXSetVtxDesc(GX_VA_POS,GX_DIRECT);GXSetVtxDesc(GX_VA_CLR0,GX_DIRECT);GXSetVtxDesc(GX_VA_TEX0,GX_DIRECT);
 GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_POS,GX_POS_XY,GX_F32,0);
 GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_CLR0,GX_CLR_RGBA,GX_RGBA8,0);
 GXSetVtxAttrFmt(GX_VTXFMT0,GX_VA_TEX0,GX_TEX_ST,GX_F32,0);
 GXSetNumChans(1);GXSetChanCtrl(GX_COLOR0A0,GX_DISABLE,GX_SRC_REG,GX_SRC_VTX,GX_LIGHT_NULL,GX_DF_NONE,GX_AF_NONE);
 GXSetNumTexGens(1);GXSetTexCoordGen(GX_TEXCOORD0,GX_TG_MTX2x4,GX_TG_TEX0,GX_IDENTITY);
 GXSetTexCoordScaleManually(GX_TEXCOORD0,GX_FALSE,1,1);
 GXSetZTexture(GX_ZT_DISABLE,GX_TF_Z8,0);GXSetCoPlanar(GX_FALSE);
 GXSetNumIndStages(0);GXSetNumTevStages(1);GXSetTevDirect(GX_TEVSTAGE0);
 GXSetTevOrder(GX_TEVSTAGE0,GX_TEXCOORD0,GX_TEXMAP0,GX_COLOR0A0);
 GXSetTevOp(GX_TEVSTAGE0,GX_MODULATE);
 GXSetTevSwapMode(GX_TEVSTAGE0,GX_TEV_SWAP0,GX_TEV_SWAP0);
 GXSetTevSwapModeTable(GX_TEV_SWAP0,GX_CH_RED,GX_CH_GREEN,GX_CH_BLUE,GX_CH_ALPHA);
 GXSetBlendMode(GX_BM_BLEND,GX_BL_SRCALPHA,GX_BL_INVSRCALPHA,GX_LO_COPY);
 GXSetAlphaCompare(GX_ALWAYS,0,GX_AOP_AND,GX_ALWAYS,0);
 GXSetZMode(GX_FALSE,GX_ALWAYS,GX_FALSE);GXSetZCompLoc(GX_FALSE);
 GXSetCullMode(GX_CULL_NONE);GXSetClipMode(GX_CLIP_ENABLE);
 GXSetFog(GX_FOG_NONE,0,0,0,0,GXColor{0,0,0,0});
 GXSetFogRangeAdj(GX_FALSE,0,nullptr);GXSetColorUpdate(GX_TRUE);GXSetAlphaUpdate(GX_FALSE);
 return true;
}
static void vertex(float x,float y,float u,float v,uint32_t color){
 GXPosition2f32(x,y);GXColor4u8(color>>24,color>>16,color>>8,color);GXTexCoord2f32(u,v);
}
static void use(uint64_t texture){GXLoadTexObj(&textures[texture-1]->object,GX_TEXMAP0);}
void foregroundQuad(uint64_t texture,float left,float top,float right,float bottom,float u0,float v0,float u1,float v1,uint32_t color){
 use(texture);GXBegin(GX_QUADS,GX_VTXFMT0,4);
 vertex(left,top,u0,v0,color);vertex(right,top,u1,v0,color);
 vertex(right,bottom,u1,v1,color);vertex(left,bottom,u0,v1,color);GXEnd();
}
void foregroundLine(float x0,float y0,float x1,float y1,uint32_t color,float width){
 use(blank);GXSetLineWidth(static_cast<u8>(std::clamp(width*6.f,1.f,255.f)),GX_TO_ZERO);
 GXBegin(GX_LINES,GX_VTXFMT0,2);vertex(x0,y0,0,0,color);vertex(x1,y1,0,0,color);GXEnd();
}
void foregroundTriangle(float x0,float y0,float x1,float y1,float x2,float y2,uint32_t color){
 use(blank);GXBegin(GX_TRIANGLES,GX_VTXFMT0,3);
 vertex(x0,y0,0,0,color);vertex(x1,y1,0,0,color);vertex(x2,y2,0,0,color);GXEnd();
}
}
