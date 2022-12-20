#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengles2.h>
//#include <GLES2/gl2.h>

#include <stddef.h>
#include <stdint.h>
#include <math.h>

#include <cstdio>
#include <cstdlib>
#include <assert.h>

GLuint texture_load(const char* filename);
void texture_destroy(GLuint texName);

typedef struct Vertex_s {
	float x, y;	// position
	uint8_t r, g, b, a;
	float u, v;	// texCoord
} Vertex;

const unsigned int DISP_WIDTH = 1280;
const unsigned int DISP_HEIGHT = 720;

static const char * vertex_glsl =
	"attribute vec2 a_pos;					\n"
	"attribute vec4 a_color;				\n"
	"attribute vec2 a_texCoord;				\n"
	"varying vec4 color;					\n"
	"varying vec2 texCoord;					\n"
	"							            \n"
	"void main()						    \n"
	"{							            \n"
	"	gl_Position = vec4(a_pos, 0, 1);	\n"
	"	color = a_color;				    \n"
	"	texCoord = a_texCoord;				\n"
	"}							            \n"
;

static const char * fragment_glsl =
	"#if GL_ES						        \n"
	"precision lowp float;					\n"
	"#endif		                            \n"
	"varying vec4 color;					\n"
	"varying vec2 texCoord;					\n"
	"uniform sampler2D texSampler;			\n"
	"void main()						    \n"
	"{							            \n"
	"	gl_FragColor = texture2D(texSampler, texCoord); \n"
	"}          							\n"
;

static GLuint compile_shader(GLuint type, const char* glsl)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &glsl, NULL);
	glCompileShader(shader);

	GLint compiled;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		char message[4096];
		glGetShaderInfoLog(shader, sizeof(message), NULL, message);
		printf("Error compiling %s shader!\n%s", type == GL_VERTEX_SHADER ? "vertex" : "fragment", message);
		assert(0);
	}

	return shader;
}


int main(int argc, char* args[])
{
	// The Window
	SDL_Window* window = NULL;

	// The OpenGl Context
	SDL_GLContext context = NULL;

	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	// Request OpenGL ES 2.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Want double-buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// prueba
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	// Create the window

	window = SDL_CreateWindow("Demo - SDL2 + GLES2", 0, 0, DISP_WIDTH, DISP_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);

	if (!window)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could't create the main window.", NULL);
		return EXIT_FAILURE;
	}

	context = SDL_GL_CreateContext(window);

	if (!context)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Could't create the OpenGL context.", NULL);
		return EXIT_FAILURE;
	}

	SDL_GL_MakeCurrent(window, context);

	glViewport(0, 0, DISP_WIDTH, DISP_HEIGHT);


	// Load Texture
	GLuint texture_id = texture_load("crate1_diffuse.png");


	// Renderizar

	static int setup = 0;

	if (!setup) {
		setup = 1;

		GLuint vsh = compile_shader(GL_VERTEX_SHADER, vertex_glsl);
		GLuint fsh = compile_shader(GL_FRAGMENT_SHADER, fragment_glsl);

		GLuint program = glCreateProgram();
		glAttachShader(program, fsh);
		glAttachShader(program, vsh);
		glBindAttribLocation(program, 0, "a_pos");
		glBindAttribLocation(program, 1, "a_color");
		glBindAttribLocation(program, 2, "a_texCoord");
		glLinkProgram(program);
		glDeleteShader(fsh);
		glDeleteShader(vsh);

		GLint linked;
		glGetProgramiv(program , GL_LINK_STATUS, &linked);

		if (!linked) {
			char message[4096];
			glGetProgramInfoLog(program, sizeof(message), NULL, message);
			printf("Error linking shader!\n%s\n", message);
			assert(0);
		}

		glUseProgram(program);

		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, r));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindTexture(GL_TEXTURE_2D, texture_id);

		GLint texSamplerUniformLoc = glGetUniformLocation(program, "texSampler");
		if (texSamplerUniformLoc < 0) {
			SDL_Log("ERROR: No se pudo obtener la ubicacion de texSampler\n");
		}

		glUniform1i(texSamplerUniformLoc, 0);

		glClearColor(0.392156863f, 0.584313725f, 0.929411765f, 1.f);

		assert(glGetError() == GL_NO_ERROR);
	}

	Vertex vtx[] =
	{
		{.x = -0.5f, .y = -0.5, .r = 255, .u = 0.0f, .v = 0.0f},
		{.x = 0.5f, .y = -0.5f, .g = 255, .u = 1.0f, .v = 0.0f},
		{.x = 0.5f, .y = 0.5f, .b = 255, .u = 1.0f, .v = 1.0f},

		{.x = 0.5f, .y = 0.5f, .r = 255, .u = 1.0f, .v = 1.0f},
		{.x = -0.5f, .y = 0.5f, .r = 255, .u = 0.0f, .v = 1.0f},
		{.x = -0.5f, .y = -0.5f, .r = 255, .u = 0.0f, .v = 0.0f},
	};

	glClear(GL_COLOR_BUFFER_BIT);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtx), vtx, GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	assert(glGetError() == GL_NO_ERROR);

	// Update the window

	SDL_GL_SwapWindow(window);


	// Wait for the user to quit

	bool quit = false;

	while (!quit)
	{
		SDL_Event event;

		if (SDL_WaitEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}

			// Salir ante cualquier pulsacion de tecla!
	        	if (event.type == SDL_KEYDOWN) {
        	        	quit = true;
            		}
		}
	}


	return EXIT_SUCCESS;
}

GLuint texture_load(const char* filename)
{
    // Make sure the JPEG and PNG image loaders are present
    // (don't know what file type we'll get).

    int flags = IMG_INIT_JPG | IMG_INIT_PNG;

#if 1  // Windows: Los dlls de los formatos de imagenes de SDL_Image tienen que estar en la carpeta root
    if ((IMG_Init(flags) & flags) == 0)
    {
        // Failed
        SDL_Log("ERROR: Texture loading failed. Could't get JPEG and PNG loaders.\n");
        return 0;
    }
#endif

    // Load the image
    SDL_Surface* texSurf = IMG_Load(filename);

    if (!texSurf)
    {
        SDL_Log("Loading image %s failed with error: %s", filename, IMG_GetError());
        return 0;
    }

    // Determine the format
    // NOTE: Only supporting 24 and 32-bit images

    GLenum format;
    GLenum type = GL_UNSIGNED_BYTE;
    switch (texSurf->format->BytesPerPixel)
    {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        defualt:
        SDL_Log("Can't load image %s; it isn't a 24/32-bit image\n", filename);
        SDL_FreeSurface(texSurf);
        texSurf = NULL;
        return 0;
    }

    // Create the texture
    //	glActiveTexture(GL_TEXTURE0);
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);


    glTexImage2D(GL_TEXTURE_2D, 0, format, texSurf->w, texSurf->h, 0, format, type, texSurf->pixels);

    // Set up the filtering
    // NOTE: Failure to do this may result in no texture

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        // Failed
        glDeleteBuffers(1, &texture);
        texture = 0;
        SDL_FreeSurface(texSurf);
        texSurf = NULL;
        SDL_Log("Creating texture %s failed, code %u\n", filename, err);
        //		__debugbreak();
        return 0;
    }

    // Cleanup
    SDL_FreeSurface(texSurf);
    texSurf = NULL;

    return texture;
}

void texture_destroy(GLuint texName)
{
    glDeleteTextures(1, &texName);
}