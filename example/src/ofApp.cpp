#include "ofApp.h"


void ofApp::setup()
{
    // Make a new wave file.
    writeFile();

    // Try reading it to extract the raw waveform.
    readFile();

    // Try to load the file we just wrote into a sound player.
    player.load("test.wav");

    // Try to play the file we jsut loaded.
    player.play();

    // Make it loop.
    player.setLoop(true);
}


void ofApp::update()
{
}


void ofApp::draw()
{
}


void ofApp::readFile()
{
    // Create a file path.
    std::string filePath = ofToDataPath("test-8-bit.wav", true);

    // Load the file into a handle.
    SndfileHandle soundFile(filePath);

    // Query parameters.
    ofLogNotice("ofApp::readFile") << "Opened file " << filePath;
    ofLogNotice("ofApp::readFile") << "\t     Format:" << soundFile.format();
    ofLogNotice("ofApp::readFile") << "\tSample Rate:" << soundFile.samplerate();
    ofLogNotice("ofApp::readFile") << "\t   # Frames:" << soundFile.frames();
    ofLogNotice("ofApp::readFile") << "\t # Channels:" << soundFile.channels();

    // Calculate duration.
    std::size_t numSamples = soundFile.channels() * soundFile.frames();
    uint64_t durationMicros = (1000000 * soundFile.frames()) / soundFile.samplerate();
    float durationMillis = durationMicros / 1000.0f;

    ofLogNotice("ofApp::readFile") << "\t  # Samples:" << numSamples;
    ofLogNotice("ofApp::readFile") << "\tDuration MS:" << durationMillis;
    ofLogNotice("ofApp::readFile") << "\tDuration uS:" << durationMicros;

    // Create a buffer with the right number of samples.
    std::vector<float> sampleBuffer(numSamples, 0.0f);

    // Read the samples into the memory buffer.
    // libsndfile will automatically normalize data when reading into float
    // buffers. See this link http://www.mega-nerd.com/libsndfile/api.html#note1
    auto numRead = soundFile.read(sampleBuffer.data(), sampleBuffer.size());

    ofLogNotice("ofApp::readFile") << "Loaded file with " << numRead << " samples.";

    // Find the minimum and maximum values in the sound file.
    auto result = std::minmax_element(sampleBuffer.begin(), sampleBuffer.end());

    ofLogNotice("ofApp::readFile") << "Min. amplitude is " << *result.first << " at sample [" << (result.first-sampleBuffer.begin()) << "].";
    ofLogNotice("ofApp::readFile") << "Max. amplitude is " << *result.second << " at sample [" << (result.second-sampleBuffer.begin()) << "].";
}


void ofApp::writeFile()
{
    // Create a file path.
    std::string filePath = ofToDataPath("test.wav", true);

    // Remove the file if it already exists.
    std::filesystem::remove(filePath);

    // Choose a mode for opening the file: SFM_READ, SFM_WRITE, or SFM_RDWR.
    int openMode = SFM_WRITE;

    // Choose a format for saving. This will often include a type and subtype.
    // The available formats are determined by how the library was compiled.
    // Support for formats like FLAC and OGG are optional.
    //
    // Find a list of formats here http://www.mega-nerd.com/libsndfile/api.html.
    int fileFormat = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

    // Choose the number of channels.
    int numChannels = 2;

    // Choose a sample rate in Hz.
    int sampleRate = 11025;

    // Create a handle to a sound file. The SndFileHandle type manages memory by
    // keeping an internal reference count. So if you copy this file by
    // assigning it to another variable, it will reference the original file,
    // not create a deep copy. It behaves somewhat like a std::shared_ptr. When
    // the last reference goes out of scope, the underlying memory is freed. In
    // other words, it uses an RAII approach to memory management.
    SndfileHandle soundFile(filePath,
                            openMode,
                            fileFormat,
                            numChannels,
                            sampleRate);

    // 2. Fill generate a stereo (2 channel) sine wave and save it to the file.

    // First choose some parameters for our sine wave.
    int numSeconds = 4;
    int numFrames = numSeconds * sampleRate;
    int numSamples = numFrames * numChannels;

    float amplitude = 0.75;

    float leftSineFrequency = 344.0;
    float rightSineFrequency = 466.0;

    // Create a buffer.  We use int32_t because we chose SF_FORMAT_PCM_32 above.
    //
    // Other options might include:
    //
    //   SF_FORMAT_PCM_S8 -> int8_t
    //   SF_FORMAT_PCM_16 -> int16_t
    //   SF_FORMAT_PCM_24 -> int32_t // There is no 24-bit int, so this is one way.
    //   SF_FORMAT_PCM_32 -> int32_t
    //
    //   SF_FORMAT_PCM_U8 -> uint8_t (WAV and RAW only)
    //
    //   SF_FORMAT_FLOAT -> float
    //   SF_FORMAT_DOUBLE -> double
    //
    // The buffer can be anny array-like type with a pointer and a known length
    // (e.g. std::vector, std::array and a raw C++ or C array would all work).
    std::vector<float> buffer(numSamples, 0);

    // Fill the buffer with the interleved sine wave.
    for (std::size_t k = 0; k < numFrames; ++k)
    {
        // Calculate the angle
        float angle = k / float(sampleRate) * glm::two_pi<float>();

        // Right channel.
        buffer[numChannels * k    ] = amplitude * sin (leftSineFrequency  * angle);

        // Left channel.
        buffer[numChannels * k + 1] = amplitude * sin (rightSineFrequency * angle);
    }

    // Write the buffer to the file.
    soundFile.write(buffer.data(), buffer.size());

    // The sound file will sync and save when it goes out of scope.
}

