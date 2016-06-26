// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SoapyMultiSDR.hpp"

struct SoapyMultiStreamData
{
    SoapySDR::Device *device;
    SoapySDR::Stream *stream;
    std::vector<size_t> channels;
};

struct SoapyMultiStreamsData : std::vector<SoapyMultiStreamData>
{
    //placeholder for book-keeping common to all streams
};

/*******************************************************************
 * Stream API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::getStreamFormats(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getStreamFormats(direction, localChannel);
}

std::string SoapyMultiSDR::getNativeStreamFormat(const int direction, const size_t channel, double &fullScale) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getNativeStreamFormat(direction, localChannel, fullScale);
}

SoapySDR::ArgInfoList SoapyMultiSDR::getStreamArgsInfo(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getStreamArgsInfo(direction, localChannel);
}

SoapySDR::Stream *SoapyMultiSDR::setupStream(
    const int direction,
    const std::string &format,
    const std::vector<size_t> &channels_,
    const SoapySDR::Kwargs &args)
{
    //ensure channels is at least size 1
    std::vector<size_t> channels(channels_);
    if (channels.empty()) channels.push_back(0);

    //stream the data structure
    auto multiStreams = new SoapyMultiStreamsData();

    //iterate through the channels to fill the data structure
    for (const auto &channel : channels)
    {
        size_t localChannel = 0;
        auto device = this->getDevice(direction, channel, localChannel);
        if (multiStreams->empty() or multiStreams->back().device != device)
        {
            multiStreams->resize(multiStreams->size()+1);
        }
        multiStreams->back().device = device;
        multiStreams->back().channels.push_back(localChannel);
    }

    //create the streams
    for (auto &multiStream : *multiStreams)
    {
        multiStream.stream = multiStream.device->setupStream(
            direction, format, multiStream.channels, args);
    }

    return reinterpret_cast<SoapySDR::Stream *>(multiStreams);
}

void SoapyMultiSDR::closeStream(SoapySDR::Stream *stream)
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);
    for (auto &multiStream : *multiStreams)
    {
        multiStream.device->closeStream(multiStream.stream);
    }
    delete multiStreams;
}

size_t SoapyMultiSDR::getStreamMTU(SoapySDR::Stream *stream) const
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);
    const auto &stream0 = multiStreams->front();
    return stream0.device->getStreamMTU(stream0.stream);
}

int SoapyMultiSDR::activateStream(
    SoapySDR::Stream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);
    for (auto &multiStream : *multiStreams)
    {
        int ret = multiStream.device->activateStream(multiStream.stream, flags, timeNs, numElems);
        if (ret != 0) return ret;
    }
    return 0;
}

int SoapyMultiSDR::deactivateStream(
    SoapySDR::Stream *stream,
    const int flags,
    const long long timeNs)
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);
    for (auto &multiStream : *multiStreams)
    {
        int ret = multiStream.device->deactivateStream(multiStream.stream, flags, timeNs);
        if (ret != 0) return ret;
    }
    return 0;
}

int SoapyMultiSDR::readStream(
    SoapySDR::Stream *stream,
    void * const *buffs,
    const size_t numElems,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);

    int ret = 0;
    int offset = 0;
    int originalFlags = flags;
    int flagsOut = 0;
    long long timeNsOut = 0;

    for (auto &multiStream : *multiStreams)
    {
        flags = originalFlags; //restore flags before each call
        ret = multiStream.device->readStream(multiStream.stream,
            buffs+offset, numElems, flags, timeNs, timeoutUs);
        if (ret <= 0) return ret;

        //on the first readStream, store the output flags and time
        if (offset == 0)
        {
            flagsOut = flags;
            timeNsOut = timeNs;
        }

        offset += multiStream.channels.size();
    }

    //Note: ret represents the last read number of elements
    //this should be homogeneous, but if not, this implementation
    //should be improved to handle different length reads
    //by saving the remainder and writing it into the buffer
    //on subsequent calls and performing other book-keeping tasks.

    //setup the result
    flags = flagsOut;
    timeNs = timeNsOut;
    return ret;
}

int SoapyMultiSDR::writeStream(
    SoapySDR::Stream *stream,
    const void * const *buffs,
    const size_t numElems,
    int &flags,
    const long long timeNs,
    const long timeoutUs)
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);

    int ret = 0;
    int offset = 0;
    int originalFlags = flags;
    int flagsOut = 0;

    for (auto &multiStream : *multiStreams)
    {
        flags = originalFlags; //restore flags before each call
        ret = multiStream.device->writeStream(multiStream.stream,
            buffs+offset, numElems, flags, timeNs, timeoutUs);
        if (ret <= 0) return ret;

        //on the first writeStream, store the output flags
        if (offset == 0) flagsOut = flags;

        offset += multiStream.channels.size();
    }

    //Note: ret represents the last written number of elements
    //this should be homogeneous, but if not, this implementation
    //should be improved to handle different length writes
    //by saving the remainder and reading it from the buffer
    //on subsequent calls and performing other book-keeping tasks.

    //setup the result
    flags = flagsOut;
    return ret;
}

int SoapyMultiSDR::readStreamStatus(
    SoapySDR::Stream *stream,
    size_t &chanMask,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    auto multiStreams = reinterpret_cast<SoapyMultiStreamsData *>(stream);

    int ret = 0;
    size_t offset = 0;
    for (auto &multiStream : *multiStreams)
    {
        ret = multiStream.device->readStreamStatus(multiStream.stream,
            chanMask, flags, timeNs, timeoutUs);

        chanMask <<= offset; //mask bits shifted up for global channel mapping

        if (ret == 0) return ret; //status message found

        offset += multiStream.channels.size();
    }

    //return last-seen error code (probably timeout or not supported)
    return ret;
}

/*******************************************************************
 * Direct buffer access API
 ******************************************************************/

size_t SoapyMultiSDR::getNumDirectAccessBuffers(SoapySDR::Stream *stream)
{
    
}

int SoapyMultiSDR::getDirectAccessBufferAddrs(SoapySDR::Stream *stream, const size_t handle, void **buffs)
{
    
}

int SoapyMultiSDR::acquireReadBuffer(
    SoapySDR::Stream *stream,
    size_t &handle,
    const void **buffs,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    
}

void SoapyMultiSDR::releaseReadBuffer(
    SoapySDR::Stream *stream,
    const size_t handle)
{
    
}

int SoapyMultiSDR::acquireWriteBuffer(
    SoapySDR::Stream *stream,
    size_t &handle,
    void **buffs,
    const long timeoutUs)
{
    
}

void SoapyMultiSDR::releaseWriteBuffer(
    SoapySDR::Stream *stream,
    const size_t handle,
    const size_t numElems,
    int &flags,
    const long long timeNs)
{
    
}
