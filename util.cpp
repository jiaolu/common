#include "stdafx.h"
#include <string>
#include <assert.h>

void GetShaderSrc(char *& src, const char* filename)
{
	FILE* ft = 0;
	
	fopen_s(&ft,filename,"rt");
	
	if(ft != NULL)
	{
		long b = ftell(ft);
		fseek(ft,0,SEEK_END);
		long e = ftell(ft);
		size_t fsize = e - b;
		src =  new char[fsize+1];
		fseek(ft,0,SEEK_SET);
		fread(src,fsize,1,ft);
		src[fsize]=0;
		

	}
	fclose(ft);
}

void GetShaderSrc(std::string& src, const char* filename)
{
	FILE* ft = 0;
	
	fopen_s(&ft,filename,"rb");
	
	if(ft != NULL)
	{
		long b = ftell(ft);
		fseek(ft,0,SEEK_END);
		long e = ftell(ft);
		size_t fsize = e - b;
        src.resize(fsize);
		
		fseek(ft,0,SEEK_SET);
		fread(&src[0],fsize,1,ft);
		src[fsize]=0;
	}
	fclose(ft);
}




void checkErrors(const char* desc) {
	GLenum e = glGetError();
	if (e != GL_NO_ERROR) {

#ifdef GLU
		fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n", desc, gluGetString(e), e);
#else
		fprintf(stderr, "OpenGL error in \"%s\": %s (%d)\n", desc, "", e);
#endif
		assert(!"stop here");
	}
}

void compileShader(const GLuint shader,const char* shadersrc)
{
    glShaderSource(shader,1,&shadersrc,NULL);
    glCompileShader(shader);
    GLint param,len;
    std::string logstr;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&param);
    
    if(param != GL_TRUE)
    {        
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&len);
        logstr.resize(len + 1);
        glGetShaderInfoLog(shader,len,&len,&logstr[0]);
        printf("shader error:%s\n",logstr.c_str());
    }
}

static int debug_shader_idx = 0;
void compileShader(const GLuint shader, int shader_src_count, const char** shadersrc_array)
{
    glShaderSource(shader,shader_src_count,shadersrc_array,NULL);
    glCompileShader(shader);
    GLint param,len;
    std::string logstr;
    glGetShaderiv(shader,GL_COMPILE_STATUS,&param);

	char debug_shader_name[100];
	sprintf_s(debug_shader_name, "shader_error%d.txt\0", debug_shader_idx++);
    
    if(param != GL_TRUE)
    {        
        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&len);
        logstr.resize(len + 1);
        glGetShaderInfoLog(shader,len,&len,&logstr[0]);
        printf("shader error:%s\n", logstr.c_str());

		FILE* dfh = NULL;
		fopen_s(&dfh, debug_shader_name, "wt");
		fprintf(dfh, "%s\n", logstr.c_str());
		fclose(dfh);
    }
}

GLuint makeshader(const GLuint& prg,GLuint shadertype , const char* filename, bool fromfile)
{        
    GLuint shader = glCreateShader(shadertype);

    std::string strShaderSrc;
    const char* cShadersrc = 0;
    if (fromfile)
    {
        GetShaderSrc(strShaderSrc, filename);
        cShadersrc = strShaderSrc.c_str();
    }        
    else
    {
        cShadersrc = filename;
    }
        
    compileShader(shader, cShadersrc);
    glAttachShader(prg,shader);
//    glDeleteShader(shader);
    return shader;
}


GLuint makeshader(const GLuint& prg,GLenum shaderType, const char** filename, const int files_num,bool fromfile)
{    
    
    GLuint shader;
    
    shader = glCreateShader(shaderType);
    
    std::string shaderstr[16];
    const char* shadersrc[16];

    assert(files_num < 16);
    
    
    for (int i = 0; i < files_num; ++i)
    {
        if (fromfile)
        {
            GetShaderSrc(shaderstr[i], filename[i]);
            shadersrc[i] = shaderstr[i].c_str();
        }
        else
        {
            shadersrc[i] = filename[i];
        }
    }

    compileShader(shader, files_num, shadersrc);
    
    glAttachShader(prg,shader);
//    glDeleteShader(shader);
    return shader;
}


bool makeComputeProg(GLuint& prg,GLuint& shader, const char** filenames, const int files_num)
{
    prg = glCreateProgram();

    shader = makeshader(prg, GL_COMPUTE_SHADER, filenames, files_num, true);
    glLinkProgram(prg);
    
    GLint param,len;
    std::string logstr;
    glGetProgramiv(prg,GL_LINK_STATUS,&param);
	
    if(param != GL_TRUE)
    {
        glGetProgramiv(prg,GL_INFO_LOG_LENGTH,&len);
        logstr.resize(len + 1);
        glGetProgramInfoLog(prg,len,&len,&logstr[0]);
        fprintf(stderr,"%s\n",logstr.c_str());
		FILE* dfh = NULL;
		fopen_s(&dfh, "dbug_compute_prg.txt", "wt");
		fprintf(dfh, "%s\n", logstr.c_str());
		fclose(dfh);
        
        printf("program error:%s\n",logstr.c_str());
		assert(!"program link error");
        return false;
    }

    return true;
}

bool makeProg(GLuint& prg,int shader_count,GLuint shaders[], const char** filenames,bool fromfile)
{    
    prg = glCreateProgram();

    for(int i = 0;i < shader_count;++i)
    {
        shaders[i] = makeshader(prg,shaders[i],filenames[i], fromfile);
    }

    glLinkProgram(prg);
    
    GLint param,len;
    std::string logstr;
    glGetProgramiv(prg,GL_LINK_STATUS,&param);
    if(param != GL_TRUE)
    {
        glGetProgramiv(prg,GL_INFO_LOG_LENGTH,&len);
        logstr.resize(len + 1);
        glGetProgramInfoLog(prg,len,&len,&logstr[0]);
        fprintf(stderr,"%s\n",logstr.c_str());
        assert(!"program link error");
        printf("program error:%s\n",logstr.c_str());
        return false;
    }

    return true;
}

#pragma pack (push, 1)
struct BmpHeader {
	unsigned short bmpIdentifier;
	unsigned char  junk[16];
	unsigned int   width;
	unsigned int   height;
	unsigned short junk2;
	unsigned short bpp;
	unsigned short compression;
	unsigned char  junk3[22];
};
#pragma pack (pop)

bool saveToBmp(unsigned char *pixels, unsigned int width, unsigned int height, const char* fileName, int srcChannels )
{
    FILE *file;
    fopen_s(&file,fileName, "wb");
	if (file == NULL) return false;
    
    int destChannels = 3;

    BmpHeader header = {
        0x4d42, //'b' 'm'
        {0x36, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00},
        width,
        height,
        0x0001,
        destChannels * 8,
        0,
        {0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
    };
    fwrite(&header, sizeof(header), 1, file);
    int red   = 0;
	int blue  = 2;
    int paddBytes = (width * destChannels) & 3;
    if (paddBytes) paddBytes = 4 - paddBytes;
    int len = 0, size = (width * destChannels + paddBytes) * height;
    unsigned char *dest,*src,*buffer = new unsigned char[size];
    
    assert(destChannels >= 3);

    for (unsigned int y = 0; y < height; y++){
			dest = buffer + y * (width * destChannels + paddBytes);
			src  = pixels + y * width * srcChannels;
			len = width;
			do {
				*dest++ = src[blue];
				*dest++ = src[1];
				*dest++ = src[red];
				src += srcChannels;
                dest += (destChannels - 3);
			} while (--len);
		}
    fwrite(buffer, size, 1, file);
    delete []buffer;
    fclose(file);
    return true;
}


void LoadBmp(const char* fileName,unsigned char *&pixels,int& width,int& height,int& channel)
{
    pixels = NULL;
    
    FILE* file;
    fopen_s(&file,fileName,"rb");
    if(file == NULL) return;
    BmpHeader header;
    fread(&header,sizeof(header),1,file);

    if(header.bmpIdentifier != 0x4d42)
    {
        goto end;
    }

    width = header.width;
    height = header.height;
    

    assert(header.bpp == 24 || header.bpp == 32);
    channel = header.bpp/8;
    
    
    int  bytesperLine =  (channel * width + 3)/4 * 4; //4 bytes alignment for images and texture 2D
    int size = height * bytesperLine;
    unsigned char *srcPixels = new unsigned char[size];
    fread(srcPixels,size,1,file);

    pixels = new unsigned char[size];
    
    
    for(int i = 0; i<height; ++i)
    {
        int len = width;
        unsigned char *src = &srcPixels[(height - 1 - i) * bytesperLine];
        unsigned char* dest = &pixels[i * bytesperLine];
        do
        {
            *dest++ = src[2];
            *dest++ = src[1];
            *dest++ = src[0];
            src += channel;
        }
        while(--len);
    }

    delete[]srcPixels;

end:
    fclose(file);
}



