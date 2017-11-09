
// This is a skeleton file for use in creating your own plugin
// libraries.  Replace MyPlugin and myPlugin throughout with the name
// of your first plugin class, and fill in the gaps as appropriate.


// Remember to use a different guard symbol in each header!
#ifndef FIND_SILENCE_H
#define FIND_SILENCE_H


#include <vamp-sdk/Plugin.h>

//includes the string class from the std namespace
using std::string;

// creates a class Findsilence that inherits from Plugin
class FindSilence : public Vamp::Plugin
{
  // these members are public: everything that is aware of FindSilence is aware of these
public:
    // parameterized class constructor declaration (see FindSilence.cpp for its guts)
    FindSilence(float inputSampleRate);
    // virtual class destructor declaration - objects will be destructed in an order (firstly derived object, then base)
    virtual ~FindSilence();

    // declare const functions - const makes it a compiler error for these functions to change a member variable of the class.
    string getIdentifier() const;
    string getName() const;
    string getDescription() const;
    string getMaker() const;
    int getPluginVersion() const;
    string getCopyright() const;

    InputDomain getInputDomain() const;
    // size_t is the unsigned integer type of the result of the sizeof operator
    size_t getPreferredBlockSize() const; //uses size_t to model size
    size_t getPreferredStepSize() const; //uses size_t to model size
    size_t getMinChannelCount() const; //uses size_t to model size
    size_t getMaxChannelCount() const; //uses size_t to model size

    ParameterList getParameterDescriptors() const;
    float getParameter(string identifier) const;
    void setParameter(string identifier, float value);

    ProgramList getPrograms() const;
    string getCurrentProgram() const;
    void selectProgram(string name);

    OutputList getOutputDescriptors() const;

    bool initialise(size_t channels, size_t stepSize, size_t blockSize);
    void reset();

    // for the inputBuffers parameter: it's a constant float pointer that is a constant to data that is constant
    FeatureSet process(const float *const *inputBuffers,
                       Vamp::RealTime timestamp);

    FeatureSet getRemainingFeatures();

// These are protected members, only the children (and thier Children) are aware of these
protected:
    // plugin-specific data and methods go here
    size_t m_stepSize;
    size_t m_blockSize;

    float m_threshold;
    float sensitivity; //in seconds

    int prevOverThreshold;
    float time_of_last_feature;

};



#endif
