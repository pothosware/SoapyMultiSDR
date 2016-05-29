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
