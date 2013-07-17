#include <math.h>
#include <numeric>
#include "itkImageFileWriter.h"
#include "itkImage.h"

#include "itkResampleImageFilter.h"
#include "itkBSplineInterpolateImageFunction.h"
#include "itkConstrainedValueDifferenceImageFilter.h"
#include "itkConstrainedValueAdditionImageFilter.h"
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

} // end of anonymous namespace

int main( int argc, char * argv[] )
{
  PARSE_ARGS;

  typedef itk::Image<double, 3> InputImageType;  
  typedef itk::Image<unsigned char, 3> LabelImageType;

  typedef itk::ImageFileReader<InputImageType>  ReaderType;
  typedef itk::ImageFileReader<LabelImageType>  LabelReaderType;

  typedef itk::BSplineInterpolateImageFunction<InputImageType>   Interpolator;
  typedef itk::ResampleImageFilter<InputImageType, InputImageType>  ResampleType;
  typedef itk::ConstrainedValueDifferenceImageFilter<InputImageType, InputImageType, InputImageType> Difference;
  typedef itk::ConstrainedValueAdditionImageFilter<InputImageType, InputImageType, InputImageType> Addition;
  typedef itk::LabelStatisticsImageFilter<InputImageType, LabelImageType> LabelStatistics;

  ReaderType::Pointer reader1 = ReaderType::New();
  itk::PluginFilterWatcher watchReader1(reader1, "Read Volume 1",
                                        CLPProcessInformation);

  ReaderType::Pointer reader2 = ReaderType::New();
  itk::PluginFilterWatcher watchReader2(reader2, "Read Volume 2",
                                        CLPProcessInformation);
  
  LabelReaderType::Pointer reader3 = LabelReaderType::New();
  itk::PluginFilterWatcher watchReader3(reader3, "Read Volume 3", 
					CLPProcessInformation);

  try
    {
      reader1->SetFileName( inputVolume1.c_str() );
      reader2->SetFileName( inputVolume2.c_str() );
      reader3->SetFileName( labelVolume.c_str() );
      reader2->ReleaseDataFlagOn();
      reader3->ReleaseDataFlagOn();

      reader1->Update();
      reader2->Update();
      reader3->Update();

      Interpolator::Pointer interp = Interpolator::New();
      interp->SetInputImage(reader2->GetOutput() );
      interp->SetSplineOrder(1);

      ResampleType::Pointer resample = ResampleType::New();
      resample->SetInput(reader2->GetOutput() );
      resample->SetOutputParametersFromImage(reader1->GetOutput() );
      resample->SetInterpolator( interp );
      resample->SetDefaultPixelValue( 0 );
      resample->ReleaseDataFlagOn();

      itk::PluginFilterWatcher watchResample(resample, "Resampling", CLPProcessInformation);

      Difference::Pointer subtract= Difference::New();
      subtract->SetInput1( reader1->GetOutput() );
      subtract->SetInput2( resample->GetOutput() );
      itk::PluginFilterWatcher watchFilter1(subtract, "Subtracting",
					CLPProcessInformation);
      subtract->ReleaseDataFlagOn();
      subtract->Update();
 
      Addition::Pointer add = Addition::New();
      add->SetInput1( reader1->GetOutput() );
      add->SetInput2( resample->GetOutput() );
      itk::PluginFilterWatcher watchFilter2(add, "Adding", 
  					CLPProcessInformation);
      add->ReleaseDataFlagOn();
      add->Update();

      LabelStatistics::Pointer labelStat0 = LabelStatistics::New();
      labelStat0->SetInput(add->GetOutput());
      labelStat0->SetLabelInput(reader3->GetOutput());
      labelStat0->ReleaseDataFlagOn(); 
      labelStat0->Update();

      LabelStatistics::Pointer labelStat1 = LabelStatistics::New();
      labelStat1->SetInput(subtract->GetOutput());
      labelStat1->SetLabelInput(reader3->GetOutput()); 
      labelStat1->ReleaseDataFlagOn();
      labelStat1->Update();

      typedef LabelStatistics::ValidLabelValuesContainerType ValidLabelValuesType;
      typedef LabelStatistics::LabelPixelType LabelPixelType;
  
      std::vector<double> lAddMean;
      std::vector<double> lAddStdv;

      for(ValidLabelValuesType::const_iterator vIt=labelStat0->GetValidLabelValues().begin();
	  vIt != labelStat0->GetValidLabelValues().end(); ++vIt)
	{
	  if(labelStat0->HasLabel(*vIt))
	    {
	      LabelPixelType labelValue = *vIt;
	      lAddMean.push_back(labelStat0->GetMean(labelValue));
	    }
	}

      std::vector<double> lSubMean;
      std::vector<double> lSubStdv;
      for(ValidLabelValuesType::const_iterator vIt=labelStat1->GetValidLabelValues().begin();
	  vIt != labelStat1->GetValidLabelValues().end(); ++vIt)
	{
	  if(labelStat1->HasLabel(*vIt))
	    {
	      LabelPixelType labelValue = *vIt;
	      lSubStdv.push_back(labelStat1->GetSigma(labelValue));  
	    }
	}

      double sumMean = std::accumulate(lAddMean.begin()+1, lAddMean.end(),0);
      double subStdv = std::accumulate(lSubStdv.begin()+1, lSubStdv.end(),0);

      float snr = 10*log10((1.0/sqrt(2.0))*sumMean/subStdv);

      std::ofstream rts;
      rts.open(returnParameterFile.c_str());
      if(snr != snr)
	{
	  rts << "SNR = ERROR "<<std::endl;
	}
      else
	{
	  rts << "SNR = "<<snr<<std::endl;
	}
      rts.close();
    }

  catch( itk::ExceptionObject & excep )
    {
    std::cerr << argv[0] << ": exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
