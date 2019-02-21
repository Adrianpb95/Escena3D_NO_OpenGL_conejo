#pragma once
#ifndef MESH_HEADER
#define MESH_HEADER


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <vector>
#include <Point.hpp>
#include "Rasterizer.hpp"
#include "Color_Buffer_Rgb565.hpp"
#include "Color_Buffer_Rgba8888.hpp"
#include <list>
#include <Shared_Ptr.hpp>
#include "Transformation.hpp"
#include "Translation.hpp"
#include "Scaling.hpp"

////////////////////////////////////////////////////////////
// Usings
////////////////////////////////////////////////////////////
using namespace std;
using namespace toolkit;

namespace example
{
	class Mesh
	{
	private:

		typedef Color_Buffer_Rgba8888 Color_Buffer;
		typedef Color_Buffer::Color   Color;
		typedef Point4f               Vertex;
		typedef vector< Vertex >      Vertex_Buffer;
		typedef vector< int    >      Index_Buffer;
		typedef vector< Color  >      Vertex_Colors;

	private:

		////////////////////////////////////////////////////////////
		// Declaracion de variables			
		////////////////////////////////////////////////////////////				

		Vertex_Buffer			original_vertices;
		Index_Buffer			original_indices;
		Vertex_Colors			original_colors;
		Vertex_Buffer			transformed_vertices;
		Vertex_Buffer			transformed_normales;
		vector< Point4i >		display_vertices;	
		string					error;		
		vector< Point4f >		original_normals;
		vector< Color  >		transformed_colors;
		size_t					indicesNum;		
		Scaling3f				scaling;		
		Translation3f			translation;
		list<shared_ptr<Mesh>>	children;

	public:

		////////////////////////////////////////////////////////////
		/// \brief Constructor de Mesh	--> Recibe, ordena y guarda los vertices, las normales, los indices y los colores del objeto pasado por parametro
		///
		/// obj_file_path				--> Ruta del archivo a introducir
		////////////////////////////////////////////////////////////
		Mesh(const string & obj_file_path);	

		////////////////////////////////////////////////////////////
		/// \brief Actualiza los componentes del objeto a cada momento, por si esta en movimiento
		///
		/// width				--> Ancho de la ventana --- 
		/// height				--> Alto de la ventana --- 
		/// parent_transform	--> Transformation del padre pasada a los hijos. Si es el padre, recibe 1
		////////////////////////////////////////////////////////////
		void update(size_t width, size_t height, Transformation3f & parent_transform);

		////////////////////////////////////////////////////////////
		/// \brief Pinta los vertices, los triangulos formados con ellos y su color
		///
		/// rasterizer			--> Encargado de renderizar
		////////////////////////////////////////////////////////////
		void paint(Rasterizer<Color_Buffer> rasterizer);

		////////////////////////////////////////////////////////////
		/// \brief Informa si el vertice pasado por parametro, se encuentra orientado hacia la camara(true) o es un backfase(false)
		///
		/// projected_vertices			--> Vertices a analizar --- 
		/// indices					--> Indices ordenados
		////////////////////////////////////////////////////////////
		bool is_frontface(const Vertex * const projected_vertices, const int * const indices);

		////////////////////////////////////////////////////////////
		/// \brief Añade un hijo a la Mesh
		///
		/// m			--> Mesh hija			
		////////////////////////////////////////////////////////////
		void add(shared_ptr <Mesh> & m);	

		////////////////////////////////////////////////////////////
		/// \brief 	El usuario introduce el valor de escala de la Mesh
		///
		/// scale			--> Escala de la Mesh		
		////////////////////////////////////////////////////////////
		void setScale(float scale);

		////////////////////////////////////////////////////////////
		/// \brief 	El usuario introduce el valor de posicion de la Mesh
		///
		/// translation	--> Posicion de la Mesh		
		////////////////////////////////////////////////////////////
		void setTranslation(Translation3f translation);
		
	};
}

#endif
