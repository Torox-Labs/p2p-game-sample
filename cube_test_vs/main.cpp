#include <iostream>
#include <sstream>
#include <fstream>
#include <functional>


#include <RoxApp/RoxApp.h>

#include <RoxLogger/RoxLogger.h>
#include <RoxInput/RoxInput.h>

#include <RoxScene/location.h>
#include <RoxScene/camera.h>
#include <RoxScene/mesh.h>

#include <RoxRender/RoxVBO.h>
#include <RoxRender/RoxTexture.h>
#include <RoxRender/RoxShader.h>
#include <RoxRender/RoxRender.h>
#include <RoxRender/RoxDebugDraw.h>

#include <RoxFormats/RoxTruevisionGraphicsAdapter.h>
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

		bool load_using_mesh = true;

		if (load_using_mesh)
		{
			RoxResources::setResourcesPath("D:/Dev/p2p-game-sample/cube_test_vs/resources/");

			RoxScene::mesh::register_load_function(RoxScene::mesh::load_nms);
			if (!m_mesh.load("cube.nms"))
			{
				// Handle loading error
				std::cerr << "Failed to load NMS mesh file." << std::endl;
				return;
			}
		}else
			testFileReading("D:/Dev/p2p-game-sample/cube_test_vs/resources/cube.nms");

		std::cout << "============= Uniforms list ===============\n";

		int uniform_count = m_shader.getUniformsCount();
		std::cout << "Found " << uniform_count << " uniforms:" << std::endl;
		for (int i = 0; i < uniform_count; ++i) {
			std::string name = m_shader.getUniformName(i);
			int type = m_shader.getUniformType(i);
			std::cout << "  Uniform " << i << ": name: " << name << ", type: " << type << std::endl;
		}

	}

	void onFrame(unsigned int dt) override
	{
		RoxRender::clear(true, true);

		m_rot += dt * 0.05f;
		if (m_rot > 360)
			m_rot = 0;

		// Set mesh rotation: yaw = m_rot, pitch = 0, roll = 0
		m_mesh.set_rot(RoxMath::AngleDeg(m_rot), RoxMath::AngleDeg(m_rot), RoxMath::AngleDeg(m_rot));

		RoxMath::Matrix4 mv;
		mv.translate(0, 0, -2.0f);
		mv.rotate(30.0f, 1.0f, 0.0f, 0.0f);
		mv.rotate(m_rot, 0.0f, 1.0f, 0.0f);
		
		RoxRender::setModelViewMatrix(mv);


		//m_shader.bind();
		m_mesh.draw();
		//m_shader.unbind();

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
	}

	void onMouseScroll(int dx, int dy) override
	{
		RoxLogger::log() << "Mouse Scroll\n";

		
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