// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#pragma once
#include <SoapySDR/Device.hpp>
#include <vector>

class SoapyMultiSDR : public SoapySDR::Device
{
public:
    SoapyMultiSDR(const SoapySDR::Kwargs &args);
    ~SoapyMultiSDR(void);

private:

    //! Get the internal device pointer given the channel and direction
    SoapySDR::Device *getDevice(const int direction, const size_t channel) const
    {
        switch (direction)
        {
        case SOAPY_SDR_RX: return _rxDeviceMap.at(channel);
        case SOAPY_SDR_TX: return _txDeviceMap.at(channel);
        }
        return nullptr;
    }

    //mapping of channel index to internal device pointer
    std::vector<SoapySDR::Device *> _rxDeviceMap;
    std::vector<SoapySDR::Device *> _txDeviceMap;
};
