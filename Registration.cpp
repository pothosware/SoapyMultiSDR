// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SoapyMultiSDR.hpp"
#include <SoapySDR/Registry.hpp>

/***********************************************************************
 * Discovery routine -- find acceptable multi-devices
 * Because single devices instances will be discoverable normally
 * this routine should only yield results when specifically invoked.
 **********************************************************************/
static std::vector<SoapySDR::Kwargs> findMultiSDR(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> result;

    //find the maximum index
    int maxIndex = -1;
    for (const auto &pair : args)
    {
        if (not isIndexedName(pair.first)) continue;
        size_t index = 0; splitIndexedName(pair.first, index);
        if (int(index) > maxIndex) maxIndex = index;
    }

    //no indexed arguments specified
    if (maxIndex < 0) return result;

    //gather results at a specific device index
    for (size_t index = 0; index <= size_t(maxIndex); index++)
    {
        
    }

    return result;
}

/***********************************************************************
 * Factory routine -- create a device with multiple internal handles.
 **********************************************************************/
static SoapySDR::Device *makeMultiSDR(const SoapySDR::Kwargs &args)
{
    return new SoapyMultiSDR(args);
}

/***********************************************************************
 * Registration
 **********************************************************************/
static SoapySDR::Registry registerRemote("multi", &findMultiSDR, &makeMultiSDR, SOAPY_SDR_ABI_VERSION);
