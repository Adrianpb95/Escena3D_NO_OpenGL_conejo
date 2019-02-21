


#define TINYOBJLOADER_IMPLEMENTATION

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "tiny_obj_loader.h"
#include <cmath>
#include <cassert>
#include "Mesh.hpp"
#include <Vector.hpp>
#include <Scaling.hpp>
#include <Rotation.hpp>
#include <Projection.hpp>
#include <Translation.hpp>
#include <math.h>


////////////////////////////////////////////////////////////
// Usings
////////////////////////////////////////////////////////////
using namespace toolkit;
using namespace tinyobj;

namespace example
{
	///Constructor de Mesh --> Recibe, ordena y guarda los vertices, las normales, los indices y los colores del objeto pasado por parametro
	Mesh::Mesh(const string & obj_file_path)	
    {	
		
        // Se definen los datos de los vértices:		
		attrib_t             attributes;
		vector< shape_t    > shapes;
		vector< material_t > materials;

		// Se intenta cargar el archivo OBJ:
		if (!tinyobj::LoadObj(&attributes, &shapes, &materials, &error, obj_file_path.c_str()) || !error.empty())
		{
			return;
		}

		if (shapes.size() == 0) { error = string("There're no shapes in ") + obj_file_path; return; }
		if (attributes.vertices.size() == 0) { error = string("There're no vertices in ") + obj_file_path; return; }
		if (attributes.normals.size() == 0) { error = string("There're no normals in ") + obj_file_path; return; }
	
		indicesNum = 0; //Contador auxiliar de indices por si tiene mas de un shape

		
		for (auto & shapes : shapes)
		{
			vector< float > vertex_components(shapes.mesh.indices.size() * 3);
			vector< float > normal_components(shapes.mesh.indices.size() * 3);

			const vector< index_t > & indices = shapes.mesh.indices;
			const size_t             indices_count = (size_t)indices.size();			

			if (indices_count > 0)
			{				
				const size_t   vertices_count = indices_count;

				//Se guardan los vertices, las normales y se ordenan los indices
				for (size_t src = 0, dst = 0; src < indices_count; ++src, dst += 3)
				{
					int vertex_src = indices[src].vertex_index * 3;
					int normal_src = indices[src].normal_index * 3;

					vertex_components[dst + 0] = attributes.vertices[vertex_src + 0];
					vertex_components[dst + 1] = attributes.vertices[vertex_src + 1];
					vertex_components[dst + 2] = attributes.vertices[vertex_src + 2];

					normal_components[dst + 0] = attributes.normals[normal_src + 0];
					normal_components[dst + 1] = attributes.normals[normal_src + 1];
					normal_components[dst + 2] = attributes.normals[normal_src + 2];

					original_indices.push_back(indicesNum + src);
				}
			}

			size_t number_of_vertices = shapes.mesh.indices.size();			

			original_vertices.resize(indicesNum + number_of_vertices);
			original_normals.resize(indicesNum + number_of_vertices);

			//Organiza los vertices y las normales
			for (size_t i = 0; i < number_of_vertices; i++)
			{
				original_vertices[indicesNum + i].coordinates()[0] = vertex_components[3 * i + 0];
				original_vertices[indicesNum + i].coordinates()[1] = vertex_components[3 * i + 1];
				original_vertices[indicesNum + i].coordinates()[2] = vertex_components[3 * i + 2];
				original_vertices[indicesNum + i].coordinates()[3] = 1.0f;

				original_normals[indicesNum + i].coordinates()[0] = normal_components[3 * i + 0];
				original_normals[indicesNum + i].coordinates()[1] = normal_components[3 * i + 1];
				original_normals[indicesNum + i].coordinates()[2] = normal_components[3 * i + 2];
				original_normals[indicesNum + i].coordinates()[3] = 1.0f;
			}

			//Los vertices, normales y colores deben tener la misma cantidad de elementos
			transformed_vertices.resize(number_of_vertices + indicesNum);
			transformed_normales.resize(number_of_vertices + indicesNum);
			transformed_colors.resize(number_of_vertices + indicesNum);
			display_vertices.resize(number_of_vertices + indicesNum);

			size_t number_of_colors = number_of_vertices;				
			original_colors.resize(number_of_colors + indicesNum);

			//Guarda los colores
			for (size_t index = 0; index < number_of_colors; index++)
			{
				//original_colors[index].set (attributes.colors[3 * index + 0], attributes.colors[3 * index + 1], attributes.colors[3 * index + 2]);
				original_colors[index+ indicesNum].set(255, 0, 0); //Se da un color base, ya que los obj usados no poseen color propio, y se recibe 1 1 1 como rgb --> negro
			}

			indicesNum = indicesNum + indices_count;
		}
    }

	///Actualiza los componentes del objeto a cada momento, por si esta en movimiento
    void Mesh::update (size_t width, size_t height, Transformation3f & parent_transform)
    {		
        static float angle = 0.f;
        angle += 0.025f;
		
        // Se crean las matrices de transformación:
		Projection3f projection (5, 15, 20, float(width) / float(height));
		Rotation3f    rotation_x;
		Rotation3f    rotation_y;

        rotation_x.set< Rotation3f::AROUND_THE_X_AXIS > (0.5f);
        rotation_y.set< Rotation3f::AROUND_THE_Y_AXIS > (angle);
		
        // Creación de la matriz de transformación unificada sin la proyeccion:

		Transformation3f transformation = parent_transform * translation * rotation_x * rotation_y * scaling;
		
        // Se transforman todos los vértices y las normales usando la matriz de transformación resultante:
        for (size_t index = 0, number_of_vertices = original_vertices.size (); index < number_of_vertices; index++)
        {
            Vertex & vertex = transformed_vertices[index] = Matrix44f(transformation) * Matrix41f(original_vertices[index]);
			Vertex & normal = transformed_normales[index] = Matrix44f(transformation) * Matrix41f(original_normals[index]);
			
			//Se calcula el vector entre el punto de luz y cada vertice
			Point3f vectorL;
			vectorL.coordinates()[0] = (-150.f - original_vertices[index].coordinates()[0]); //Punto de luz (-150,0,-10)
			vectorL.coordinates()[1] = (0.f - original_vertices[index].coordinates()[1]);
			vectorL.coordinates()[2] = (-10.f - original_vertices[index].coordinates()[2]);

			//Se sacan sus vectores unitarios
			float moduloL;			
			moduloL = sqrt(pow(vectorL.coordinates()[0], 2) + pow(vectorL.coordinates()[1], 2) + pow(vectorL.coordinates()[2], 2));
			float moduloN;
			moduloN = sqrt(pow(transformed_normales[index].coordinates()[0], 2) + pow(transformed_normales[index].coordinates()[1], 2) + pow(transformed_normales[index].coordinates()[2], 2));
			
			Point3f vectorL_unitario;
			vectorL_unitario.coordinates()[0] = vectorL.coordinates()[0] / moduloL;
			vectorL_unitario.coordinates()[1] = vectorL.coordinates()[1] / moduloL;
			vectorL_unitario.coordinates()[2] = vectorL.coordinates()[2] / moduloL;

			Point3f vectorN_unitario;
			vectorN_unitario.coordinates()[0] = transformed_normales[index].coordinates()[0] / moduloN;
			vectorN_unitario.coordinates()[1] = transformed_normales[index].coordinates()[1] / moduloN;
			vectorN_unitario.coordinates()[2] = transformed_normales[index].coordinates()[2] / moduloN;
			
			//Se usa el producto escalar de los dos unitarios para sacar la intendisad de la luz 
			float intensidad;
			intensidad = 5*(
				(vectorL_unitario.coordinates()[0] * vectorN_unitario.coordinates()[0]) +
				(vectorL_unitario.coordinates()[1] * vectorN_unitario.coordinates()[1]) +
				(vectorL_unitario.coordinates()[2] * vectorN_unitario.coordinates()[2])
				);

			//El valor de la intensidad tiene q ser siempre entre 0 y 1
			if (intensidad < 0) intensidad = 0; else if (intensidad > 1.f) intensidad = 1.f;

			//Una vez transformados los vertices, aplicamos la proyeccion
			vertex = transformed_vertices[index] = Matrix44f(projection) * Matrix41f(transformed_vertices[index]);
			transformed_colors[index].set((uint8_t)(intensidad*(float)original_colors[index].data.component.r), (uint8_t)(intensidad*(float)original_colors[index].data.component.g), (uint8_t)(intensidad*(float)original_colors[index].data.component.b));
			
            // La matriz de proyección en perspectiva hace que el último componente del vector
            // transformado no tenga valor 1.0, por lo que hay que normalizarlo dividiendo:
            float divisor = 1.f / vertex[3];			
            vertex[0] *= divisor;
            vertex[1] *= divisor;
            vertex[2] *= divisor;
            vertex[3]  = 1.f;
        }

		//Repetimos el proceso por cada hijo declarado
		for (auto & child : children)
		{
			child->update(width,height,transformation);			
		}
    }

	///Pinta los vertices, los triangulos formados con ellos y su color
    void Mesh::paint (Rasterizer<Color_Buffer> rasterizer)
    {
        // Se convierten las coordenadas transformadas y proyectadas a coordenadas
        // de recorte (-1 a +1) en coordenadas de pantalla con el origen centrado.
        // La coordenada Z se escala a un valor suficientemente grande dentro del
        // rango de int (que es lo que espera fill_convex_polygon_z_buffer).

        Scaling3f        scaling        = Scaling3f    (float(rasterizer.get_color_buffer().get_width() / 2), float(rasterizer.get_color_buffer().get_height() / 2), 100000000.f);
        Translation3f    translation    = Translation3f(float(rasterizer.get_color_buffer().get_width() / 2), float(rasterizer.get_color_buffer().get_height() / 2), 0.f);
        Transformation3f transformation = translation * scaling;

        for (size_t index = 0, number_of_vertices = transformed_vertices.size (); index < number_of_vertices; index++)
        {
            display_vertices[index] = Point4i( Matrix44f(transformation) * Matrix41f(transformed_vertices[index]) );
        }

        //Se dibujan los triángulos:
        for (int * indices = original_indices.data (), * end = indices + original_indices.size (); indices < end; indices += 3)
        {
            if (is_frontface (transformed_vertices.data (), indices))
            {
                // Se establece el color del polígono a partir del color de su primer vértice:
                rasterizer.set_color (transformed_colors[*indices]); 

                // Se rellena el polígono:
                rasterizer.fill_convex_polygon_z_buffer (display_vertices.data (), indices, indices + 3);
            }
        }

        // Se copia el framebúffer oculto en el framebúffer de la ventana:
        rasterizer.get_color_buffer ().gl_draw_pixels (0, 0);

		//Se repite el proceso por cada hijo declarado
		for (auto & child : children)
		{
			child->paint(rasterizer);			
		}
    }

	///Informa si el vertice pasado por parametro, se encuentra orientado hacia la camara(true) o es un backfase(false)
    bool Mesh::is_frontface (const Vertex * const projected_vertices, const int * const indices)
    {
        const Vertex & v0 = projected_vertices[indices[0]];
        const Vertex & v1 = projected_vertices[indices[1]];
        const Vertex & v2 = projected_vertices[indices[2]];

        // Se asumen coordenadas proyectadas y polígonos definidos en sentido horario.
        // Se comprueba a qué lado de la línea que pasa por v0 y v1 queda el punto v2:

        return ((v1[0] - v0[0]) * (v2[1] - v0[1]) - (v2[0] - v0[0]) * (v1[1] - v0[1]) > 0.f);
	}

	///Añade un hijo a la Mesh
	void Mesh::add(shared_ptr<Mesh > & m) {			
		children.push_back(m);			
	}

	///El usuario introduce el valor de escala de la Mesh
	void Mesh::setScale(float scaleAux) {
		scaling = scaleAux;
	}

	///El usuario introduce el valor de posicion de la Mesh
	void Mesh::setTranslation(Translation3f translationAux) {
		translation = translationAux;		
	}
}
