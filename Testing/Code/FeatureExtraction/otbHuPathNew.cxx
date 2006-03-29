/*=========================================================================

  Programme :   OTB (ORFEO ToolBox)
  Auteurs   :   CS - P.Imbo
  Language  :   C++
  Date      :   24 mars 2006
  Version   :   
  Role      :   
  $Id$

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#include "itkExceptionObject.h"
#include "otbHuPathFunction.h"
#include "itkPolyLineParametricPath.h"

int otbHuPathNew( int argc, char ** argv )
{
  try 
    { 
        const   unsigned int                                     Dimension = 2;
	typedef itk::PolyLineParametricPath< Dimension >	 PathType;
	typedef otb::HuPathFunction<PathType>                    FunctionType;
	typedef FunctionType::RealType                           RealType;

	FunctionType::Pointer function =FunctionType::New();

    } 
  catch( itk::ExceptionObject & err ) 
    { 
    std::cout << "Exception itk::ExceptionObject levee !" << std::endl; 
    std::cout << err << std::endl; 
    return EXIT_FAILURE;
    } 
  catch( ... ) 
    { 
    std::cout << "Exception levee inconnue !" << std::endl; 
    return EXIT_FAILURE;
    } 
  return EXIT_SUCCESS;
}

