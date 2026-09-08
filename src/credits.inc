// Original GCN credits layout and attribution, copied from pinned TPGZ.
static bool drawCredits(std::string_view page){if(page!="credits")return false;
 const auto heart=foregroundResourceTexture("tex/heart.tex");
 if(heart)foregroundQuad(heart,315,65,345,95,0,0,1,1,0xffffffff);
 drawGzText("tpgz - the twilight princess practice rom",25.0f,60.0f,0xffffffff,25.0f);
 drawGzText("made with",175.0f,90.0f,0xffffffff,25.0f);
 drawGzText("by",360.0f,90.0f,0xffffffff,25.0f);
 drawGzText("kipcode",135.0f,110.0f,0xffffffff,20.0f);
 drawGzText("pheenoh",265.0f,110.0f,0xffffffff,20.0f);
 drawGzText("taka",390.0f,110.0f,0xffffffff,20.0f);
 drawGzText("special thanks",175.0f,170.0f,0xffffffff,25.0f);
 drawGzText("tp decomp team",85.0f,190.0f,0xffffffff,17.0f);
 drawGzText("tp speedrun community",345.0f,190.0f,0xffffffff,17.0f);
 drawGzText("c_midnight",85.0f,210.0f,0xffffffff,17.0f);
 drawGzText("cryze",245.0f,210.0f,0xffffffff,17.0f);
 drawGzText("jdflyer",345.0f,210.0f,0xffffffff,17.0f);
 drawGzText("icogn",85.0f,230.0f,0xffffffff,17.0f);
 drawGzText("lunarsoap",245.0f,230.0f,0xffffffff,17.0f);
 drawGzText("zephiles",345.0f,230.0f,0xffffffff,17.0f);
 return true;
}
