
# where should the scripts/..xml file be installed?  Perhaps in bin?

art_make(LIB_LIBRARIES
           lardataalg_DetectorInfo
           lardataobj_RecoBase
           larcorealg_Geometry
           lardata_ArtDataHelper
           lardata_RecoObjects
           art::Framework_Core
           art::Framework_Principal
           art::Framework_Services_Registry
           art_root_io::tfile_support
           ${ART_UTILITIES}
           ROOT::Core
           ROOT::Gpad
           ROOT::Hist
           ROOT::Matrix
           ROOT::Physics
           ROOT::Tree
           art_root_io::tfile_support
           art_root_io::TFileService_service
           art::Persistency_Provenance
           canvas::canvas
           messagefacility::MF_MessageLogger
           fhiclcpp::fhiclcpp
           cetlib::cetlib
           cetlib_except::cetlib_except
        TOOL_LIBRARIES 
           larreco_RecoAlg_Cluster3DAlgs
           lardataalg_DetectorInfo
           lardataobj_RecoBase
           lardata_ArtDataHelper
           art::Framework_Services_Registry
           art_root_io::tfile_support
           art_root_io::TFileService_service
           ROOT::Tree
        MODULE_LIBRARIES
          larreco_Calorimetry
          larreco_RecoAlg_Cluster3DAlgs
          larreco_RecoAlg_TCAlg
          larreco_RecoAlg
          larreco_ClusterFinder
          larsim_MCCheater_ParticleInventoryService_service
          lardataobj_AnalysisBase
          lardataobj_RecoBase
          nurandom::RandomUtils_NuRandomService_service
          nusimdata::SimulationBase
          art::Framework_Services_Registry
          art_root_io::tfile_support
          ROOT::Core
          ROOT::Physics
          art_root_io::TFileService_service
          messagefacility::MF_MessageLogger
       )

install_headers()
install_fhicl()
install_source()
