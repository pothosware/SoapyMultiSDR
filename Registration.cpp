// Copyright (c) 2016-2016 Josh Blum
// SPDX-License-Identifier: BSL-1.0

#include "SoapyMultiSDR.hpp"
#include <SoapySDR/Registry.hpp>

//! Use this magic stop key in the server to prevent infinite loops
#define SOAPY_MULTI_KWARG_STOP "soapy_multi_no_deeper"

//! Use this key prefix to pass in args that will become local
#define SOAPY_MULTI_KWARG_PREFIX "multi:"

/***********************************************************************
 * Args translator for nested keywords
 **********************************************************************/
static SoapySDR::Kwargs translateArgs(const SoapySDR::Kwargs &args, const size_t index)
{
    SoapySDR::Kwargs argsOut;

    //stop infinite loops with special keyword
    argsOut[SOAPY_MULTI_KWARG_STOP] = "";

    //copy all non-multi keys
    for (auto &pair : args)
    {
        if (isIndexedName(pair.first)) continue;
        if (pair.first == "driver") continue; //don't propagate local driver filter
        if (pair.first == "type") continue; //don't propagate local sub-type filter
        if (pair.first.find(SOAPY_MULTI_KWARG_PREFIX) == std::string::npos)
        {
            argsOut[pair.first] = pair.second;
        }
    }

    //write all multi keys with prefix stripped
    for (auto &pair : args)
    {
        if (pair.first.find(SOAPY_MULTI_KWARG_PREFIX) == 0)
        {
            static const size_t offset = std::string(SOAPY_MULTI_KWARG_PREFIX).size();
            argsOut[pair.first.substr(offset)] = pair.second;
        }
    }

    //when indexed, copy in by stripped key when index matches
    //drop when the index does not match current index
    for (auto &pair : args)
    {
        if (not isIndexedName(pair.first)) continue;

        size_t splitIndex = 0;
        const auto name = splitIndexedName(pair.first, splitIndex);
        if (splitIndex == index) argsOut[name] = pair.second;
    }

    return argsOut;
}

static std::vector<SoapySDR::Kwargs> translateArgs(const SoapySDR::Kwargs &args)
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

    //translate for each specific index
    for (size_t index = 0; index <= size_t(maxIndex); index++)
    {
        result.push_back(translateArgs(args, index));
    }

    return result;
}

/***********************************************************************
 * Discovery routine -- find acceptable multi-devices
 * Because single devices instances will be discoverable normally
 * this routine should only yield results when specifically invoked.
 **********************************************************************/
static std::vector<SoapySDR::Kwargs> findMultiSDR(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> result;

    //split args into indexes for each device
    const auto &argses = translateArgs(args);
    if (argses.empty()) return result;

    //gather results at a specific device index
    //TODO handle multiple results
    SoapySDR::Kwargs result0;
    for (size_t index = 0; index < argses.size(); index++)
    {
        const auto args_i = argses.at(index);
        const auto results_i = SoapySDR::Device::enumerate(args_i);
        if (results_i.empty()) return result; //nothing for this index
        for (const auto &resultArgs : results_i.front())
        {
            result0[toIndexedName(resultArgs.first, index)] = resultArgs.second;
        }
    }
    result.push_back(result0);

    //remove instances of the stop key from the result
    for (auto &resultArgs : result)
    {
        resultArgs.erase(SOAPY_MULTI_KWARG_STOP);
        if (resultArgs.count("driver") != 0)
        {
            resultArgs["multi:driver"] = resultArgs.at("driver");
            resultArgs.erase("driver");
        }
        if (resultArgs.count("type") != 0)
        {
            resultArgs["multi:type"] = resultArgs.at("type");
            resultArgs.erase("type");
        }
    }

    return result;
}

/***********************************************************************
 * Factory routine -- create a device with multiple internal handles.
 **********************************************************************/
static SoapySDR::Device *makeMultiSDR(const SoapySDR::Kwargs &args)
{
    if (args.count(SOAPY_MULTI_KWARG_STOP) != 0) //probably wont happen
    {
        throw std::runtime_error("makeMultiSDR() -- factory loop");
    }

    //split args into indexes for each device
    const auto &argses = translateArgs(args);
    if (argses.empty()) throw std::runtime_error("makeMultiSDR() -- no indexed args");

    return new SoapyMultiSDR(argses);
}

/***********************************************************************
 * Registration
 **********************************************************************/
static SoapySDR::Registry registerRemote("multi", &findMultiSDR, &makeMultiSDR, SOAPY_SDR_ABI_VERSION);
