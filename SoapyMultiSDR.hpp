// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include "MultiNameUtils.hpp"
#include <SoapySDR/Device.hpp>
#include <utility> //pair
#include <vector>

class SoapyMultiSDR : public SoapySDR::Device
{
public:
    SoapyMultiSDR(const std::vector<SoapySDR::Kwargs> &args);
    ~SoapyMultiSDR(void);

    /*******************************************************************
     * Channels API
     ******************************************************************/

    void setFrontendMapping(const int direction, const std::string &mapping);

    std::string getFrontendMapping(const int direction) const;

    size_t getNumChannels(const int direction) const;

    bool getFullDuplex(const int direction, const size_t channel) const;

private:

    //! Get the internal device pointer given the channel and direction
    SoapySDR::Device *getDevice(const int direction, const size_t channel, size_t &localChannel) const
    {
        const auto &map = (direction == SOAPY_SDR_RX)?_rxChanMap:_txChanMap;
        const auto &pair = map.at(channel);
        localChannel = pair.first;
        return pair.second;
    }

    //internal devices mapped by device index
    std::vector<SoapySDR::Device *> _devices;

    //mapping of channel index to internal device pointer
    void reloadChanMaps(void);
    std::vector<std::pair<size_t, SoapySDR::Device *>> _rxChanMap;
    std::vector<std::pair<size_t, SoapySDR::Device *>> _txChanMap;
};
