#include "util.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <png.h>
#include <string.h>
#include <sys/stat.h>

std::ostream& operator<<(std::ostream &strm, const vec3& v) {
    return strm << "<" << v.x << ", " << v.y << "," << v.z << ">";
}

void draw_capped_cylinder(const float r, const float h, const int slices, const int stacks) {
    GLUquadricObj *obj = gluNewQuadric();
    gluQuadricNormals(obj, GLU_SMOOTH);

    gluCylinder(obj, r, r, h, slices, stacks);

    // top cap
    glPushMatrix();
        glTranslatef(0, 0, h);
        gluDisk(obj, 0, r, slices, stacks);
    glPopMatrix();

    // bottom cap
    glPushMatrix();
        glRotatef(180, 1, 0, 0);
        gluDisk(obj, 0, r, slices, stacks);
    glPopMatrix();
}

void draw_cone(const float base, const float h, const int slices, const int stacks) {
    GLUquadricObj *obj = gluNewQuadric();
    gluQuadricNormals(obj, GLU_SMOOTH);

    gluCylinder(obj, base, 0, h, slices, stacks);
}

vec3 cross(vec3 a, vec3 b) {
    vec3 res = {
        (a.y * b.z) - (a.z * b.y),
        (a.z * b.x) - (a.x * b.z),
        (a.x * b.y) - (a.y * b.x)
    };
    return res;
}

vec3 normalize(vec3 v) {
    float l = v.len();
    vec3 norm = {
        v.x / l,
        v.y / l,
        v.z / l
    };
    return norm;
}

float dot(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dist(vec3 a, vec3 b) {
    return sqrt(
        pow(a.x - b.x, 2) +
        pow(a.y - b.y, 2) +
        pow(a.z - b.z, 2)
    );
}

unsigned int g_program_obj = 0;
unsigned int g_vertex_obj = 0;
unsigned int g_fragment_obj = 0;

void draw_text(const int x, const int y, const char *str) {

	const float scale = 0.2;

	glColor3f(1.0f, 1.0f, 0.0f);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, ORTHO_SIZE, 0, ORTHO_SIZE);

		glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
		glPushMatrix();
			glTranslatef(x, y, 0.0f);
			glScalef(scale, scale, 1.0f);
            glColor3f(1, 1, 1);
    		size_t len = strlen(str);
    		for (size_t i = 0; i < len; i++)
       			glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
		glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
}

void draw_centered(const int y, const char *str) {

    const float scale = 0.2;

    size_t len = strlen(str);
    int width = 0;
    for (size_t i = 0; i < len; i++)
        width += glutStrokeWidth(GLUT_STROKE_ROMAN, str[i]);
    width *= scale;
    draw_text((ORTHO_SIZE - width)/2, y, str);
}

void draw_raligned(const int x, const int y, const char *str) {
    
    const float scale = 0.2;

    size_t len = strlen(str);
    int width = 0;
    for (size_t i = 0; i < len; i++)
        width += glutStrokeWidth(GLUT_STROKE_ROMAN, str[i]);
    width *= scale;

    draw_text(x - width, y, str);
}

unsigned int load_and_bind_tex(const char *fname) {
    
    char *imgbuf = NULL;
    int width = 0;
    int height = 0;

    if (png_load(fname, &width, &height, &imgbuf) == 0) {
        fprintf(stderr, "Failed to read texture from %s\n", fname);
        exit(1);
    }

    unsigned int tex_handle = 0;
    glGenTextures(1, &tex_handle);

    glBindTexture(GL_TEXTURE_2D, tex_handle);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
            GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) imgbuf
    );

    glBindTexture(GL_TEXTURE_2D, 0);

    free(imgbuf);
    return tex_handle;
}

int png_load(const char* file_name, 
		     int* width, 
			 int* height, 
			 char** image_data_ptr) {

    png_byte header[8];

    FILE* fp = fopen(file_name, "rb");
    if (fp == 0) {
        fprintf(stderr, "erro: could not open PNG file %s\n", file_name);
        perror(file_name);
        return 0;
    }

    // read the header
    fread(header, 1, 8, fp);

    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr, "error: %s is not a PNG.\n", file_name);
        fclose(fp);
        return 0;
    }

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr, "error: png_create_read_struct returned 0.\n");
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        fclose(fp);
        return 0;
    }

    // create png info struct
    png_infop end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        fprintf(stderr, "error: png_create_info_struct returned 0.\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(fp);
        return 0;
    }

    // the code in this if statement gets called if libpng encounters an error
    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stderr, "error from libpng\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // init png reading
    png_init_io(png_ptr, fp);

    // let libpng know you already read the first 8 bytes
    png_set_sig_bytes(png_ptr, 8);

    // read all the info up to the image data
    png_read_info(png_ptr, info_ptr);

    // variables to pass to get info
    int bit_depth, color_type;
    png_uint_32 temp_width, temp_height;

    // get info about png
    png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
        NULL, NULL, NULL);

    if (width) { *width = temp_width; }
    if (height){ *height = temp_height; }

    // Update the png info struct.
    png_read_update_info(png_ptr, info_ptr);

    // Row size in bytes.
    int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

    // glTexImage2d requires rows to be 4-byte aligned
    rowbytes += 3 - ((rowbytes-1) % 4);

    // Allocate the image_data as a big block, to be given to opengl
    png_byte* image_data;
    image_data = (png_byte*)malloc(rowbytes * temp_height * sizeof(png_byte)+15);
    if (image_data == NULL) {
        fprintf(stderr, "error: could not allocate memory for PNG image data\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        fclose(fp);
        return 0;
    }

    // row_pointers is for pointing to image_data for reading the png with libpng
    png_bytep* row_pointers = (png_bytep*)malloc(temp_height * sizeof(png_bytep));
    if (row_pointers == NULL) {
        fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return 0;
    }

    // set the individual row_pointers to point at the correct offsets of image_data
    int i;
    for (i = 0; i < temp_height; i++) {
        row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
    }

    // read the png into image_data through row_pointers
    png_read_image(png_ptr, row_pointers);

    // clean up
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    //free(image_data);
	*image_data_ptr = (char*)image_data; // return data pointer

    free(row_pointers);
    fclose(fp);

	fprintf(stderr, "\t texture image size is %d x %d\n", *width, *height);

	return 1;
}

char* read_shader_source(const char* filename) {
	char* buffer = NULL;

	FILE* fp = fopen(filename, "r");
	if (fp!=NULL)
	{
		struct stat status_buf;
		stat(filename, &status_buf); // find out how big it is

		buffer = new char[status_buf.st_size+1];
		// read in the file
		fread(buffer, 1, status_buf.st_size, fp); 
		buffer[status_buf.st_size] = '\0'; // null terminate it

		fclose(fp);
	}
	else
	{
		fprintf(stderr, "Failed to open shader file %s for reading\n", filename);
		exit(1);
	}

	return buffer;
}

void print_shader_info_log(unsigned int shader_obj) {
	int len = 0;
	glGetShaderiv(shader_obj, GL_INFO_LOG_LENGTH, &len);
	if (len>1)
	{
		char* log = new char[len];
		int s_len = 0;
		glGetShaderInfoLog(shader_obj, len, &s_len, log);
		fprintf(stderr, "%s", log);
		delete[] log;
	}
}

void print_program_info_log(unsigned int shader_obj) {
	int len = 0;
	glGetProgramiv(shader_obj, GL_INFO_LOG_LENGTH, &len);
	if (len>1)
	{
		char* log = new char[len];
		int s_len = 0;
		glGetProgramInfoLog(shader_obj, len, &s_len, log);
		fprintf(stderr, "%s", log);
		delete[] log;
	}
}

void create_and_compile_shaders(
		const char* vertex_shader_filename,
		const char* fragment_shader_filename
	) {

	fprintf(stderr, "create_and_compile shaders called: vertex shader = %s, fragment shader = %s\n",
						vertex_shader_filename, fragment_shader_filename );
	fprintf(stderr, "Shading Language version %s\n", 
			glGetString(GL_SHADING_LANGUAGE_VERSION));

	// read the shader source files
	char* vertex_source = read_shader_source(vertex_shader_filename);
	char* frag_source = read_shader_source(fragment_shader_filename);

	if (vertex_source && frag_source)
	{
		// create shader program object and shader objects
		g_vertex_obj = glCreateShader(GL_VERTEX_SHADER);
		g_fragment_obj = glCreateShader(GL_FRAGMENT_SHADER);


		// put sources into shader objects
		glShaderSource(g_vertex_obj, 1, (const char**)&vertex_source, NULL);
		glShaderSource(g_fragment_obj, 1, (const char**)&frag_source, NULL);
		
		// attempt to compile and link
		glCompileShader(g_vertex_obj);

		// check if it has compile ok
		int compiled = 0;
		glGetShaderiv(g_vertex_obj, GL_COMPILE_STATUS, &compiled);
		if (compiled==0)
		{
			// failed to compile vertex shader
			fprintf(stderr, "Failed to compile vertex shader\n");
			print_shader_info_log(g_vertex_obj);

			exit(1);
		}

		glCompileShader(g_fragment_obj);
		glGetShaderiv(g_fragment_obj, GL_COMPILE_STATUS, &compiled);
		if (compiled==0)
		{
			// failed to compile fragment shader
			fprintf(stderr, "Failed to compile fragment shader\n");
			print_shader_info_log(g_fragment_obj);
			exit(1);
		}

		// attach shaders to the program object
		g_program_obj = glCreateProgram();
		glAttachShader(g_program_obj, g_vertex_obj);
		glAttachShader(g_program_obj, g_fragment_obj);

		// try to link the program
		glLinkProgram(g_program_obj);

		int linked = 0;
		glGetProgramiv(g_program_obj, GL_LINK_STATUS, &linked);
		if (linked==0)
		{
			// failed to link program 
			fprintf(stderr, "Failed to link shader program\n");
			print_program_info_log(g_program_obj);
			exit(1);
		}

		delete[] vertex_source;
		delete[] frag_source;
	}

	if (glIsProgram(g_program_obj))
		glUseProgram(g_program_obj);
}
