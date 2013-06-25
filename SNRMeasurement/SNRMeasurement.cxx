#include <math.h>
#include "itkImageFileWriter.h"
#include "itkImage.h"

#include "itkResampleImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkConstrainedValueDifferenceImageFilter.h"
#include "itkConstrainedValueAdditionImageFilter.h"
#include "itkStatisticsImageFilter.h"
#include "itkLabelStatisticsImageFilter.h"

#include "itkPluginUtilities.h"

#include "SNRMeasurementCLP.h"

// Use an anonymous namespace to keep class types and function names
// from colliding when module is used as shared object module.  Every
// thing should be in an anonymous namespace except for the module
// entry point, e.g. main()
//
namespace
{

template <class T>
int DoIt( int argc, char * argv[], T )
{
  PARSE_ARGS;

  typedef    T InputPixelType;
  typedef    T OutputPixelType;

  typedef itk::Image<unsigned char, 3> UCharImageType;

  typedef itk::ImageFileReader<UCharImageType>  ReaderType;

  typedef itk::BSplineInterpolateImageFunction<UCharImageType>   Interpolator;
  typedef itk::ResampleImageFilter<UCharImageType, UCharImageType>  ResampleType;
  typedef itk::ConstrainedValueDifferenceImageFilter<UCharImageType, UCharImageType, UCharImageType> Difference;
  typedef itk::ConstrainedValueAdditionImageFilter<UCharImageType, UCharImageType, UCharImageType> Addition;

  typename ReaderType::Pointer reader1 = ReaderType::New();
  itk::PluginFilterWatcher watchReader1(reader1, "Read Volume 1",
                                        CLPProcessInformation);

  typename ReaderType::Pointer reader2 = ReaderType::New();
  itk::PluginFilterWatcher watchReader2(reader2,
                                        "Read Volume 2",
                                        CLPProcessInformation);
  
  typename ReaderType::Pointer reader3 = ReaderType::New();
  itk::PluginFilterWatcher watchReader3(reader3, "Read Volume 3", CLPProcessInformation);

  reader1->SetFileName( inputVolume1.c_str() );
  reader2->SetFileName( inputVolume2.c_str() );
  reader3->SetFileName( labelVolume.c_str() );
  reader2->ReleaseDataFlagOn();
  reader3->ReleaseDataFlagOn();

  reader1->Update();
  reader2->Update();
  reader3->Update();

  typename Interpolator::Pointer interp = Interpolator::New();
  interp->SetInputImage(reader2->GetOutput() );
  interp->SetSplineOrder(1);

  typename ResampleType::Pointer resample = ResampleType::New();
  resample->SetInput(reader2->GetOutput() );
  resample->SetOutputParametersFromImage(reader1->GetOutput() );
  resample->SetInterpolator( interp );
  resample->SetDefaultPixelValue( 0 );
  resample->ReleaseDataFlagOn();

  itk::PluginFilterWatcher watchResample(resample, "Resampling", CLPProcessInformation);

  typename Difference::Pointer subtract= Difference::New();
  subtract->SetInput1( reader1->GetOutput() );
  subtract->SetInput2( resample->GetOutput() );
  itk::PluginFilterWatcher watchFilter1(subtract, "Subtracting",
					CLPProcessInformation);
  subtract->Update();
 
  typename Addition::Pointer add = Addition::New();
  add->SetInput1( reader1->GetOutput() );
  add->SetInput2( resample->GetOutput() );
  itk::PluginFilterWatcher watchFilter2(add, "Adding", 
  					CLPProcessInformation);
  add->Update();

  typedef itk::LabelStatisticsImageFilter<UCharImageType, UCharImageType> LabelStatistics;
  typename LabelStatistics::Pointer labelStat0 = LabelStatistics::New();
  labelStat0->SetInput(add->GetOutput());
  labelStat0->SetLabelInput(reader3->GetOutput());
  labelStat0->Update();

  typename LabelStatistics::Pointer labelStat1 = LabelStatistics::New();
  labelStat1->SetInput(subtract->GetOutput());
  labelStat1->SetLabelInput(reader3->GetOutput());
  labelStat1->Update();

  typedef LabelStatistics::ValidLabelValuesContainerType ValidLabelValuesType;
  typedef LabelStatistics::LabelPixelType LabelPixelType;
  
  float lAddMean = 0;
  float lAddStdv = 0;
  for(ValidLabelValuesType::const_iterator vIt=labelStat0->GetValidLabelValues().begin();
      vIt != labelStat0->GetValidLabelValues().end(); ++vIt)
    {
      if(labelStat0->HasLabel(*vIt))
	{
	  LabelPixelType labelValue = *vIt;
	  lAddMean += labelStat0->GetMean(labelValue);
	  lAddStdv += labelStat0->GetSigma(labelValue);
	}
    }

  float lSubMean = 0;
  float lSubStdv = 0;
  for(ValidLabelValuesType::const_iterator vIt=labelStat1->GetValidLabelValues().begin();
      vIt != labelStat1->GetValidLabelValues().end(); ++vIt)
    {
      if(labelStat1->HasLabel(*vIt))
	{
	  LabelPixelType labelValue = *vIt;
	  lSubMean += labelStat1->GetMean(labelValue);
	  lSubStdv += labelStat1->GetSigma(labelValue);
	}
    }
  
  float snr = 10*log10((1.0/sqrt(2.0))*lAddMean/lSubStdv);

  std::cout<< "SNR is :"<<snr<<std::endl;

  return EXIT_SUCCESS;
}

} // end of anonymous namespace

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  itk::ImageIOBase::IOPixelType     pixelType;
  itk::ImageIOBase::IOComponentType componentType;

  try
    {
    itk::GetImageType(inputVolume1, pixelType, componentType);

    // This filter handles all types on input, but only produces
    // signed types
    switch( componentType )
      {
      case itk::ImageIOBase::UCHAR:
        return DoIt( argc, argv, static_cast<unsigned char>(0) );
        break;
      case itk::ImageIOBase::CHAR:
        return DoIt( argc, argv, static_cast<char>(0) );
        break;
      case itk::ImageIOBase::USHORT:
        return DoIt( argc, argv, static_cast<unsigned short>(0) );
        break;
      case itk::ImageIOBase::SHORT:
        return DoIt( argc, argv, static_cast<short>(0) );
        break;
      case itk::ImageIOBase::UINT:
        return DoIt( argc, argv, static_cast<unsigned int>(0) );
        break;
      case itk::ImageIOBase::INT:
        return DoIt( argc, argv, static_cast<int>(0) );
        break;
      case itk::ImageIOBase::ULONG:
        return DoIt( argc, argv, static_cast<unsigned long>(0) );
        break;
      case itk::ImageIOBase::LONG:
        return DoIt( argc, argv, static_cast<long>(0) );
        break;
      case itk::ImageIOBase::FLOAT:
        return DoIt( argc, argv, static_cast<float>(0) );
        break;
      case itk::ImageIOBase::DOUBLE:
        return DoIt( argc, argv, static_cast<double>(0) );
        break;
      case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      default:
        std::cout << "unknown component type" << std::endl;
        break;
      }
    }

  catch( itk::ExceptionObject & excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
