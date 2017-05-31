#include "stdafx.h"
#include <string>
#include "font.h"
#include "util.h"
#include <vector>


GLuint texFont;
GLuint FontvtxArray;
GLuint FontBuffer;
GLuint FontPrg;

float g_fontSize = 16.0;

void InitFont(int frameWidth,int frameHeight, float fontsize)
{
    g_fontSize = fontsize;
    unsigned char* pixels = NULL;
    int width = 0 ,height= 0,channel = 0;

    LoadBmp("Font.bmp",pixels,width,height,channel);
    glGenTextures(1,&texFont);
    glActiveTexture(GL_TEXTURE8);
	glBindTexture(GL_TEXTURE_2D, texFont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GLint itnFormat = GL_RGB8;
    GLenum fmt = GL_RGB;
    if(channel == 4)
    {
        itnFormat = GL_RGBA8;
        fmt = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D,0,itnFormat,width,height,0,fmt,GL_UNSIGNED_BYTE,pixels);

    delete []pixels;
    checkErrors("create font textures");
    const char* shsrcfiles[] = {"font.vs","font.ps"};
    GLuint shaders[2] = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    makeProg(FontPrg,2,shaders,shsrcfiles, true);
    checkErrors("Create font shader");
    glUseProgram(FontPrg);
    GLint normalPos  = glGetUniformLocation(FontPrg,"framesizereq");
    float framebufferreq[] = {1.0f/frameWidth,1.0f/frameHeight};
    glUniform2fv(normalPos,1,framebufferreq);
    GLint heightPos = glGetUniformLocation(FontPrg,"frameheight");
    glUniform1f(heightPos,(float)frameHeight);

    GLint texpos = glGetUniformLocation(FontPrg,"fontTex");
    glUniform1i(texpos,8);
    checkErrors("");

    GLint posPtr = glGetAttribLocation(FontPrg, "pos");
    GLint uvPtr = glGetAttribLocation(FontPrg,"uvs");

    glGenVertexArrays(1,&FontvtxArray);
    glBindVertexArray(FontvtxArray);
    glGenBuffers(1,&FontBuffer);
    glBindBuffer(GL_ARRAY_BUFFER,FontBuffer);
    unsigned char* ptr = 0;
    glEnableVertexAttribArray(posPtr);
    glVertexAttribPointer(posPtr, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ptr);
    ptr += 2 * sizeof(float);
    glEnableVertexAttribArray(uvPtr);
    glVertexAttribPointer(uvPtr,2,GL_FLOAT,GL_FALSE,4 * sizeof(float),ptr);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    

}
const float req = 1.0f/16.0f;
const float texs = 0.0625f;
std::vector<float> fontbuffer;
void DrawString(const char* String,int startx,int starty)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glBindVertexArray(FontvtxArray);
    float startXPos = (float)startx;
    float startYPos = (float)starty;
    size_t cn = strlen(String); //characters number

    /*float* fontbuffer = new float[cn * 4 * 4],*fontbPtr ;
    fontbPtr = fontbuffer;*/
    fontbuffer.clear();
    float cx,cy;

    for(size_t i = 0;i < cn;++i)
    {
        char texpos = String[i]  - 32;
        cx = (texpos%16) * req;
        cy = (texpos/16) * req;


        fontbuffer.push_back( startXPos);
        fontbuffer.push_back( startYPos);
        
        fontbuffer.push_back( cx);
        fontbuffer.push_back( cy);

        fontbuffer.push_back( startXPos);
        fontbuffer.push_back(startYPos + g_fontSize);

        fontbuffer.push_back( cx);
        fontbuffer.push_back( cy + texs);

        fontbuffer.push_back(startXPos + g_fontSize);
        fontbuffer.push_back(startYPos + g_fontSize);

        fontbuffer.push_back( cx + texs);
        fontbuffer.push_back( cy + texs);

        fontbuffer.push_back(startXPos + g_fontSize);
        fontbuffer.push_back( startYPos);

        fontbuffer.push_back( cx + texs);
        fontbuffer.push_back( cy) ;

        startXPos += g_fontSize*0.625;
    }

    
    glBindBuffer(GL_ARRAY_BUFFER,FontBuffer);
    glBufferData(GL_ARRAY_BUFFER,cn * 4 * 4 * sizeof(float),fontbuffer.data(),GL_STREAM_DRAW);

    glUseProgram(FontPrg);
    glDrawArrays(GL_QUADS,0,cn * 4);
    checkErrors("");
    //glDisable(GL_BLEND);    
    
}