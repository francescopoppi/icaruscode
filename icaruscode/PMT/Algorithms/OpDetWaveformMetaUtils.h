/**
 * @file   icaruscode/PMT/Algorithms/OpDetWaveformMetaUtils.h
 * @brief  Writes a collection of sbn::OpDetWaveformMeta from PMT waveforms.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 22, 2021
 */

#ifndef ICARUSCODE_PMT_ALGORITHMS_OPDETWAVEFORMMETAUTILS_H
#define ICARUSCODE_PMT_ALGORITHMS_OPDETWAVEFORMMETAUTILS_H


// SBN libraries
#include "icaruscode/IcarusObj/OpDetWaveformMeta.h"

// LArSoft libraries
#include "lardataalg/DetectorInfo/DetectorTimingTypes.h" // electronics_time
#include "lardataalg/Utilities/quantities/spacetime.h" // microseconds
#include "lardataobj/RawData/OpDetWaveform.h"

// C/C++ standard libraries
#include <optional>


// -----------------------------------------------------------------------------
// forward declarations
namespace detinfo { class DetectorTimings; }


// -----------------------------------------------------------------------------
namespace sbn {
  
  // --- BEGIN -- Creation of sbn::OpDetWaveformMeta from raw::OpDetWaveform -----
  /**
   * @name Creation of `sbn::OpDetWaveformMeta` from `raw::OpDetWaveform`
   * 
   * The creation of summary objects `sbn::OpDetWaveformMeta` from optical
   * detector waveforms is possible with two options:
   * 
   * * one shot: call to convert a single waveform (`makeOpDetWaveformMeta()`);
   * * bulk: converter object reused for multiple conversions
   *   (`sbn::OpDetWaveformMetaMaker`).
   * 
   * For usage examples, see their respective documentation.
   */
  /// @{
  class OpDetWaveformMetaMaker;
  
  /**
   * @brief Creates a `sbn::OpDetWaveformMeta` out of a `raw::OpDetWaveform`.
   * @param waveform the input waveform
   * @param detTimings timing service provider
   * @return a `sbn::OpDetWaveformMeta` object with the summary information
   * 
   * Returns a new summary object extracted from the `waveform`.
   * 
   * The timing information is used to determine whether the global trigger
   * and beam times are included.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * detinfo::DetectorTimings const detTimings = detinfo::makeDetectorTimings
   *   (art::ServiceHandle<detinfo::DetectorClocksService const>()->DataFor(event));
   * 
   * sbn::OpDetWaveformMeta info = sbn::makeOpDetWaveformMeta(waveform, detTimings);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  OpDetWaveformMeta makeOpDetWaveformMeta(
    raw::OpDetWaveform const& waveform,
    detinfo::DetectorTimings const& detTimings
    );
  
  
  /**
   * @brief Creates a `sbn::OpDetWaveformMeta` out of a `raw::OpDetWaveform`.
   * @param waveform the input waveform
   * @param opDetTickPeriod period of the optical detector digitizer
   * @return a `sbn::OpDetWaveformMeta` object with the summary information
   * 
   * Returns a new summary object extracted from the `waveform`.
   * 
   * Information requiring timing is not saved.
   * 
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * util::quantities::intervals::nanoseconds const opDetPeriod { 2.0 };
   * 
   * sbn::OpDetWaveformMeta info = sbn::makeOpDetWaveformMeta(waveform, opDetPeriod);
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   */
  OpDetWaveformMeta makeOpDetWaveformMeta(
    raw::OpDetWaveform const& waveform,
    util::quantities::intervals::microseconds opDetTickPeriod
    );
  
  /// @}
  // --- END ---- Creation of sbn::OpDetWaveformMeta from raw::OpDetWaveform -----
  
  
} // namespace sbn


// -----------------------------------------------------------------------------
/**
 * @brief Converter from `raw::OpDetWaveform` into `sbn::OpDetWaveformMeta`.
 * 
 * An object of this class is initialized once with some timings (e.g. once per
 * event), used to `make()` multiple `sbn::OpDetWaveformMeta` and then discarded:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * 
 * detinfo::DetectorTimings const detTimings = detinfo::makeDetectorTimings
 *   (art::ServiceHandle<detinfo::DetectorClocksService const>()->DataFor(event));
 * 
 * std::vector<sbn::OpDetWaveformMeta> PMTinfo;
 * for (raw::OpDetWaveform const& waveform: waveforms)
 *   PMTinfo.push_back(makeOpDetWaveformMeta(waveform));
 * 
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * It supports a scenario where times (trigger and beam gate) are not available
 * or not relevant, in which case only the duration in time of a optical
 * detector waveform tick is needed.
 */
class sbn::OpDetWaveformMetaMaker {
  
    public:
  
  using microseconds = util::quantities::intervals::microseconds;
  
  /// Constructor: allows creation of `sbn::OpDetWaveformMeta` with full
  /// information.
  OpDetWaveformMetaMaker(detinfo::DetectorTimings const& detTimings);
  
  
  /// Constructor: allows creation of `sbn::OpDetWaveformMeta` with no
  /// trigger/beam time information.
  OpDetWaveformMetaMaker(microseconds opDetTickPeriod);
  
  //@{
  /// Creates a `sbn::OpDetWaveformMeta` out of the specified `waveform`.
  sbn::OpDetWaveformMeta make(raw::OpDetWaveform const& waveform) const;
  sbn::OpDetWaveformMeta operator() (raw::OpDetWaveform const& waveform) const
    { return make(waveform); }
  //@}
  
    private:
  
  using electronics_time = detinfo::timescales::electronics_time;
  
  microseconds fOpDetTickPeriod; ///< The duration of a optical detector tick.
  
  std::optional<electronics_time> fTriggerTime; ///< Cached trigger time.
  std::optional<electronics_time> fBeamGateTime; ///< Cached beam gate time.
  
}; // sbn::OpDetWaveformMetaMaker


// -----------------------------------------------------------------------------

#endif // ICARUSCODE_PMT_ALGORITHMS_OPDETWAVEFORMMETAUTILS_H
