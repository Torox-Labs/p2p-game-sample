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

		///////////// -- Load Texture -- ////////////////

		////---- Load DDS
		//RoxFormats::DirectDrawSurface ddsFile;
		//FILE* file = nullptr;
		//fopen_s(&file, "resources/test.dds", "rb");
		//if (!file) {
		//	std::cerr << "Failed to open DDS file\n";
		//	return;
		//}
		//
		//// Get file size
		//fseek(file, 0, SEEK_END);
		//size_t fileSize = ftell(file);
		//fseek(file, 0, SEEK_SET);
		//
		//// Read file data into buffer
		//std::vector<char> buffer(fileSize);
		//fread(buffer.data(), 1, fileSize, file);
		//fclose(file);
		//
		//// Decode the DDS header and data
		//size_t headerSize = ddsFile.decodeHeader(buffer.data(), buffer.size());
		//if (headerSize == 0) {
		//	std::cerr << "Failed to decode DDS header\n";
		//	return;
		//}
		//
		//// Now you can use the decoded data to build a texture
		//RoxRender::RoxTexture::COLOR_FORMAT format;
		//switch (ddsFile.pf) {
		//case RoxFormats::DirectDrawSurface::DXT1: format = RoxRender::RoxTexture::DXT1; break;
		//case RoxFormats::DirectDrawSurface::DXT3: format = RoxRender::RoxTexture::DXT3; break;
		//case RoxFormats::DirectDrawSurface::DXT5: format = RoxRender::RoxTexture::DXT5; break;
		//case RoxFormats::DirectDrawSurface::RGBA: format = RoxRender::RoxTexture::COLOR_RGBA; break;
		//case RoxFormats::DirectDrawSurface::BGRA: format = RoxRender::RoxTexture::COLOR_RGBA; break;
		//default: std::cerr << "Unsupported DDS format" << std::endl; return;
		//}
		//m_texture.buildTexture(ddsFile.data, ddsFile.width, ddsFile.height, format,
			//ddsFile.need_generate_mipmaps ? -1 : ddsFile.mipmap_count);

		////---- Load TGA
		// Create a shared texture resrouce from a TGA file
		RoxFormats::TgaFile tgaFile;
		if (!tgaFile.load("wall.tga")) {
			std::cerr << "Failed to load texture!\n";
			return;
		}

		// Get Image Data
		const unsigned char* data = tgaFile.getData();
		int width = tgaFile.getWidth();
		int height = tgaFile.getHeight();
		int Channels = tgaFile.getChannels();

		RoxRender::RoxTexture::COLOR_FORMAT format;
		if (Channels == RoxFormats::TGA::BGR) {
			std::cout << "COLOR RGB\n";
			format = RoxRender::RoxTexture::COLOR_RGB;
		
		}
		else if (Channels == RoxFormats::TGA::BGRA) {
			std::cout << "COLOR RGBA\n";
			format = RoxRender::RoxTexture::COLOR_BGRA;
		}
		else {
			std::cout << "COLOR Grey\n";
			format = RoxRender::RoxTexture::GREYSCALE;
		
		}
		m_texture.buildTexture(data, width, height, format);
		////////////////////////////

		///////////// -- Load Shaders -- ////////////////
		RoxRender::RoxShader::setBinaryShaderCachingEnabled(false);
		m_shader.addProgram(RoxRender::RoxShader::VERTEX, m_vertex_code.c_str());
		if(!m_shader.addProgram(RoxRender::RoxShader::PIXEL, m_fragment_code.c_str()))
		{
			std::cout << "Error Compiling Shaders Shader\n";
			return;
		}
		////////////////////////////

		///////////// -- Save/Load Shaders -- ////////////////
		//RoxRender::RoxCompiledShader shader;
		//RoxSystem::RoxShaderCacheProvider shader_cache_provider;
		//
		//shader_cache_provider.setLoadPath(RoxSystem::getAppPath());
		//shader_cache_provider.setSavePath(RoxSystem::getAppPath());
		//
		//// Combined Both Shaders Text for saving
		//size_t vs_length = strlen(m_vertex_code.c_str());
		//size_t ft_length = strlen(m_fragment_code.c_str());
		//size_t total_length = vs_length + ft_length + 1; // +1 for the null terminator
		//
		//char* combined = new char[total_length];
		//strcpy_s(combined, total_length, m_vertex_code.c_str());
		//strcat_s(combined, total_length, m_fragment_code.c_str());


		//bool shader_exist = true ? false : shader_cache_provider.get(combined, shader);
		//shader_exist = false;
		//std::cout << "Shader Exist: " << (shader_exist ? "true" : "false") << "\n";
		
		
		
		//else if (!shader_exist)
		//{
		//	RoxRender::RoxShader::setBinaryShaderCachingEnabled(true);
		//	m_shader.addProgram(RoxRender::RoxShader::VERTEX, m_vertex_code.c_str());
		//	m_shader.addProgram(RoxRender::RoxShader::PIXEL, m_fragment_code.c_str());
		//
		//	
		//	if (m_shader.getProgramBinaryShader(shader))
		//	{
		//		//std::cout << "Saving binary data to disk\n";
		//		shader_cache_provider.set(combined, shader);
		//	}
		//}
		//else
		//{
		//	if (shader_cache_provider.get(combined, shader))
		//	{
		//		std::cout << "Load binary data to disk\n";
		//		if (!m_shader.setProgramBinaryShader(m_vertex_code.c_str(), m_fragment_code.c_str(), shader))
		//			std::cout << "Error loading binary data";
		//	}
		//	
		//}
		////////////////////////////

		///////////// -- Set Uniforms -- ////////////////
		std::cout << "============= Set Uniforms Data ===============\n";

		// After successfully loading and compiling the shaders
		int lightPosUniform = m_shader.findUniform("lightPos");
		if (lightPosUniform >= 0) {
			m_shader.setUniform(lightPosUniform, 1.0f, 1.0f, 2.0f);
			std::cout << "lightPos uniform found at index " << lightPosUniform << std::endl;
		}
		else {
			std::cout << "lightPos uniform not found!" << std::endl;
		}

		int lightColorUniform = m_shader.findUniform("lightColor");
		if (lightColorUniform >= 0){ 
			m_shader.setUniform(lightColorUniform, 1.0f, 1.0f, 1.0f);
			std::cout << "lightColor uniform found at index " << lightColorUniform << std::endl;
		}
		else {
			std::cout << "lightColor uniform not found!" << std::endl;
		}
		
		int ambientStrengthUniform = m_shader.findUniform("ambientStrength");
		if (ambientStrengthUniform >= 0){
			m_shader.setUniform(ambientStrengthUniform, 0.2f);
			std::cout << "ambientStrength uniform found at index " << ambientStrengthUniform << std::endl;
		}
		else {
			std::cout << "ambientStrength uniform not found!" << std::endl;
		}

		std::cout << "============= Uniforms list ===============\n";

		int uniform_count = m_shader.getUniformsCount();
		std::cout << "Found " << uniform_count << " uniforms:" << std::endl;
		for (int i = 0; i < uniform_count; ++i) {
			std::string name = m_shader.getUniformName(i);
			int type = m_shader.getUniformType(i);
			std::cout << "  Uniform " << i << ": name: " << name << ", type: " << type << std::endl;
		}

		//m_shader.setUniform(0, 1.0f, 0.0f, 0.0f, 1.0f);
		//int uniform_count = m_shader.getUniformsCount();
		//for (int i = 0; i < uniform_count; ++i) {
		//	std::string name = m_shader.getUniformName(i);
		//	int type = m_shader.getUniformType(i);
		//
		//	// Check if it's a sampler type (7 is UNIFORM_SAMPLER2D in most implementations)
		//	if ((name == "ourTex") && (type == 7 || type == 8)) {
		//		m_shader.setUniform(i, 0);  // Set texture unit 0
		//		break;
		//	}
		//}

		//if (uniform_count > 0) {
		//	for (int i = 0; i < uniform_count; ++i)
		//		std::cout << "id: " << i << " Uniform name: " << m_shader.getUniformName(i) << std::endl;
		//	m_shader.setUniform(0, 0);
		//}
		
		//m_shader.setUniform(0, 1.0f, 0.0f, 0.0f, 1.0f);

		//RoxRender::Rectangle viewportInfo = RoxRender::getViewport();
		//std::cout << "x:" << viewportInfo.x << " y: " << viewportInfo.y << " width: " << viewportInfo.width << " height: " << viewportInfo.height << "\n";
		//std::cout << "App Path: " << RoxSystem::getAppPath()<< std::endl;
		//std::cout << "User Path: " << RoxSystem::getUserPath() << std::endl;
		//std::cout << "Statistics state: " << RoxRender::Statistics::enabled() << "\n";
	
		///////////// -- Set Debugging -- ////////////////
		RoxMath::Vector3 line_start = RoxMath::Vector3(0, 0, 0);
		RoxMath::Vector3 line_end = RoxMath::Vector3(1, 1, 1);
		m_debug_draw.addLine(line_start, line_end, RoxMath::Vector4(1, 0, 0, 1));
		m_debug_draw.addTri(RoxMath::Vector3(-0.5f, -0.5f, 0.5f), RoxMath::Vector3(0.5f, -0.5f, 0.5f)
			, RoxMath::Vector3(0.5f, 0.5f, 0.5f), RoxMath::Vector4(1, 0, 0, 1));

		m_debug_draw.addQuad(RoxMath::Vector3(-1, -1, 1), RoxMath::Vector3(1, -1, 1)
			, RoxMath::Vector3(1, 1, 1), RoxMath::Vector3(-1, 1, 1) , RoxMath::Vector4(1, 0, 0, 0.5f));
		//m_debug_draw.addPoint(RoxMath::Vector3(0.5f, 0.5f, 0.5f));
		//m_debug_draw.setPointSize(0.5f);

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
		
		//m_camera.set_pos(m_move_x, m_move_y, m_move_z);
		//m_camera.set_rot(m_rotate_x, m_rotate_y, m_rotate_z);
		//RoxRender::setModelviewMatrix(m_camera.get_view_matrix());

		RoxRender::setModelViewMatrix(mv);
		//RoxRender::setProjectionMatrix(mv);
		//RoxRender::setModelViewMatrix(m_camera.get_view_matrix());

		//RoxRender::setCamera(m_camera.get_view_matrix());
		// Make light move in a circle
		//float lightX = sin(m_rot * 0.01f) * 2.0f;
		//float lightZ = cos(m_rot * 0.01f) * 2.0f;


		// Update light position
		//int lightPosUniform = m_shader.findUniform("lightPos");
		//if (lightPosUniform >= 0) {
		//	m_shader.setUniform(lightPosUniform, lightX, 1.0f, lightZ);
		//}

		m_shader.bind();
		//m_vbo.bind();
		//m_texture.bind(0);
		//m_vbo.draw();
		//m_debug_draw.draw();
		m_mesh.draw();
		//m_texture.unbind(0);
		//m_vbo.unbind();
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