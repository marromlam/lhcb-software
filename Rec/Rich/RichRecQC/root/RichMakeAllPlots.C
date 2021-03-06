
#include "RichPIDPerformance.h"
#include "RichCKResolution.h"
#include "RichTrackResolutions.h"
#include "RichNCKPhotons.h"

namespace Rich
{

  // Creates images files for Aerogel CK resolution, for given root file
  void aerogel( TFile * file,
                const std::string & prodTag,
                const std::string & nameTag,
                const std::string & imageType )
  {
    CKResolution::ckRes(file,prodTag,nameTag,imageType,"Aerogel");
    //TrackResolution::trackRes(file,prodTag,nameTag,imageType,"Aerogel");
  }

  // Creates images files for Aerogel CK resolution, for given root file
  void c4f10( TFile * file,
              const std::string & prodTag,
              const std::string & nameTag,
              const std::string & imageType )
  {
    CKResolution::ckRes(file,prodTag,nameTag,imageType,"Rich1Gas");
    //TrackResolution::trackRes(file,prodTag,nameTag,imageType,"Rich1Gas");
  }

  // Creates images files for Aerogel CK resolution, for given root file
  void cf4( TFile * file,
            const std::string & prodTag,
            const std::string & nameTag,
            const std::string & imageType )
  {
    CKResolution::ckRes(file,prodTag,nameTag,imageType,"Rich2Gas");
    //TrackResolution::trackRes(file,prodTag,nameTag,imageType,"Rich2Gas");
  }

  // shortcut to producing all resolution figures
  void createAllFigures( TFile * file,
                         const std::string & prodTag,
                         const std::string & nameTag )
  {
    if (!file) return;
    gSystem->Exec( ("mkdir -p "+prodTag).c_str() );

    // Create EPS files
    aerogel(file,prodTag,nameTag,"eps");
    c4f10(file,prodTag,nameTag,"eps");
    cf4(file,prodTag,nameTag,"eps");

    // create PNG files
    //aerogel(file,prodTag,nameTag,"png");
    //c4f10(file,prodTag,nameTag,"png");
    //cf4(file,prodTag,nameTag,"png");

    // CK res V theta from ntuple
    //CKResolution::ckResVtheta(file,prodTag,"Aerogel");
    //CKResolution::ckResVtheta(file,prodTag,"Rich1Gas");
    //CKResolution::ckResVtheta(file,prodTag,"Rich2Gas");

  }

  void performance( const std::string hfile,
                    const std::string tag,
                    const std::string title )
  {
    // load data file
    TFile * file = TFile::Open(hfile.c_str());
    if ( !file ) return;
    std::cout << "Opened file : " << hfile << std::endl;

    // file picture types
    std::vector<std::string> imageTypes;
    imageTypes.push_back( "png" );
    //imageTypes.push_back( "eps" );

    // create figures from resolution plots
    //createAllFigures( file, tag, title );

    // PID curves
    PIDPerformance::allPlots( file, tag, title, imageTypes );

    // Number photons
    //NCKPhotons::createAllFigures( file, tag, title );

    file->Close();
  }

}
