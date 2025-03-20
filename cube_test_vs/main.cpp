#include <iostream>
#include <sstream>
#include <fstream>

#include <RoxApp/RoxApp.h>
#include <RoxRender/RoxVBO.h>
#include <RoxRender/RoxShader.h>
#include <RoxRender/RoxRender.h>
#include <RoxLogger/RoxLogger.h>
#include <RoxInput/RoxInput.h>
#include <RoxScene/location.h>
#include <RoxScene/camera.h>
#include <RoxScene/mesh.h>
#include <RoxSystem/RoxSystem.h>
#include <RoxSystem/RoxShadersCacheProvider.h>
#include <RoxRender/RoxRenderOpengl.h>
#include <RoxRender/RoxStatistics.h>

class testCube : public RoxApp::RoxApp
{
private:
	bool onSplash() override
	{
		RoxLogger::log() << "Splash\n";
		RoxLogger::log() << "This is just a number\n";

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


	void testFileReading(const char* path)
	{
		FILE* file = nullptr;
		fopen_s(&file, path, "rb");

		if (!file)
		{
			std::cerr << "Failed to open file: " << path << std::endl;
			return;
		}

		fseek(file, 0, SEEK_END);
		size_t fileSize = ftell(file);
		fseek(file, 0, SEEK_SET);

		std::cout << "File size: " << fileSize << " bytes." << std::endl;

		fclose(file);
	}

	void onInit() override
	{
		RoxLogger::log() << "Init\n";

		RoxRender::setClearColor(0.5f, 0.5f, 0.5f, 1.0f);

		RoxRender::setClearDepth(1.0f);
		RoxRender::DepthTest::enable(RoxRender::DepthTest::LESS);

		RoxScene::mesh::register_load_function(RoxScene::mesh::load_nms);

		//testFileReading("D:/Dev/rox-engine-build/x64/Debug/new.nms");


		//bool boolValue = m_mesh.load("D:/Dev/rox-engine-build/x64/Debug/new.nms");
		//if (!boolValue)
		//{
		//	// Handle loading error
		//	std::cerr << "Failed to load NMS mesh file." << std::endl;
		//	return;
		//}

		float vertices[] =
		{
			-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f
		};

		unsigned short indices[] =
		{
			0,2,1, 1,2,3, // -x
			4,5,6, 5,7,6, // +x
			0,1,5, 0,5,4, // -y
			2,6,7, 2,7,3, // +y
			0,4,6, 0,6,2, // -z
			1,3,7, 1,7,5, // +z
		};

		// Total size of each vertex: 6 floats (3 for position + 3 for color)
		m_vbo.setVertexData(vertices, sizeof(float) * 6, 8);

		// Position attribute: starts at offset 0, 3 components (x, y, z)
		m_vbo.setVertices(sizeof(float)* 0, 3);

		// Color attribute: starts after the position data (offset of 3 * sizeof(float)), 3 components (r, g, b)
		m_vbo.setColors(sizeof(float) * 3, 3);

		// Set the index data
		m_vbo.setIndexData(indices, RoxRender::RoxVBO::INDEX_2D,sizeof(indices) / sizeof(unsigned short));

		

		if (!getShaders("shaders/v_shader.txt", "shaders/f_shader.txt")) {
			std::cout << "Failed to load shaders" << std::endl;
			return;
		}

		RoxRender::RoxCompiledShader shader;
		RoxSystem::RoxShaderCacheProvider shader_cache_provider;

		shader_cache_provider.setLoadPath(RoxSystem::getAppPath());
		shader_cache_provider.setSavePath(RoxSystem::getAppPath());

		// Combined Both Shaders Text for saving
		size_t vs_length = strlen(m_vertex_code.c_str());
		size_t ft_length = strlen(m_fragment_code.c_str());
		size_t total_length = vs_length + ft_length + 1; // +1 for the null terminator
		
		char* combined = new char[total_length];
		strcpy_s(combined, total_length, m_vertex_code.c_str());
		strcat_s(combined, total_length, m_fragment_code.c_str());


		bool shader_exist = shader_cache_provider.get(combined, shader);
		std::cout << "Shader Exist: " << (shader_exist ? "true" : "false") << "\n";
		
		if (!shader_exist)
		{
			RoxRender::RoxShader::setBinaryShaderCachingEnabled(true);
			m_shader.addProgram(RoxRender::RoxShader::VERTEX, m_vertex_code.c_str());
			m_shader.addProgram(RoxRender::RoxShader::PIXEL, m_fragment_code.c_str());

			
			if (m_shader.getProgramBinaryShader(shader))
			{
				std::cout << "Saving binary data to disk\n";
				shader_cache_provider.set(combined, shader);
			}
		}
		else
		{
			if (shader_cache_provider.get(combined, shader))
			{
				std::cout << "Load binary data to disk\n";
				if (!m_shader.setProgramBinaryShader(m_vertex_code.c_str(), m_fragment_code.c_str(), shader))
					std::cout << "Error loading binary data";
			}
			
		}

		//m_shader.setUniform(0, 1.0f, 0.0f, 0.0f, 1.0f);
		int uniform_count = m_shader.getUniformsCount();
		//std::cout << "Uniform count: " << uniform_count << std::endl;
		if (uniform_count > 0)
			for (int i = 0; i < uniform_count; ++i)
			{
				std::cout << "Uniform name: " << m_shader.getUniformName(i) << std::endl;
			}
		//m_shader.setUniform(0, 1.0f, 0.0f, 0.0f, 1.0f);

		RoxRender::Rectangle viewportInfo = RoxRender::getViewport();
		std::cout << "x:" << viewportInfo.x << " y: " << viewportInfo.y << " width: " << viewportInfo.width << " height: " << viewportInfo.height << "\n";
		std::cout << "App Path: " << RoxSystem::getAppPath()<< std::endl;
		std::cout << "User Path: " << RoxSystem::getUserPath() << std::endl;
		std::cout << "Statistics state: " << RoxRender::Statistics::enabled() << "\n";
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
		
		m_camera.set_pos(m_move_x, m_move_y, m_move_z);
		m_camera.set_rot(m_rotate_x, m_rotate_y, m_rotate_z);
		//RoxRender::setModelviewMatrix(m_camera.get_view_matrix());

		//RoxRender::setModelViewMatrix(mv);
		RoxRender::setModelViewMatrix(mv);

		//RoxRender::setCamera(m_camera.get_view_matrix());

		m_shader.bind();
		m_vbo.bind();
		//m_vbo.bindVerts();
		//m_vbo.bindIndices();
		m_vbo.draw();
		m_vbo.unbind();
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

		if ((key == ::RoxInput::KEY_BACK || key == ::RoxInput::KEY_ESCAPE) && !pressed)
			finish();

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


	RoxRender::RoxVBO m_vbo;
	RoxRender::RoxShader m_shader;
	RoxRender::RoxShader m_shader_dif;
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