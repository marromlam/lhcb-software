/* Generated by Together */

#ifndef PIXELREADOUT_H
#define PIXELREADOUT_H

class RichShape;
class RichNoisifier;
class RichFrontEndDigitiser;
class RichCoder;

class PixelReadout {

public:

  PixelReadout();
  ~PixelReadout();

  const RichShape* Shape() const;
  const RichNoisifier* Noisifier() const;
  const RichFrontEndDigitiser* ADC() const;
  const RichCoder* Coder() const;

  double Gain() const;
  double BaseLine() const;
  double SigmaElecNoise() const;
  double PeakTime() const;

  int FrameSize() const;
  int BinOfMaximum() const;

  void setShape ( RichShape * ) ;
  void setNoisifier( RichNoisifier * ) ;
  void setCoder( RichCoder * ) ;
  void setADC( RichFrontEndDigitiser *) ;
  void setGain ( unsigned int index, double ) ;
  void setBaseLine ( unsigned int index, double ) ;
  void setBaseLine ( double ) ;  //! same value for all index
  void setSigmaElecNoise ( double ) ;
  void setPeakTime ( double ) ;
  void setFrameSize ( int ) ;
  void setBinOfMaximum ( int ) ;

private:

  void cleanUp();

  RichShape* shape_;
  RichCoder* coder_;
  RichNoisifier * noisifier_;
  RichFrontEndDigitiser* frontEnd_;

  int frameSize_;
  int offset_;
  int adc_cut_;
  int binOfMaximum_;

  double peakTime_;
  double gain_;
  double calib_;
  double threshold_;
  double baseline_;
  double sigmaElecNoise_;

};

#endif //PIXELREADOUT_H
