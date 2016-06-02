// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SoapyMultiSDR.hpp"

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
    const std::vector<size_t> &channels,
    const SoapySDR::Kwargs &args)
{
    
}

void SoapyMultiSDR::closeStream(SoapySDR::Stream *stream)
{
    
}

size_t SoapyMultiSDR::getStreamMTU(SoapySDR::Stream *stream) const
{
    
}

int SoapyMultiSDR::activateStream(
    SoapySDR::Stream *stream,
    const int flags,
    const long long timeNs,
    const size_t numElems)
{
    
}

int SoapyMultiSDR::deactivateStream(
    SoapySDR::Stream *stream,
    const int flags,
    const long long timeNs)
{
    
}

int SoapyMultiSDR::readStream(
    SoapySDR::Stream *stream,
    void * const *buffs,
    const size_t numElems,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    
}

int SoapyMultiSDR::writeStream(
    SoapySDR::Stream *stream,
    const void * const *buffs,
    const size_t numElems,
    int &flags,
    const long long timeNs,
    const long timeoutUs)
{
    
}

int SoapyMultiSDR::readStreamStatus(
    SoapySDR::Stream *stream,
    size_t &chanMask,
    int &flags,
    long long &timeNs,
    const long timeoutUs)
{
    
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
