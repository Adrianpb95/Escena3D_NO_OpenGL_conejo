#ifndef VIEW_HEADER
#define VIEW_HEADER


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <vector>
#include <Point.hpp>
#include "Mesh.hpp"
#include "Rasterizer.hpp"
#include "Color_Buffer_Rgb565.hpp"
#include "Color_Buffer_Rgba8888.hpp"


namespace example
{
	////////////////////////////////////////////////////////////
	// Usings
	////////////////////////////////////////////////////////////
    using std::vector;
    using toolkit::Point4i;
    using toolkit::Point4f;

    class View
    {
    private:

        typedef Color_Buffer_Rgba8888 Color_Buffer;
        typedef Color_Buffer::Color   Color;
        typedef Point4f               Vertex;
  

    private:

		////////////////////////////////////////////////////////////
		// Declaracion de variables			
		////////////////////////////////////////////////////////////	

        size_t						width;
        size_t						height;
        Color_Buffer				Color_buffer;
        Rasterizer< Color_Buffer >	rasterizer;			
		vector< Mesh >				meshArray;
		Transformation3f			firstParent;

    public:

		////////////////////////////////////////////////////////////
		/// \brief Constructor de View --> Crea las meshes y le da valores
		///
		/// width				--> Ancho de la ventana --- 
		/// height				--> Alto de la ventana
		////////////////////////////////////////////////////////////
        View(size_t width, size_t height);	

		////////////////////////////////////////////////////////////
		/// \brief Actualiza todas las Meshes
		///
		////////////////////////////////////////////////////////////
        void update ();

		////////////////////////////////////////////////////////////
		/// \brief Pinta todas las Meshes
		///
		////////////////////////////////////////////////////////////
        void paint  ();			
    };

}

#endif
