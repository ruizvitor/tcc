#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <fstream>
#include "GL/glew.h"
#include "GL/glut.h"

#include <wand/magick_wand.h>


using namespace std;

static string textFileRead(const char *fileName) 
{
	string fileString;
	string line;
	
	ifstream file(fileName,ios_base::in);

	if (file.is_open()) 
	{
		while (!file.eof()) 
		{
			getline(file, line);
		  	fileString.append(line);
			fileString.append("\n");
		}
		file.close();
	}
	else
		cout<<"Unable to open "<<fileName<<"\n";

    return fileString;
}

// this function creates Shader Program which consists of both, a vertex and a fragment shader

void createProgram_VF(const char *VSfile, const char *FSfile, GLuint *handle, bool forceTextShader = true)
{
	
	GLint compiled;
	char infoLog[4096];
	int infoLogLength;

	
	std::string binaryFileName(VSfile);
	binaryFileName = binaryFileName.substr(0, binaryFileName.find("_")).append(".bin");
	std::cout<<binaryFileName<<std::endl;
	
	
	 *handle = glCreateProgram();
	
	char* binary;
	GLint formats = 0; 
	int len = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint* binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);
	
	 std::ifstream inFile(binaryFileName.c_str(), std::ofstream::binary );
	 
	 
	 if(inFile.is_open() && ! inFile.fail() && !forceTextShader)
	 {
		 inFile.seekg(0, inFile.end);
		 len = inFile.tellg();
		   
		 inFile.seekg(0);
		std::cout<<"Use Binary File: Goodbye 1+chiffre  Size: "<<len<<std::endl;

		binary = new char[len]; 

		inFile.read(binary, len);
		
		//for(int i=0; i< len;i++)
		//	std::cout <<binary[i];
			
		std::cout <<std::endl;
		
		inFile.close();
		 
		
		glProgramBinary(*handle, binaryFormats[0], binary, len);
		 
		 
		 
		delete [] binary;
		 
		//glLinkProgram(*handle); 
		GLint success;
		glGetProgramiv(*handle, GL_LINK_STATUS, &success);
		
		if (success)
		{
			std::cout  <<  "Binary Loaded Successfully" << std::endl;
			return ; 
		}
		

		glGetProgramInfoLog(*handle,  sizeof(infoLog), &infoLogLength, infoLog);
		if(infoLogLength > 0)
		{
			printf("LinkShader: \n %s\n", infoLog); 
		} 
		
		return;
	}
	
	string codeVS = textFileRead(VSfile);
	const char *VshaderCode = codeVS.c_str();

	string codeFS = textFileRead(FSfile);
	const char *FshaderCode = codeFS.c_str();


	//compile vertex shader:
	GLuint Vshader= glCreateShader(GL_VERTEX_SHADER);



	glShaderSource(Vshader,1,&VshaderCode,0);
	glCompileShader(Vshader);
	glGetShaderiv(Vshader,GL_COMPILE_STATUS, &compiled);
	if ( !compiled) 
	{
		// Print out the info log
		glGetShaderInfoLog(Vshader, sizeof(infoLog), &infoLogLength, infoLog);
		if(infoLogLength > 0)
		{
			printf("CompileShader() infoLog for Vertex Shader %s \n%s\n",VSfile, infoLog);
			exit(1);
		}
	}

	//compile Fragment shader:
	GLuint Fshader= glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(Fshader,1,&FshaderCode,0);
	glCompileShader(Fshader);
	glGetShaderiv(Fshader,GL_COMPILE_STATUS, &compiled);
	if ( !compiled) 
	{
		// Print out the info log
		glGetShaderInfoLog(Fshader, sizeof(infoLog), &infoLogLength, infoLog);
		if(infoLogLength > 0)
		{
			printf("CompileShader() infoLog for Fragment Shader %s\n%s\n", FSfile, infoLog);
			exit(1);
		}
	}


	glAttachShader(*handle, Vshader);
	glAttachShader(*handle, Fshader);
	glDeleteShader(Vshader);
	glDeleteShader(Fshader);
	glLinkProgram(*handle);

	glGetProgramInfoLog(*handle,  sizeof(infoLog), &infoLogLength, infoLog);
	if(infoLogLength > 0)
	{
		printf("LinkShader() \n%s\n",  infoLog);			
	} 

	glProgramParameteri(*handle, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE);



	std::cout << "Store Shader Binary"<<std::endl; 

 
	glGetProgramiv(*handle, GL_PROGRAM_BINARY_LENGTH, &len);
	binary = new char[len]; 
	glGetProgramBinary(*handle, len, NULL, (GLenum*)binaryFormats, binary);


	std::ofstream outFile;
	outFile.open(binaryFileName.c_str(), std::ofstream::binary  );
	outFile.write(binary,len);
	outFile.close();

	delete [] binary;


}

// this function creates Shader Program which consists only of a vertex shader

void createProgram_V(const char *VSfile, GLuint *handle) {

GLint compiled;
char infoLog[4096];
int infoLogLength;
string code = textFileRead(VSfile);

const char *VshaderCode = code.c_str();

//compile vertex shader:
GLuint Vshader= glCreateShader(GL_VERTEX_SHADER);
glShaderSource(Vshader,1,&VshaderCode,0);
glCompileShader(Vshader);
glGetShaderiv(Vshader,GL_COMPILE_STATUS, &compiled);
if ( !compiled) {
// Print out the info log
glGetShaderInfoLog(Vshader, sizeof(infoLog), &infoLogLength, infoLog);
if(infoLogLength > 0)
{
printf("CompileShader() infoLog %s \n%s\n",VSfile, infoLog);
exit(1);
}
}

*handle = glCreateProgram();
glAttachShader(*handle, Vshader);
glDeleteShader(Vshader);

glLinkProgram(*handle);
}


class MyImage
{
public:


	MyImage()
	{ 

		pixels = NULL;
	}

	~MyImage()
	{
		 
		delete []pixels;
		pixels = NULL;
		 
	}
	

	unsigned int width;
	unsigned int height;
	unsigned char* pixels;

bool load(const char* fileName)
{

 

MagickWand *mw = NULL;

	MagickWandGenesis();

	mw = NewMagickWand();

	MagickReadImage(mw, fileName);
	

	width = MagickGetImageWidth(mw);
	height = MagickGetImageHeight(mw);

	pixels = new unsigned char[3*width*height];


  MagickGetImagePixels (mw, 0, 0, width, height, "RGB", CharPixel, pixels );
 
	if(mw) mw = DestroyMagickWand(mw);

	MagickWandTerminus();


return true;
}
};


