////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "View.hpp"
#include "Mesh.hpp"
#include <Vector.hpp>
#include <Scaling.hpp>
#include <Translation.hpp>
#include <Shared_Ptr.hpp>

////////////////////////////////////////////////////////////
// Usings
////////////////////////////////////////////////////////////
using namespace std;
using namespace toolkit;


namespace example
{

	///Constructor de View --> Creamos las meshes y les damos valores
	View::View(size_t width, size_t height)
		:
		width(width),
		height(height),
		Color_buffer(width, height),
		rasterizer(Color_buffer)
	{
		
		////////////////////////////////////////////////////////////////////////
		//////////////////EJEMPLO DE MESH PADRE CON HIJO///////////////////////
		////////////////////////////////////////////////////////////////////////
		
		Mesh uno("../../binaries/bunny.obj");							//Padre
		uno.setScale(0.8f);
		uno.setTranslation(Translation3f(0, 0.5, -10));		
		
		Mesh dos("../../binaries/head.obj");									//Hijo	
		dos.setScale(0.5f);
		dos.setTranslation(Translation3f(-1, 0, 1));		
		shared_ptr<Mesh> dos_ptr = std::make_shared<Mesh>(dos);	

		Mesh tres("../../binaries/bunny-lowpoly.obj");									//Hijo	
		tres.setScale(0.5f);
		tres.setTranslation(Translation3f(1, 0, 0));
		shared_ptr<Mesh> tres_ptr = std::make_shared<Mesh>(tres);

		uno.add(dos_ptr);
		uno.add(tres_ptr);
		meshArray.push_back(uno);

		////////////////////////////////////////////////////////////////////////
		////////////////////EJEMPLO DE MESH CON MULTISHAPE//////////////////////
		////////////////////////////////////////////////////////////////////////
		Mesh cuatro("../../binaries/corolla.obj");		
		cuatro.setScale(0.5f);
		cuatro.setTranslation(Translation3f(0, -1, -10));		
		meshArray.push_back(cuatro);	

	}

	///Actualiza todas las Meshes
	void View::update()
	{			
		for (size_t i = 0; i < meshArray.size(); i++){
			meshArray[i].update(width, height, firstParent);
		}
	}

	///Pinta todas las Meshes
	void View::paint(){
		//Se limpian toda la pantalla
		rasterizer.clear();
		for (size_t i = 0; i < meshArray.size(); i++){
			meshArray[i].paint(rasterizer);
		}
	}
}
