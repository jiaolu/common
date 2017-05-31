#ifndef _UTIL_H
#define _UTIL_H



void checkErrors(const char* desc);
bool makeProg(GLuint& prg,int shader_count,GLuint shaders[], const char** filenames,bool fromfile);
bool makeComputeProg(GLuint& prg,GLuint& shader, const char** filenames, const int files_num);
GLuint makeshader(const GLuint& prg,const char* filename, bool fromfile);
GLuint makeshader(const GLuint& prg, GLuint shadertype, const char* filename, bool fromfile);
GLuint makeshader(const GLuint& prg, GLenum shaderType, const char** filename, const int files_num, bool fromfile);
void LoadBmp(const char* fileName,unsigned char *&pixels,int& width,int& height,int& channel);
bool saveToBmp(unsigned char *pixels,unsigned int width,unsigned int height,const char* fileName, int nChannels = 3);
void GetShaderSrc(std::string& src, const char* filename);


#endif