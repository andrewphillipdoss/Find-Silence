//include header file, where class and member delcarations happen
#include "FindSilence.h"
#include <math.h>

// constructor - member initialization list
FindSilence::FindSilence(float inputSampleRate) :
    Plugin(inputSampleRate),
    m_stepSize(0),
    m_blockSize(0),
    m_threshold(1e-5),
    sensitivity(0.1),
    prevOverThreshold(0),
    time_of_last_feature(0)

    // Also be sure to set your plugin parameters (presumably stored
    // in member variables) to their default values here -- the host
    // will not do that for you

{

}
// destructor
FindSilence::~FindSilence()
{
    prevOverThreshold = 0;
    time_of_last_feature = 0;
}

string
FindSilence::getIdentifier() const
{
    return "find_silence";
}

string
FindSilence::getName() const
{
    return "Find Silence";
}

string
FindSilence::getDescription() const
{
    // Return something helpful here!
    return "Calculates times when power goes over or under a certain threshold";
}

string
FindSilence::getMaker() const
{
    // Your name here
    return "Andrew Doss";
}

int
FindSilence::getPluginVersion() const
{
    // Increment this each time you release a version that behaves
    // differently from the previous one
    return 1;
}

string
FindSilence::getCopyright() const
{
    // This function is not ideally named.  It does not necessarily
    // need to say who made the plugin -- getMaker does that -- but it
    // should indicate the terms under which it is distributed.  For
    // example, "Copyright (year). All Rights Reserved", or "GPL"
    return "";
}

FindSilence::InputDomain
FindSilence::getInputDomain() const
{
    return TimeDomain;
}

size_t
FindSilence::getPreferredBlockSize() const
{
    return 0; // 0 means "I can handle any block size"
}

size_t
FindSilence::getPreferredStepSize() const
{
    return 0; // 0 means "anything sensible"; in practice this
              // means the same as the block size for TimeDomain
              // plugins, or half of it for FrequencyDomain plugins
}

size_t
FindSilence::getMinChannelCount() const
{
    return 1;
}

size_t
FindSilence::getMaxChannelCount() const
{
    return 1;
}

FindSilence::ParameterList
FindSilence::getParameterDescriptors() const
{
    ParameterList list;

    // If the plugin has no adjustable parameters, return an empty
    // list here (and there's no need to provide implementations of
    // getParameter and setParameter in that case either).

    // Note that it is your responsibility to make sure the parameters
    // start off having their default values (e.g. in the constructor
    // above).  The host needs to know the default value so it can do
    // things like provide a "reset to default" function, but it will
    // not explicitly set your parameters to their defaults for you if
    // they have not changed in the mean time.

    ParameterDescriptor t;
    t.identifier = "threshold";
    t.name = "Silence Threshold";
    t.description = "";
    t.unit = "";
    t.minValue = 0;
    t.maxValue = 1;
    t.defaultValue = 1e-5;
    t.isQuantized = false;

    ParameterDescriptor s;
    s.identifier = "sensitivity";
    s.name = "Silence Sensitivity";
    s.description = "";
    s.unit = "";
    s.minValue = 0;
    s.maxValue = 5;
    s.defaultValue = 0.5;
    s.isQuantized = false;
    list.push_back(t);

    return list;
}

float
FindSilence::getParameter(string identifier) const
{
    if (identifier == "threshold") {
        return m_threshold; // return the ACTUAL current value of your parameter here!
    }
    if (identifier == "sensitivity") {
        return sensitivity;
    }
    return 0.f;
}

void
FindSilence::setParameter(string identifier, float value)
{
    if (identifier == "threshold") {
        // set the actual value of your parameter
        if (value < 0) value = 0;
        if (value > 1) value = 1;
        m_threshold = value;
    }
    if (identifier == "sensitivity") {
        if (value < 0) value = 0;
        if (value > 5) value = 0;
        sensitivity = value;
    }

}

FindSilence::ProgramList
FindSilence::getPrograms() const
{
    ProgramList list;

    // If you have no programs, return an empty list (or simply don't
    // implement this function or getCurrentProgram/selectProgram)

    return list;
}

string
FindSilence::getCurrentProgram() const
{
    return ""; // no programs
}

void
FindSilence::selectProgram(string name)
{
}

FindSilence::OutputList
FindSilence::getOutputDescriptors() const
{
    OutputList list;

    // See OutputDescriptor documentation for the possibilities here.
    // Every plugin must have at least one output.

    OutputDescriptor t;
    t.identifier = "over_threshold";
    t.name = "overThreshold";
    t.description = "";
    t.unit = "";
    t.hasFixedBinCount = true;
    t.binCount = 0;
    t.hasKnownExtents = false;
    t.isQuantized = false;
    t.sampleType = OutputDescriptor::VariableSampleRate;
    t.sampleRate = m_inputSampleRate;
    list.push_back(t);

    return list;
}

bool
FindSilence::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
    if (channels < getMinChannelCount() || channels > getMaxChannelCount()) return false;

    // Real initialisation work goes here!
    m_blockSize = blockSize;
    m_stepSize = stepSize;
    prevOverThreshold = 0;

    return true;
}

void
FindSilence::reset()
{
    // Clear buffers, reset stored values, etc
}

FindSilence::FeatureSet
FindSilence::process(const float *const *inputBuffers, Vamp::RealTime timestamp)
{

    Feature f;
    f.hasTimestamp = true;
    FeatureSet fs1;
    // Do actual work!
    float sumOfSquares = 0.0f;
    int overThreshold = 0;

    
    //We use current_timestamp as a Vamp::RealTime object to return timestamp so it can be output, 
    //and current_time as a float representation of that object (to be used in conjunction with the sensitivity parameter)
    Vamp::RealTime current_timestamp = timestamp - Vamp::RealTime::frame2RealTime(m_stepSize, lrintf(m_inputSampleRate));
    float current_time = (float)current_timestamp.sec + ((float)current_timestamp.nsec)/1e09;

    size_t i = 0;

    while (i < m_blockSize) {
      float sample = inputBuffers[0][i];
      sumOfSquares += sample * sample;
      ++i;
    }

    float meanPower = sumOfSquares / m_blockSize;

    if (meanPower >= m_threshold) {
      overThreshold = 1;
    }
    else {
      overThreshold = 0;
    }


    if ((overThreshold != prevOverThreshold) && (current_time - time_of_last_feature) > sensitivity)
    {
      f.timestamp = current_timestamp;
      fs1[0].push_back(f);
      time_of_last_feature = current_time;
    }

    prevOverThreshold = overThreshold;

    return fs1;
}


FindSilence::FeatureSet
FindSilence::getRemainingFeatures()
{
    return FeatureSet();
}
