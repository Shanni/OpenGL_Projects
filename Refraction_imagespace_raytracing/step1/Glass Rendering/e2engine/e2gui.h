#pragma once

#include <e2manager.h>

#define ET_FONT_BOLD       1
#define ET_FONT_ITALIC     2
#define ET_FONT_UNDERLINE  4

#define ET_ALIGNMENT_NEAR    0
#define ET_ALIGNMENT_CENTER  2
#define ET_ALIGNMENT_FAR  4

const unsigned short ET_ALIGN_TOP_LEFT = 0;
const unsigned short ET_ALIGN_BOTTOM_LEFT = ET_ALIGNMENT_FAR << 8;
const unsigned short ET_ALIGN_BOTTOM_RIGHT = ET_ALIGNMENT_FAR | ET_ALIGNMENT_FAR << 8;
const unsigned short ET_ALIGN_TOP_CENTER = ET_ALIGNMENT_CENTER;

namespace et
{
 struct Ce2CharDesc
 {
  int originX;
  int originY;
  int glyphWidth;
  int glyphHeight;
 };

 class Ce2Font
 {
  public:
   Ce2Font(std::string face, int size, int params);
   ~Ce2Font();

   inline Ce2TextureObject* texture() const {return _texture;}
   inline const Ce2CharDesc& charDesc(unsigned char c) const {return _charDesc[c];}

  private:
    void init(std::string face, int size, int params);

  private:
   Ce2CharDesc _charDesc[256];
   Ce2TextureObject* _texture;
 };

 class Ce2GUILayer
 {
  public:
   bool onMouseDown(int x, int y, int button);
   bool onMouseMove(int x, int y);
   bool onMouseUp(int x, int y, int button);
 };
  
 class Ce2GUI
 {
  public:
   Ce2GUI();

   void createFontTexture(std::string fontName, int fontSize, int fontParams);
   int textWidth(Ce2Font* font, std::string text);
   void renderText(Ce2Font* font, const std::string& text, int x, int y, 
     unsigned short alignment = ET_ALIGN_TOP_LEFT, float scale = 1.0f);

   void beginRenderUI();
   void endRenderUI();

  public:

  private:
   bool _blendEnabled;
   GLenum _lastCullingState;

  private:
   Ce2ProgramObject* _copyProgram;
 };
}