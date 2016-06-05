// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SoapyMultiSDR.hpp"
#include <SoapySDR/Logger.hpp>
#include <mutex>

//mutex to protect the factory
static std::mutex factoryMutex;

SoapyMultiSDR::SoapyMultiSDR(const std::vector<SoapySDR::Kwargs> &args)
{
    //Create the device at each index
    _devices.resize(args.size());
    for (size_t i = 0; i < args.size(); i++)
    {
        SoapySDR::logf(SOAPY_SDR_INFO, "Making device %z...", i);
        std::lock_guard<std::mutex> lock(factoryMutex);
        _devices[i] = SoapySDR::Device::make(args.at(i));
    }

    //load the channels lookup
    this->reloadChanMaps();
}

SoapyMultiSDR::~SoapyMultiSDR(void)
{
    //cleanup each device
    for (auto device : _devices)
    {
        if (device == nullptr) continue;
        std::lock_guard<std::mutex> lock(factoryMutex);
        SoapySDR::Device::unmake(device);
    }
}

void SoapyMultiSDR::reloadChanMaps(void)
{
    _rxChanMap.clear();
    _txChanMap.clear();

    //map global channel index to local index with device
    for (auto device : _devices)
    {
        for (size_t ch = 0; ch < device->getNumChannels(SOAPY_SDR_RX); ch++)
        {
            _rxChanMap.push_back(std::make_pair(ch, device));
        }
        for (size_t ch = 0; ch < device->getNumChannels(SOAPY_SDR_TX); ch++)
        {
            _txChanMap.push_back(std::make_pair(ch, device));
        }
    }
}

/*******************************************************************
 * Channels API
 ******************************************************************/

void SoapyMultiSDR::setFrontendMapping(const int direction, const std::string &mapping)
{
    //TODO use multi-format for mapping
    this->reloadChanMaps();
}

std::string SoapyMultiSDR::getFrontendMapping(const int direction) const
{
    //TODO multi-format for mapping
}

size_t SoapyMultiSDR::getNumChannels(const int direction) const
{
    const auto &map = (direction == SOAPY_SDR_RX)?_rxChanMap:_txChanMap;
    return map.size();
}

bool SoapyMultiSDR::getFullDuplex(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFullDuplex(direction, localChannel);
}

/*******************************************************************
 * Antenna API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listAntennas(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->listAntennas(direction, localChannel);
}

void SoapyMultiSDR::setAntenna(const int direction, const size_t channel, const std::string &name)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setAntenna(direction, localChannel, name);
}

std::string SoapyMultiSDR::getAntenna(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getAntenna(direction, localChannel);
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

bool SoapyMultiSDR::hasDCOffsetMode(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->hasDCOffsetMode(direction, localChannel);
}

void SoapyMultiSDR::setDCOffsetMode(const int direction, const size_t channel, const bool automatic)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setDCOffsetMode(direction, localChannel, automatic);
}

bool SoapyMultiSDR::getDCOffsetMode(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getDCOffsetMode(direction, localChannel);
}

bool SoapyMultiSDR::hasDCOffset(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->hasDCOffset(direction, localChannel);
}

void SoapyMultiSDR::setDCOffset(const int direction, const size_t channel, const std::complex<double> &offset)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setDCOffset(direction, localChannel, offset);
}

std::complex<double> SoapyMultiSDR::getDCOffset(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getDCOffset(direction, localChannel);
}

bool SoapyMultiSDR::hasIQBalance(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->hasIQBalance(direction, localChannel);
}

void SoapyMultiSDR::setIQBalance(const int direction, const size_t channel, const std::complex<double> &balance)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setIQBalance(direction, localChannel, balance);
}

std::complex<double> SoapyMultiSDR::getIQBalance(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getIQBalance(direction, localChannel);
}

/*******************************************************************
 * Gain API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listGains(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->listGains(direction, localChannel);
}

bool SoapyMultiSDR::hasGainMode(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->hasGainMode(direction, localChannel);
}

void SoapyMultiSDR::setGainMode(const int direction, const size_t channel, const bool automatic)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setGainMode(direction, localChannel, automatic);
}

bool SoapyMultiSDR::getGainMode(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getGainMode(direction, localChannel);
}

void SoapyMultiSDR::setGain(const int direction, const size_t channel, const double value)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setGain(direction, localChannel, value);
}

void SoapyMultiSDR::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setGain(direction, localChannel, name, value);
}

double SoapyMultiSDR::getGain(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getGain(direction, localChannel);
}

double SoapyMultiSDR::getGain(const int direction, const size_t channel, const std::string &name) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getGain(direction, localChannel, name);
}

SoapySDR::Range SoapyMultiSDR::getGainRange(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getGainRange(direction, localChannel);
}

SoapySDR::Range SoapyMultiSDR::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getGainRange(direction, localChannel, name);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/

void SoapyMultiSDR::setFrequency(const int direction, const size_t channel, const double frequency, const SoapySDR::Kwargs &args)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setFrequency(direction, localChannel, frequency, args);
}

void SoapyMultiSDR::setFrequency(const int direction, const size_t channel, const std::string &name, const double frequency, const SoapySDR::Kwargs &args)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setFrequency(direction, localChannel, name, frequency, args);
}

double SoapyMultiSDR::getFrequency(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFrequency(direction, localChannel);
}

double SoapyMultiSDR::getFrequency(const int direction, const size_t channel, const std::string &name) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFrequency(direction, localChannel, name);
}

std::vector<std::string> SoapyMultiSDR::listFrequencies(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->listFrequencies(direction, localChannel);
}

SoapySDR::RangeList SoapyMultiSDR::getFrequencyRange(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFrequencyRange(direction, localChannel);
}

SoapySDR::RangeList SoapyMultiSDR::getFrequencyRange(const int direction, const size_t channel, const std::string &name) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFrequencyRange(direction, localChannel, name);
}

SoapySDR::ArgInfoList SoapyMultiSDR::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFrequencyArgsInfo(direction, localChannel);
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/

void SoapyMultiSDR::setSampleRate(const int direction, const size_t channel, const double rate)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setSampleRate(direction, localChannel, rate);
}

double SoapyMultiSDR::getSampleRate(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getSampleRate(direction, localChannel);
}

std::vector<double> SoapyMultiSDR::listSampleRates(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->listSampleRates(direction, localChannel);
}

/*******************************************************************
 * Bandwidth API
 ******************************************************************/

void SoapyMultiSDR::setBandwidth(const int direction, const size_t channel, const double bw)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setBandwidth(direction, localChannel, bw);
}

double SoapyMultiSDR::getBandwidth(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getBandwidth(direction, localChannel);
}

std::vector<double> SoapyMultiSDR::listBandwidths(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->listBandwidths(direction, localChannel);
}

SoapySDR::RangeList SoapyMultiSDR::getBandwidthRange(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getBandwidthRange(direction, localChannel);
}

/*******************************************************************
 * Clocking API
 ******************************************************************/

void SoapyMultiSDR::setMasterClockRate(const double rate)
{
    for (auto device : _devices)
    {
        device->setMasterClockRate(rate);
    }
}

double SoapyMultiSDR::getMasterClockRate(void) const
{
    return _devices[0]->getMasterClockRate();
}

SoapySDR::RangeList SoapyMultiSDR::getMasterClockRates(void) const
{
    return _devices[0]->getMasterClockRates();
}

std::vector<std::string> SoapyMultiSDR::listClockSources(void) const
{
    return _devices[0]->listClockSources();
}

void SoapyMultiSDR::setClockSource(const std::string &source)
{
    for (auto device : _devices)
    {
        device->setClockSource(source);
    }
}

std::string SoapyMultiSDR::getClockSource(void) const
{
    return _devices[0]->getClockSource();
}

/*******************************************************************
 * Time API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listTimeSources(void) const
{
    return _devices[0]->listTimeSources();
}

void SoapyMultiSDR::setTimeSource(const std::string &source)
{
    for (auto device : _devices)
    {
        device->setTimeSource(source);
    }
}

std::string SoapyMultiSDR::getTimeSource(void) const
{
    return _devices[0]->getTimeSource();
}

bool SoapyMultiSDR::hasHardwareTime(const std::string &what) const
{
    return _devices[0]->hasHardwareTime(what);
}

long long SoapyMultiSDR::getHardwareTime(const std::string &what) const
{
    return _devices[0]->getHardwareTime(what);
}

void SoapyMultiSDR::setHardwareTime(const long long timeNs, const std::string &what)
{
    for (auto device : _devices)
    {
        device->setHardwareTime(timeNs, what);
    }
}

void SoapyMultiSDR::setCommandTime(const long long timeNs, const std::string &what)
{
    for (auto device : _devices)
    {
        device->setCommandTime(timeNs, what);
    }
}
