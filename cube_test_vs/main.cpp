#include <iostream>
#include <sstream>
#include <fstream>


#include <RoxApp/RoxApp.h>


#include <RoxSystem/RoxSystem.h>
#include <RoxSystem/RoxShadersCacheProvider.h>

#include <RoxLogger/RoxLogger.h>
#include <RoxInput/RoxInput.h>

#include <RoxScene/location.h>
#include <RoxScene/camera.h>
#include <RoxScene/mesh.h>
#include <RoxScene/texture.h>

#include <RoxRender/RoxVBO.h>
#include <RoxRender/RoxTexture.h>
#include <RoxRender/RoxShader.h>
#include <RoxRender/RoxRender.h>
#include <RoxRender/RoxStatistics.h>
#include <RoxRender/RoxDebugDraw.h>

#include <RoxFormats/RoxTruevisionGraphicsAdapter.h>
#include <RoxFormats/RoxDirectDrawSurface.h>
#include <RoxFormats/RoxMesh.h>


class testCube : public RoxApp::RoxApp
{
private:
	bool onSplash() override
	{
		//RoxLogger::log() << "Splash\n";
		//RoxLogger::log() << "This is just a number\n";

		RoxRender::setClearColor(1.0f, 0.5f, 0, 1.0f);
		RoxRender::clear(true, true);

		return true;
	}

	bool getShaders(const char* vertex_path, const char* fragment_path) {

		std::ifstream vertex_file;
		std::ifstream fragment_file;

		// Ensure ifstream objects can throw exceptions
		vertex_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragment_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			// Open files
			vertex_file.open(vertex_path);
			fragment_file.open(fragment_path);
			std::stringstream vertex_stream, fragment_stream;

			// Read file's buffer contents into streams
			vertex_stream << vertex_file.rdbuf();
			fragment_stream << fragment_file.rdbuf();

			// Close file handlers
			vertex_file.close();
			fragment_file.close();

			// Convert stream into string
			m_vertex_code = vertex_stream.str();
			m_fragment_code = fragment_stream.str();
		}
		catch (std::ifstream::failure err) {
			return false;
		}

		return true;
	}


	bool testFileReading(const char* path)
	{
		FILE* file = nullptr;
		fopen_s(&file, path, "rb");

		if (!file)
		{
			std::cerr << "Failed to open file: " << path << std::endl;
			return false;
		}

		fseek(file, 0, SEEK_END);
		size_t file_size = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::cout << "File size: " << file_size << " bytes." << std::endl;

		// Read file into buffer
		std::vector<char> buffer(file_size);
		size_t read_size = fread(buffer.data(), 1, file_size, file);
		fclose(file);

		if (read_size != file_size) {
			std::cerr << "Error: Read only " << read_size << " of " << file_size << " bytes" << std::endl;
			return false;
		}

		// Check file signature
		if (file_size < 8 || memcmp(buffer.data(), "nya mesh", 8) != 0) {
			std::cerr << "Not a valid NMS file (missing 'nya mesh' signature)" << std::endl;
			return false;
		}

		// Parse the mesh using readChunksInfo
		RoxFormats::nms mesh;
		bool success = mesh.read_chunks_info(buffer.data(), file_size);

		if (!success)
		{
			std::cerr << "Failed to parse mesh" << std::endl;
			return false;
		}

		// Print detailed mesh info for debugging
		std::cout << "Mesh loaded successfully!" << std::endl;
		std::cout << "Version: " << mesh.version << std::endl;
		std::cout << "Chunks: " << mesh.chunks.size() << std::endl;

		for (size_t i = 0; i < mesh.chunks.size(); i++) {
			auto& chunk = mesh.chunks[i];
			std::cout << "Chunk " << i << ":" << std::endl;
			std::cout << "  Type: " << chunk.type << std::endl;
			std::cout << "  Size: " << chunk.size << std::endl;
		}

		return true;
	}

	void onInit() override
	{
		RoxLogger::log() << "Init\n";

		RoxRender::setClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		RoxRender::setClearDepth(1.0f);
		RoxRender::DepthTest::enable(RoxRender::DepthTest::LESS);

		RoxResources::setResourcesPath("D:/Dev/p2p-game-sample/cube_test_vs/resources/");
		
		RoxScene::mesh::register_load_function(RoxScene::mesh::load_nms);
		//RoxScene::mesh::set_resources_prefix("D:/Dev/p2p-game-sample/cube_test_vs/resources/");
		if (!m_mesh.load("sniper.nms"))
		{
			// Handle loading error
			std::cerr << "Failed to load NMS mesh file." << std::endl;
			return;
		}


		// Load .nms file to the RoxMesh from the Format Module
		//if (!testFileReading("resources/sniper.nms")) {
		//	std::cerr << "Falied to load .nms\n";
		//	return;
		//}

		//RoxScene::mesh::register_load_function(RoxScene::mesh::load_nms);
		//RoxScene::mesh::set_resources_prefix("resources/");
		//m_mesh.load("cube.nms");
		//if (!m_mesh.internal().get_shared_data().isValid())
		//{
		//	// Handle loading error
		//	std::cerr << "Failed to load NMS mesh file." << std::endl;
		//	return;
		//}


		//RoxFormats::nms mesh_data;
		//std::ifstream file("resources/cube_2.nms", std::ios::binary | std::ios::ate);
		//if (!file) { 
		//	/* handle error */ 
		//	std::cout << "Error reading file\n";
		//}

		//std::streamsize size = file.tellg();
		//file.seekg(0, std::ios::beg);
		//std::vector<char> buffer(size);
		//if (!file.read(buffer.data(), size)) { 
		//	/* handle error */
		//	std::cout << "Error reading data\n";
		//}
		//if (!mesh_data.readChunksInfo(buffer.data(), buffer.size())) { 
		//	/* handle error */ 
		//	std::cout << "Error Reading Chunk Info\n";
		//}
		// Now use mesh_data to create your mesh

		//RoxScene::mesh myMesh("resources/cube.nms");
		//if (!myMesh.internal().get_shared_data().isValid()) {
		//	// Handle error
		//	std::cerr << "Failed to load NMS mesh file." << std::endl;
		//}

		//std::cout << "Mesh Name" << myMesh.get_name() << "\n";

		const float vertices_Rec[36] = {
			// Position			 // Color				 // Texture
			-1, -1, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 0. bottom left
			1, -1,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, // 1. bottom right
			-1, 1,  0.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, // 2. top left
			1, 1,   0.0f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // 3. top right
		};


		unsigned short indices_Rec[6] = {
		0, 1, 2,
		1, 3, 2,
		};

		float vertices[] =
		{
			// Position           // Colors           // Normals          // Texture 
			// Front face (+z)
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,
													  					 
			// Back face (-z)						  					 
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
													  					 
			// Left face (-x)						  					 
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
													  					 
			// Right face (+x)						  					 
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
													  					 
			 // Bottom face (-y)					  					 
			 -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
			  0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
			  0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
			 -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
													  					 
			 // Top face (+y)						  					 
			 -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
			  0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
			  0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
			 -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f
		};

		// Update the indices to match the new vertices
		unsigned short indices[] =
		{
			// Front face
			0, 1, 2,
			0, 2, 3,
			// Back face
			4, 5, 6,
			4, 6, 7,
			// Left face
			8, 9, 10,
			8, 10, 11,
			// Right face
			12, 13, 14,
			12, 14, 15,
			// Bottom face
			16, 17, 18,
			16, 18, 19,
			// Top face
			20, 21, 22,
			20, 22, 23
		};

		// Update the vertex stride - now each vertex has 11 floats (3 position + 3 color + 3 normal + 2 texture)
		m_vbo.setVertexData(vertices, sizeof(float) * 11, 24);

		// Position attribute
		m_vbo.setVertices(sizeof(float) * 0, 3);

		// Color attribute 
		m_vbo.setColors(sizeof(float) * 3, 3);

		// Normal attribute (add this)
		m_vbo.setNormals(sizeof(float) * 6, RoxRender::RoxVBO::FLOAT_32);

		// Texture attribute
		m_vbo.setTexCoord(0, sizeof(float) * 9, 2);

		// Set the index data
		m_vbo.setIndexData(indices, RoxRender::RoxVBO::INDEX_2D, sizeof(indices) / sizeof(unsigned short));


		//if(false)
			if (!getShaders("shaders/v_shader.txt", "shaders/f_shader.txt")) {
				std::cout << "Failed to load shaders" << std::endl;
				return;
			}

		///////////// -- Load Shaders -- ////////////////
		RoxRender::RoxShader::setBinaryShaderCachingEnabled(false);
		m_shader.addProgram(RoxRender::RoxShader::VERTEX, m_vertex_code.c_str());
		if(!m_shader.addProgram(RoxRender::RoxShader::PIXEL, m_fragment_code.c_str()))
		{
			std::cout << "Error Compiling Shaders Shader\n";
			return;
		}
		////////////////////////////

	}

	void onFrame(unsigned int dt) override
	{
		RoxRender::clear(true, true);

		m_rot += dt * 0.05f;
		if (m_rot > 360)
			m_rot = 0;

		RoxMath::Matrix4 mv;
		mv.translate(0, 0, -2.0f);
		mv.rotate(30.0f, 1.0f, 0.0f, 0.0f);
		mv.rotate(m_rot, 0.0f, 1.0f, 0.0f);
		
		m_camera.set_pos(mv.get_pos());
		m_camera.set_rot(mv.get_rot());
		
		RoxRender::setModelViewMatrix(mv);


		m_shader.bind();
		m_mesh.draw();
		m_shader.unbind();

		//m_mesh.draw();

		static unsigned int fps_counter = 0, fps_update_timer = 0;
		++fps_counter;
		fps_update_timer += dt;
		if (fps_update_timer > 1000)
		{
			std::ostringstream os;
			os << "Rox Engine Demo " << fps_counter << " fps";
			std::string str = os.str();
			setTitle(str.c_str());

			fps_update_timer %= 1000;
			fps_counter = 0;
		}
	}

	void onResize(unsigned int w, unsigned int h) override
	{
		RoxLogger::log() << "on_resize " << w << " " << h << "\n";

		if (!w || !h)
			return;

		RoxMath::Matrix4 proj;
		proj.perspective(70.0f, float(w) / h, 0.01f, 100.0f);
		RoxRender::setProjectionMatrix(proj);
	}

	void onFree() override
	{
		RoxLogger::log() << "on_free\n";

		m_vbo.release();
		m_shader.release();
	}

	void onKeyDown(unsigned int key, bool pressed) override
	{
		RoxLogger::log() << "key " << key << " " << pressed << "\n";

		if ((key == ::RoxInput::KEY_BACK || key == ::RoxInput::KEY_ESCAPE) && pressed) {
			finish();
		}

		if(key == ::RoxInput::KEY_W && pressed)
		{
			RoxLogger::log() << "A pressed\n";
			m_move_z += 0.1f;
		}

		if (key == ::RoxInput::KEY_S && pressed)
		{
			RoxLogger::log() << "D pressed\n";
			m_move_z -= 0.1f;
		}

		if(key == ::RoxInput::KEY_SHIFT && pressed)
			if ((key == ::RoxInput::KEY_W) && pressed)
			{
				RoxLogger::log() << "Shift + A pressed\n";
				m_move_z += 10.0f;
			}
		
		if (key == ::RoxInput::KEY_SHIFT && pressed)
			if (key == ::RoxInput::KEY_S && key == ::RoxInput::KEY_SHIFT && pressed)
			{
				RoxLogger::log() << "Shift + D pressed\n";
				m_move_z -= 10.0f;
			}


		if (key == ::RoxInput::KEY_D && pressed)
		{
			RoxLogger::log() << "D pressed\n";
			m_move_x -= 0.1f; 
		}

		if (key == ::RoxInput::KEY_A && pressed)
		{
			RoxLogger::log() << "A pressed\n";
			m_move_x += 0.1f;
		}

		if (key == ::RoxInput::KEY_Q && pressed)
		{
			RoxLogger::log() << "Q pressed\n";
			m_move_y += 0.1f;
		}

		if (key == ::RoxInput::KEY_E && pressed)
		{
			RoxLogger::log() << "E pressed\n";
			m_move_y -= 0.1f;
		}


		
	}

	void onMouseScroll(int dx, int dy) override
	{
		m_rotate_x += dy * 0.1f;
		m_move_y += dy * 0.1f;

		RoxLogger::log() << "mouse scroll dx: " << dx << " dy: " << dy << "\n";
		std::cout << "mouse scroll dx: " << dx << " dy: " << dy << "\n";
	}

	void onMouseMove(int x, int y) override
	{
		//RoxLogger::log() << "mouse move X: " << x << " Y: " << y << "\n";
	}


	void onMouseButton(::RoxInput::MOUSE_BOTTON button, bool pressed) override
	{
		//RoxLogger::log() << "mouse button " << button << " | " << pressed << "\n";
		if (button == ::RoxInput::MOUSE_BOTTON::MOUSE_LEFT && pressed)
		{
			
			
		}

		if (button == ::RoxInput::MOUSE_BOTTON::MOUSE_RIGHT && pressed)
		{
			RoxLogger::log() << "Right mouse button pressed\n";
		}

		if (button == ::RoxInput::MOUSE_BOTTON::MOUSE_MIDDLE && pressed)
		{
			RoxLogger::log() << "Middle mouse button pressed\n";
		}
	}


public:
	testCube() : m_rot(0.0f) {}

private:
	// Camera Movment and Rotation keys
	float m_move_z = 2;
	float m_move_x = 0;
	float m_move_y = 0;
	float m_rotate_z = 0;
	float m_rotate_x = 0;
	float m_rotate_y = 0;


	RoxScene::camera m_camera;
	RoxScene::mesh m_mesh;

	RoxFormats::nms MyMesh;

	RoxRender::RoxVBO m_vbo;
	RoxRender::RoxDebugDraw m_debug_draw;
	RoxRender::RoxTexture m_texture;
	RoxRender::RoxShader m_shader;
	float m_rot;

	std::string m_vertex_code;
	std::string m_fragment_code;
};


int main(int argc, char** argv)
{
	testCube app;
	RoxLogger::log() << "App Created\n";
	//app.setTitle("Loading, please wait...");
	RoxLogger::log() << "Title " << app.getTitle() << "\n";
	
	app.startWindowed(100, 100, 640, 480, 0);
	RoxLogger::log() << "exit success\n";

	return 0;
}