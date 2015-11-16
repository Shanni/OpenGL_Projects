#include <windows.h>

#include <e2gui.h>
#include <e2core.h>
#include <e2render.h>

using namespace et;

Ce2Font::Ce2Font(std::string face, int size, int params)
{
 init(face, size, params);
}

void Ce2Font::init(std::string face, int size, int params)
{
 HDC dc = CreateCompatibleDC(0);
 HFONT font = CreateFont(size, 0, 0, 0, (params & ET_FONT_BOLD) != 0 ? FW_BOLD : FW_NORMAL, params & ET_FONT_ITALIC, params & ET_FONT_UNDERLINE, 0, 
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, face.c_str());
 SelectObject(dc, font);

 ABC abc[256];
 int char_widths[256];
 GetCharABCWidths(dc, 0, 255, abc);
 
 int x_max = 0;
 int row_size = 1;
 for (int i = 0; i < 256; ++i)
 {
  char_widths[i] = abc[i].abcA + abc[i].abcB + abc[i].abcC;
  row_size += 4 + char_widths[i];
  if (i && (i % 32 == 0))
  {
   x_max = max(x_max, row_size);
   row_size = 0;
  }
 }

 int size_x = roundToHighestPowerOfTow(x_max);

 int pos_x = 1;
 int pos_y = size + 4;
 int max_y = 0;
 for (int a = 0; a < 256; ++a)
 {
  if (pos_x + char_widths[a] > size_x)
  {
   pos_y += size + 4;
   pos_x = 1;
  }
  pos_x += char_widths[a] + 4;
 }

 int size_y = roundToHighestPowerOfTow(pos_y);

 int data_size = size_x * size_y * 4;

 BITMAPINFO bmi = {0};
 bmi.bmiHeader.biBitCount = 32;
 bmi.bmiHeader.biHeight = -size_y;
 bmi.bmiHeader.biWidth = size_x;
 bmi.bmiHeader.biSizeImage = data_size;
 bmi.bmiHeader.biSize = sizeof(bmi);
 bmi.bmiHeader.biPlanes = 1;

 unsigned char* data = new unsigned char[data_size];
 memset(data, 0, data_size);

 HBITMAP bitmap = CreateBitmap(size_x, size_y, 1, 32, 0);

 SelectObject(dc, bitmap);
 SetTextColor(dc, 0xFFFFFF);
 SetBkMode(dc, TRANSPARENT);
 PatBlt(dc, 0, 0, size_x, size_y, BLACKNESS);

 pos_x = 1;
 pos_y = 0;
 max_y = 0;
 for (int a = 0; a < 256; ++a)
 {
  if (pos_x + char_widths[a] > size_x)
  {
   pos_x = 1;
   pos_y += size + 4;
  }

  _charDesc[a].originX = pos_x;
  _charDesc[a].originY = pos_y;
  _charDesc[a].glyphHeight = size;
  _charDesc[a].glyphWidth = char_widths[a];

  TextOut(dc, pos_x, pos_y, (LPCSTR)&a, 1);
  pos_x += char_widths[a] + 4;
 }

 GetDIBits(dc, bitmap, 0, size_y, data, &bmi, DIB_RGB_COLORS);
 
 for (int i = 0; i < data_size / 4; ++i)
 {
   unsigned char r = data[4*i+0];
   unsigned char g = data[4*i+1];
   unsigned char b = data[4*i+2];
   data[4*i+3] = (r + g + b) / 3;
   data[4*i+0] = 255;
   data[4*i+1] = 255;
   data[4*i+2] = 255;
 }

 _texture = manager()->genTexture2D(face + " texture", size_x, size_y, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, data);
/*
 string fname = "E:\\data" + face + IntToStr(size) + ".raw";
 FILE* f = fopen( fname.c_str(), "w");
 fwrite(data, data_size, 1, f);
 fclose(f);

 fname = "E:\\data" + face + IntToStr(size) + ".info.txt";
 ofstream of(fname);
 of << size_x << "x" << size_y;
 of.close();
*/
 delete [] data;

 DeleteObject(bitmap);
 DeleteObject(font);
 DeleteDC(dc);
}

Ce2Font::~Ce2Font()
{
 manager()->unloadTexture(_texture);
}

bool Ce2GUILayer::onMouseDown(int x, int y, int button)
{
 return true;
}

bool Ce2GUILayer::onMouseMove(int x, int y)
{
 return true;
}

bool Ce2GUILayer::onMouseUp(int x, int y, int button)
{
 return true;
}

Ce2GUI::Ce2GUI()
{
 _copyProgram = manager()->loadProgram("shaders\\copy_2d.program");
}

void Ce2GUI::createFontTexture(std::string fontName, int fontSize, int fontParams)
{
}

int Ce2GUI::textWidth(Ce2Font* font, std::string text)
{
 int result = 0;
 for (unsigned int i = 0; i < text.length(); ++i)
 {
  unsigned char c = text[i];
  result += font->charDesc(c).glyphWidth;
 }

 return result;
}

void Ce2GUI::beginRenderUI()
{
 _blendEnabled = render()->blend();
 _lastCullingState = render()->cullFace();
 render()->blend(true);
 render()->cullFace(GL_NONE);
 glDepthFunc(GL_ALWAYS);
}

void Ce2GUI::endRenderUI()
{
 glDepthFunc(GL_LESS);
 if (!_blendEnabled)
  render()->blend(false);
 render()->cullFace(_lastCullingState);
}

void Ce2GUI::renderText(Ce2Font* font, const std::string& text, int x, int y, 
  unsigned short alignment, float scale)
{
 render()->bindTexture(0, font->texture());

 float dx_scale = scale * 2.0f / core.windowSize.x;
 float dy_scale = scale * 2.0f / core.windowSize.y;
 float inv_tex_x = font->texture()->texel.x;
 float inv_tex_y = font->texture()->texel.y;

 int text_width  = textWidth(font, text);
 int text_height = font->charDesc(0).glyphHeight;

 int h_align = alignment & 0x00FF;
 int v_align = (alignment & 0xFF00) >> 8;

 if (h_align == ET_ALIGNMENT_CENTER) x -= text_width / 2;
 if (h_align == ET_ALIGNMENT_FAR)    x -= text_width;
 if (v_align == ET_ALIGNMENT_CENTER) y -= text_height / 2;
 if (v_align == ET_ALIGNMENT_FAR)    y -= text_height;

 float x0 = x * dx_scale / scale - 1.0f; 
 float y0 = 1.0f - y * dy_scale / scale;
 float source_x0 = x0;

 int text_len = text.length();
 for (int i = 0; i < text_len; ++i)
  if ( (text[i] == 10) || (text[i] == 13) ) 
   text_len--;

 int num_verts = 4 * text_len;
 VERT_V2_T2* verts = new VERT_V2_T2[num_verts];

 int k = 0;
 for (unsigned int i = 0; i < text.length(); ++i)
 {
  unsigned char c = text[i];
  int glyphHeight = font->charDesc(c).glyphHeight;
  int glyphWidth = font->charDesc(c).glyphWidth;

  float dy = glyphHeight * dy_scale;

  if ((c == 13) || (c == 10))
  {
   y0 -= dy;
   x0 = source_x0;
   continue;
  }

  int four_k = 4*k;
  float u = font->charDesc(c).originX * inv_tex_x;
  float v = font->charDesc(c).originY * inv_tex_y;
  float dx = glyphWidth * dx_scale;
  float du = glyphWidth * inv_tex_x;
  float dv = glyphHeight * inv_tex_y;
  verts[four_k+0] = VERT_V2_T2(   x0, y0 - dy,      u, v + dv);
  verts[four_k+1] = VERT_V2_T2(   x0,      y0,      u,      v);
  verts[four_k+2] = VERT_V2_T2(x0+dx, y0 - dy, u + du, v + dv);
  verts[four_k+3] = VERT_V2_T2(x0+dx,      y0, u + du,      v);
  x0 += dx;
  k++;
 }

 if (render()->supportShaders())
 {
  _copyProgram->bind();
  glEnableVertexAttribArray(RENDER_ATTRIB_TEXCOORD0);
  glEnableVertexAttribArray(RENDER_ATTRIB_POSITION);
  glVertexAttribPointer(RENDER_ATTRIB_TEXCOORD0, 2, GL_FLOAT, false, sizeof(VERT_V2_T2), &verts[0].vTexCoord);
  glVertexAttribPointer(RENDER_ATTRIB_POSITION, 2, GL_FLOAT, false, sizeof(VERT_V2_T2), &verts[0].vPosition);
 }
 else
 {
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glTexCoordPointer(2, GL_FLOAT, sizeof(VERT_V2_T2), &verts[0].vTexCoord);
  glVertexPointer(2, GL_FLOAT, sizeof(VERT_V2_T2), &verts[0].vPosition);
 }
 glDrawArrays(GL_TRIANGLE_STRIP, 0, num_verts);

 delete [] verts;
}
