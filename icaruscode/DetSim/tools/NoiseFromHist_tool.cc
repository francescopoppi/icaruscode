////////////////////////////////////////////////////////////////////////
/// \file   NoiseFromHist.cc
/// \author F. Varanini
////////////////////////////////////////////////////////////////////////

#include <cmath>
#include "IGenNoise.h"
#include "art/Utilities/ToolMacros.h"
#include "messagefacility/MessageLogger/MessageLogger.h"
#include "cetlib_except/exception.h"
#include "lardata/Utilities/LArFFT.h"

// art extensions
#include "nutools/RandomUtils/NuRandomService.h"

// CLHEP libraries
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandGaussQ.h"

#include "TH1D.h"
#include "TFile.h"
#include "TComplex.h"

#include <fstream>

namespace icarus_tool
{

class NoiseFromHist : IGenNoise
{
public:
    explicit NoiseFromHist(const fhicl::ParameterSet& pset);
    
    ~NoiseFromHist();
    
    void configure(const fhicl::ParameterSet& pset)                          override;

    void GenerateNoise(std::vector<float> &noise, double noise_factor) const override;
    
private:
    // Member variables from the fhicl file
    double              fNoiseRand;
    std::string         fInputNoiseHistFileName;
    std::string         fHistogramName;
    
    double              fHistNormFactor;

    // We'll recover the bin contents and store in a vector
    // with the likely false hope this will be faster...
    std::vector<double> fNoiseHistVec;
};
    
//----------------------------------------------------------------------
// Constructor.
NoiseFromHist::NoiseFromHist(const fhicl::ParameterSet& pset)
{
    configure(pset);
}
    
NoiseFromHist::~NoiseFromHist()
{
}
    
void NoiseFromHist::configure(const fhicl::ParameterSet& pset)
{
    // Recover the histogram used for noise generation
    fNoiseRand              = pset.get< double>("NoiseRand");
    fInputNoiseHistFileName = pset.get<std::string>("NoiseHistFileName");
    fHistogramName          = pset.get<std::string>("HistogramName");
    fHistNormFactor         = pset.get<double>("HistNormFactor");
    
    std::string fullFileName;
    cet::search_path searchPath("FW_SEARCH_PATH");
    searchPath.find_file(fInputNoiseHistFileName, fullFileName);
    
    TFile inputFile(fullFileName.c_str(), "READ");
    
    if (!inputFile.IsOpen())
        throw cet::exception("NoiseFromHist::configure") << "Unable to open input file: " << fInputNoiseHistFileName << std::endl;
    
    TH1D* histPtr = (TH1D*)inputFile.Get(fHistogramName.c_str());
    
    if (!histPtr)
        throw cet::exception("NoiseFromHist::configure") << "Unable to recover desired histogram: " << fHistogramName << std::endl;
    
    fNoiseHistVec.resize(histPtr->GetNbinsX(), 0.);
    
    for(size_t histIdx = 0; histIdx < size_t(histPtr->GetNbinsX()); histIdx++)
        fNoiseHistVec[histIdx] = histPtr->GetBinContent(histIdx+1);
    
    // Close the input file
    inputFile.Close();
   
    return;
}

void NoiseFromHist::GenerateNoise(std::vector<float> &noise, double noise_factor) const
{

    art::ServiceHandle<art::RandomNumberGenerator> rng;
    art::ServiceHandle<util::LArFFT>               fFFT;
    
    CLHEP::HepRandomEngine &engine = rng->getEngine("noise");
    
    CLHEP::RandFlat flat(engine,-1,1);
    
    size_t nFFTTicks = fFFT->FFTSize();
    
    if(noise.size() != nFFTTicks)
        throw cet::exception("SimWireICARUS")
        << "\033[93m"
        << "Frequency noise vector length must match FFT Ticks (FFT size)"
        << " ... " << noise.size() << " != " << nFFTTicks
        << "\033[00m"
        << std::endl;
    
    // noise in frequency space
    std::vector<TComplex> noiseFrequency(nFFTTicks/2+1, 0.);
    
    double pval        = 0.;
    double phase       = 0.;
    double rnd[2]      = {0.};
    double scaleFactor = fHistNormFactor * noise_factor;
    
    // width of frequencyBin in kHz
    
    for(size_t i=0; i< nFFTTicks/2 + 1; ++i)
    {
        // exponential noise spectrum
        flat.fireArray(2,rnd,0,1);
        
        pval = fNoiseHistVec[i] * ((1-fNoiseRand) + 2 * fNoiseRand*rnd[0]) * scaleFactor;

        phase = rnd[1] * 2. * TMath::Pi();

        TComplex tc(pval*cos(phase),pval*sin(phase));

        noiseFrequency.at(i) += tc;
    }
    
    // inverse FFT MCSignal
    fFFT->DoInvFFT(noiseFrequency, noise);
    
    noiseFrequency.clear();

    return;
}
    
DEFINE_ART_CLASS_TOOL(NoiseFromHist)
}
