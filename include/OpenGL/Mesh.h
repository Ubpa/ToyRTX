#ifndef _MESH_H_
#define _MESH_H_

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <OpenGL/Shader.h>

#include <string>
#include <vector>

namespace CppUtility {
	namespace OpenGL {
		class Mesh {
		public:
			struct Vertex {
				// position
				glm::vec3 Position;
				// normal
				glm::vec3 Normal;
				// texCoords
				glm::vec2 TexCoords;
				// tangent
				//glm::vec3 Tangent;
				// bitangent
				//glm::vec3 Bitangent;
			};

			struct TextureInfo {
				size_t id;
				std::string type;
				std::string path;
			};

			/*  Functions  */
			// constructor
			Mesh(const std::vector<Vertex> & vertices, const std::vector<size_t> & indices, const std::vector<TextureInfo> & textureInfos);

			// render the mesh
			void Draw(const Shader & shader, const std::string & materialPrefix = "");

		private:
			/*  Mesh Data  */
			std::vector<Vertex> vertices;
			std::vector<size_t> indices;
			std::vector<TextureInfo> textureInfos;
			size_t VAO;

			/*  Render data  */
			size_t VBO, EBO;

			/*  Functions    */
			// initializes all the buffer objects/arrays
			void SetupMesh();
		};
	}
}

#endif// !_MESH_H_
