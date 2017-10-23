#include "render.hpp"

void updateCamera(glm::mat4& V)
{
  V = glm::lookAt(
								glm::vec3(0,0,100), // Camera is at (0,0,100), in World Space
								glm::vec3(0,0,0), // and looks at the origin
								glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
						   );
}

void reshape(int w, int h, glm::mat4& P, float near)
{
  P = glm::frustum(-1.0f, 1.0f, ((float)h/(float)w), -((float)h/(float)w),near,1000.0f);
}

std::vector< unsigned char >  GetFrame(unsigned int w,unsigned int h)
{
  glReadBuffer(GL_COLOR_ATTACHMENT0);//Render Result
  std::vector< unsigned char > buf( w * h * 3 );
  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
  glReadPixels( 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, &buf[0] );
  // Mat result;
  // result.rows=h;
  // result.cols=w;
  // result.data=buf.data();
  // return result;
  return buf;
}

class FrameBuffer;

void FrameBuffer::CHECK_FRAMEBUFFER_STATUS()
{
  GLenum status;
  status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
  switch(status)
  {
    case GL_FRAMEBUFFER_COMPLETE:
      break;

    case GL_FRAMEBUFFER_UNSUPPORTED:
      /* choose different formats */
      break;

    default:
    {
      /* programming error; will fail on all hardware */
      cout << "Framebuffer Error" << endl;
      exit(-1);
    }
  }
}

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height)
{
  // GLuint Tex;
  // GLuint Tex2;
  // GLuint Depth;
  glGenFramebuffers(1, &FBO);
  glGenTextures(1, &Tex);
  glGenTextures(1, &Tex2);
  glGenRenderbuffers(1, &Depth);

  glBindFramebuffer(GL_FRAMEBUFFER, FBO);
  //tex1
  glBindTexture(GL_TEXTURE_2D, Tex);
  glTexImage2D(   GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  width, height,
                  0,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  //tex2
  glBindTexture(GL_TEXTURE_2D, Tex2);
  glTexImage2D(   GL_TEXTURE_2D,
                  0,
                  GL_RGB32F,
                  width, height,
                  0,
                  GL_RGB,
                  GL_FLOAT,
                  NULL);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Tex, 0);//mipmap 0
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, Tex2, 0);//mipmap 0

  glBindRenderbuffer(GL_RENDERBUFFER, Depth);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
  glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, Depth);

  // Set the list of draw buffers.
  GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, DrawBuffers); // "2" is the size of DrawBuffers

  FrameBuffer::CHECK_FRAMEBUFFER_STATUS();
}

FrameBuffer::~FrameBuffer()
{
  glDeleteTextures(1, &Tex);
  glDeleteTextures(1, &Tex2);
}

class ScreenQuad;
ScreenQuad::ScreenQuad(void)
{
  float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
      // positions   // texCoords
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f, 0.0f,

      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f, 1.0f
  };
  // screen quad VAO
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

ScreenQuad::~ScreenQuad(void)
{
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
}

void mylaplacian(Mat& src,unsigned int w,unsigned int h)
{
  Mat src_gray;
  Mat dst;


  int kernel_size = 3;
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  cvtColor( src, src_gray, CV_BGR2GRAY );
  Laplacian( src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
  convertScaleAbs( dst, dst );
  cvtColor(dst, src, cv::COLOR_GRAY2BGR);
}

void render(
        FrameBuffer & myFrameBuffer,
        Shader & texShader,
        Shader & simpleShader,
        Shader & screenShader,
        Model & ourModel,
        glm::mat4 &M,
        glm::mat4 &V,
        glm::mat4 &P,
        int &width,
        int &height,
        Mat &image,
        ScreenQuad &squad
      )
{
  //****************************************************************************
  // webcam image
  glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // glClear(GL_COLOR_BUFFER_BIT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  screenShader.Use();
  glBindVertexArray(squad.quadVAO);
  // glDisable(GL_DEPTH_TEST);
  glBindTexture(GL_TEXTURE_2D, myFrameBuffer.Tex2);

  glTexImage2D(   GL_TEXTURE_2D,
                  0,
                  GL_RGB,
                  width, height,
                  0,
                  GL_RGB,
                  GL_UNSIGNED_BYTE,
                  static_cast<void*>(image.data ));
  glGenerateMipmap(GL_TEXTURE_2D);

  // Parameters
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );//debug wireframe
  glDrawArrays(GL_TRIANGLES, 0, 6);

  //****************************************************************************
  // render obj
  // glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
  // cout<< "fbo="<<myFrameBuffer.FBO<<endl;
  glBindFramebuffer(GL_FRAMEBUFFER, myFrameBuffer.FBO); // back to default
  // glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // glClear(GL_DEPTH_BUFFER_BIT);
  simpleShader.Use();

  simpleShader.BindMatrices(&M,&V,&P);

  ourModel.Draw(simpleShader);

  // texShader.Use();
  //
  // texShader.BindMatrices(&M,&V,&P);
  //
  // ourModel.Draw(texShader);
  // glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default

  //****************************************************************************
}
