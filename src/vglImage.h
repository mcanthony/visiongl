/*********************************************************************
***                                                                ***
***  Header file vglImage                                          ***
***                                                                ***
*********************************************************************/

#ifndef __VGLIMAGE_H__
#define __VGLIMAGE_H__


#define GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT 0x8CD8
#define GL_FRAMEBUFFER_STATUS_ERROR_EXT 0x8CDE


#define __is_pod(type) 1
#define __is_empty(type) 1
#define __has_trivial_destructor(type) 1
//extern void* __builtin_memchr(const void*__s, int __a, unsigned int __n);
#include <stdarg.h>

#include <cstdio>

//opencv IplImage
#undef __SSE2__
#include <opencv2/core/types_c.h>

//GL
#include <GL/glew.h>
#include <GL/freeglut.h>

//CL
#ifdef __OPENCL__
#include <CL/cl.h>
#endif

#define VGL_WIN_X0   -1.0
#define VGL_WIN_X1    1.0
#define VGL_WIN_DX    (VGL_WIN_X1 - VGL_WIN_X0)
#define VGL_WIN_Y0   -1.0
#define VGL_WIN_Y1    1.0
#define VGL_WIN_DY    (VGL_WIN_Y1 - VGL_WIN_Y0)


// VGL HEADER - Vision with OpenGL


#define VGL_MIN_WINDOW_SPLIT 1
#define VGL_DEFAULT_WINDOW_SPLIT 2
#define VGL_MAX_WINDOW_SPLIT 4
#define VGL_MAX_WINDOWS      (VGL_MAX_WINDOW_SPLIT * VGL_MAX_WINDOW_SPLIT)

#define VGL_MAX_DIM 10
#define VGL_WIDTH 0
#define VGL_HEIGHT 1
#define VGL_LENGTH 2
#define VGL_4D 3

////////// VglImage

class VglImage{
 public:
  IplImage* ipl;
  void*     ndarray;
  int       ndim;                // 2 if conventional image, 3 if three-dimensional etc
  int       shape[VGL_MAX_DIM];  // shape[0] = width, shape[1] = height, shape[2] = number of frames etc
  int       depth;
  int       nChannels;
  int       has_mipmap;
  GLuint    fbo;
  GLuint    tex;
  void*     cudaPtr;
  GLuint    cudaPbo;
#ifdef __OPENCL__
  cl_mem    oclPtr;
#endif
  int       inContext;
  char*     filename;

  size_t getBytesPerPixel()
  {
    size_t bytesPerPixel = this->depth & 255;
    bytesPerPixel /= 8;
    if (bytesPerPixel < 1)
      bytesPerPixel = 1;
    return bytesPerPixel;    
  }

  size_t getTotalSizeInPixels()
  {
    size_t totalSize = 1;
    for(int i = 0; i < this->ndim; i++)
    {
      totalSize *= this->shape[i];
    }
    totalSize *= this->nChannels;

    // 1D images may be stored as 2d images.
    if ( (this->ndim == 1) && (this->shape[VGL_HEIGHT] > 1) )
    {
      totalSize *= this->shape[VGL_HEIGHT];
    }
    return totalSize;
  }

  size_t getTotalSizeInBytes()
  {
    size_t bytesPerPixel = this->getBytesPerPixel();

    size_t totalSize = this->getTotalSizeInPixels() * bytesPerPixel;

    return totalSize;
  }

  char* getImageData()
  {
    if (this->ndarray)
    {
      return (char*) this->ndarray;
    }
    else if (this->ipl)
    {
      return (char*) this->ipl->imageData;
    }
    else
    {
      fprintf(stderr, "%s: %s: Error: no pointer to raster image data available.\n", __FILE__, __FUNCTION__);
      return NULL;
    }     
  }


};

////////// VglNamedWindow

class VglNamedWindow  {
public:
  VglImage* image;
  char* name;
};

////////// vglNamedWindowList


class VglNamedWindowList  {

public:
  VglNamedWindowList(void);
  ~VglNamedWindowList(void);
  //void Init();
  int MaxWindows();
  int CreateNamedSubwindow(char* winname);
  int CreateSubwindow(void);
  int ShowImage(char* name, VglImage* image);
  void Reset(void);
  int Cycle(void);
  void Refresh(int win_index, int split = VGL_DEFAULT_WINDOW_SPLIT);
  void RefreshAll(int split = VGL_DEFAULT_WINDOW_SPLIT);
  int main_window_id;
  int NamedWindow(char* winname);

private:
  VglNamedWindow WindowList[VGL_MAX_WINDOWS];
  int FreePosition[VGL_MAX_WINDOWS];
  int WindowIdByName(char* name);
  int current_window;
};

////////// vglImage functions

//void vglShowImage(char* winname, VglImage* image)
//int vglNamedWindow(char* winname);


int vglInit();
int vglInit(int w, int h);
void vglUpload(VglImage* image, int swapRGB = 0);
VglImage* vglCopyCreateImage(VglImage* img_in);
VglImage* vglCreateImage(VglImage* img_in);
VglImage* vglCopyCreateImage(IplImage* img_in, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreateImage(IplImage* img_in, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreateImage(int* shape, int depth, int nChannels, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreateImage(CvSize size, int depth = IPL_DEPTH_8U, int nChannels = 3, int ndim = 2, int has_mipmap = 0);
VglImage* vglCreate3dImage(CvSize size, int depth, int nChannels, int nlength, int has_mipmap = 0);
VglImage* vglCreateNdImage(int ndim, int* shape, int depth, int nChannels, int has_mipmap = 0);
void vglSave3dImage(VglImage* image, char* filename, int lStart, int lEnd);
VglImage* vglCloneImage(IplImage* img_in, int ndim = 2, int has_mipmap = 0);
void vglReleaseImage(VglImage** p_image);
void vglReplaceIpl(VglImage* image, IplImage* new_ipl);
void vglDownload(VglImage* image);
void vglDownloadPPM(VglImage* image);
void vglDownloadFBO(VglImage* image);
void vglDownloadFaster(VglImage* image/*, VglImage* buf*/);
VglImage* vglLoadImage(char* filename, int iscolor = -1, int has_mipmap = 0); // -1 = CV_LOAD_IMAGE_UNCHANGED
VglImage* vglLoad3dImage(char* filename, int lStart, int lEnd, bool has_mipmap = 0);
void vglPrintImageData(VglImage* image, char* msg = NULL, char* format = (char*) "%c");
void vglPrintImageInfo(VglImage* image, char* msg = NULL);
void iplPrintImageInfo(IplImage* ipl, char* msg = NULL);
void vglCopyImageTex(VglImage* src, VglImage* dst);
void vglAxis(void);
void vglCopyImageTexVFS(VglImage* src, VglImage* dst);
void vglCopyImageTexFS(VglImage* src, VglImage* dst);
void vglGreen(VglImage* src, VglImage* dst);
void vglVerticalFlip2(VglImage* src, VglImage* dst);
void vglHorizontalFlip2(VglImage* src, VglImage* dst);
void vglClear(VglImage* image, float r, float g, float b, float a = 0.0);
void vglNdarray3To4Channels(VglImage* img);
void vglNdarray4To3Channels(VglImage* img);
void vglIpl3To4Channels(VglImage* img);
void vglIpl4To3Channels(VglImage* img);
void vglImage3To4Channels(VglImage* img);
void vglImage4To3Channels(VglImage* img);
int SavePPM(char* filename, int w, int h, void* savebuf);
int vglSavePPM(VglImage* img, char* filename);
int SavePGM(char* filename, int w, int h, void* savebuf);
int vglSavePGM(VglImage* img, char* filename);
IplImage* LoadPGM(char* filename);
VglImage* vglLoadPGM(char* filename);



void vglDistTransformCross3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglDistTransformSq3(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglGetLevelDistTransform5(VglImage* src, VglImage* dst, VglImage* buf, VglImage* buf2, int times = 1);
void vglThinBernard(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglThinChin(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglBaricenterVga(VglImage* src, double* x_avg = NULL, double* y_avg = NULL, double* pix_count = NULL);
void vglCloseSq3(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglOpenSq3 (VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglErodeSq3Sep(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglErodeSq5Sep(VglImage* src, VglImage* dst, VglImage* buf, int times = 1);
void vglCErodeCross3(VglImage* src, VglImage* mask, VglImage* dst, VglImage* buf, int times);
void vglGray2(VglImage*  src, VglImage*  dst, VglImage*  dst1);


void vglInOut_model(VglImage*  dst, VglImage*  dst1);

////////// Macros

#define CHECK_FRAMEBUFFER_STATUS() \
{ \
 /*printf("CHECK_FRAMENBUFFER_STATUS\n");*/ \
 GLenum status; \
 status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT); \
 switch(status) { \
 case GL_FRAMEBUFFER_COMPLETE_EXT: \
   break; \
 case GL_FRAMEBUFFER_UNSUPPORTED_EXT: \
   printf("framebuffer GL_FRAMEBUFFER_UNSUPPORTED_EXT\n"); \
    /* you gotta choose different formats */ \
   assert(0); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: \
   printf("framebuffer FRAMEBUFFER_MISSING_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: \
   printf("framebuffer FRAMEBUFFER_DIMENSIONS\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT: \
   printf("framebuffer INCOMPLETE_DUPLICATE_ATTACHMENT\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT: \
   printf("framebuffer INCOMPLETE_FORMATS\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_DRAW_BUFFER\n"); \
   break; \
 case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT: \
   printf("framebuffer INCOMPLETE_READ_BUFFER\n"); \
   break; \
 case GL_FRAMEBUFFER_BINDING_EXT: \
   printf("framebuffer BINDING_EXT\n"); \
   break; \
 case GL_FRAMEBUFFER_STATUS_ERROR_EXT: \
   printf("framebuffer STATUS_ERROR\n"); \
   break; \
 default: \
   printf("programming error; will fail on all hardware\n"); \
   assert(0); \
 } \
} \

#define ERRCHECK() \
{ \
  static GLenum errCode; \
  const GLubyte *errString; \
  if ((errCode = glGetError()) != GL_NO_ERROR) { \
    /*errString = gluErrorString(errCode);*/     \
    errString = glGetString(errCode); \
    printf ("OpenGL Error %x: %s at %s:%d\n", errCode, errString,  __FILE__,__LINE__); \
	exit(1); \
  } \
} \
 


#endif
