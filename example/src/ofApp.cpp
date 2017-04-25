#include "ofApp.h"


void ofApp::setup()
{
    // Make a new wave file.
    writeFile();

    // Try to load the file we just wrote.
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
    int numSamples = numSeconds * sampleRate;

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
    std::vector<float> buffer(numChannels * numSamples, 0);

    // Fill the buffer with the interleved sine wave.
    for (std::size_t k = 0; k < numSamples; ++k)
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

