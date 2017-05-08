// Copyright (c) 2016-2017 Josh Blum
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
        SoapySDR::logf(SOAPY_SDR_INFO, "Making device %d...", int(i));
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
 * Identification API
 ******************************************************************/

std::string SoapyMultiSDR::getDriverKey(void) const
{
    std::vector<std::string> keys;
    for (auto device : _devices)
    {
        keys.push_back(device->getDriverKey());
    }
    return csvJoin(keys);
}

std::string SoapyMultiSDR::getHardwareKey(void) const
{
    std::vector<std::string> keys;
    for (auto device : _devices)
    {
        keys.push_back(device->getHardwareKey());
    }
    return csvJoin(keys);
}

SoapySDR::Kwargs SoapyMultiSDR::getHardwareInfo(void) const
{
    SoapySDR::Kwargs result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (const auto &pair : _devices[i]->getHardwareInfo())
        {
            result[toIndexedName(pair.first, i)] = pair.second;
        }
    }
    return result;
}

/*******************************************************************
 * Channels API
 ******************************************************************/

void SoapyMultiSDR::setFrontendMapping(const int direction, const std::string &mapping)
{
    const auto maps = csvSplit(mapping);
    for (size_t i = 0; i < maps.size() and i < _devices.size(); i++)
    {
        _devices[i]->setFrontendMapping(direction, maps.at(i));
    }
    this->reloadChanMaps();
}

std::string SoapyMultiSDR::getFrontendMapping(const int direction) const
{
    std::vector<std::string> maps;
    for (auto device : _devices)
    {
        maps.push_back(device->getFrontendMapping(direction));
    }
    return csvJoin(maps);
}

size_t SoapyMultiSDR::getNumChannels(const int direction) const
{
    const auto &map = (direction == SOAPY_SDR_RX)?_rxChanMap:_txChanMap;
    return map.size();
}

SoapySDR::Kwargs SoapyMultiSDR::getChannelInfo(const int direction, const size_t channel) const
{
    SoapySDR::Kwargs result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (const auto &pair : _devices[i]->getChannelInfo(direction, channel))
        {
            result[toIndexedName(pair.first, i)] = pair.second;
        }
    }
    return result;
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

bool SoapyMultiSDR::hasFrequencyCorrection(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->hasFrequencyCorrection(direction, localChannel);
}

void SoapyMultiSDR::setFrequencyCorrection(const int direction, const size_t channel, const double value)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->setFrequencyCorrection(direction, localChannel, value);
}

double SoapyMultiSDR::getFrequencyCorrection(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getFrequencyCorrection(direction, localChannel);
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

SoapySDR::RangeList SoapyMultiSDR::getSampleRateRange(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getSampleRateRange(direction, localChannel);
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
    const auto sources = csvSplit(source);
    for (size_t i = 0; i < sources.size() and i < _devices.size(); i++)
    {
        _devices[i]->setClockSource(sources.at(i));
    }
}

std::string SoapyMultiSDR::getClockSource(void) const
{
    std::vector<std::string> sources;
    for (auto device : _devices)
    {
        sources.push_back(device->getClockSource());
    }
    return csvJoin(sources);
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
    const auto sources = csvSplit(source);
    for (size_t i = 0; i < sources.size() and i < _devices.size(); i++)
    {
        _devices[i]->setTimeSource(sources.at(i));
    }
}

std::string SoapyMultiSDR::getTimeSource(void) const
{
    std::vector<std::string> sources;
    for (auto device : _devices)
    {
        sources.push_back(device->getTimeSource());
    }
    return csvJoin(sources);
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

/*******************************************************************
 * Sensor API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listSensors(void) const
{
    std::vector<std::string> result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (const auto &name : _devices[i]->listSensors())
        {
            result.push_back(toIndexedName(name, i));
        }
    }
    return result;
}

SoapySDR::ArgInfo SoapyMultiSDR::getSensorInfo(const std::string &name) const
{
    size_t index = 0;
    const auto localName = splitIndexedName(name, index);
    return _devices[index]->getSensorInfo(localName);
}

std::string SoapyMultiSDR::readSensor(const std::string &name) const
{
    size_t index = 0;
    const auto localName = splitIndexedName(name, index);
    return _devices[index]->readSensor(localName);
}

std::vector<std::string> SoapyMultiSDR::listSensors(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->listSensors(direction, localChannel);
}

SoapySDR::ArgInfo SoapyMultiSDR::getSensorInfo(const int direction, const size_t channel, const std::string &name) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getSensorInfo(direction, localChannel, name);
}

std::string SoapyMultiSDR::readSensor(const int direction, const size_t channel, const std::string &name) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->readSensor(direction, localChannel, name);
}

/*******************************************************************
 * Register API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listRegisterInterfaces(void) const
{
    std::vector<std::string> result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (const auto &name : _devices[i]->listRegisterInterfaces())
        {
            result.push_back(toIndexedName(name, i));
        }
    }
    return result;
}

void SoapyMultiSDR::writeRegister(const std::string &name, const unsigned addr, const unsigned value)
{
    size_t index = 0;
    const auto localName = splitIndexedName(name, index);
    return _devices[index]->writeRegister(localName, addr, value);
}

unsigned SoapyMultiSDR::readRegister(const std::string &name, const unsigned addr) const
{
    size_t index = 0;
    const auto localName = splitIndexedName(name, index);
    return _devices[index]->readRegister(localName, addr);
}

void SoapyMultiSDR::writeRegister(const unsigned addr, const unsigned value)
{
    return _devices[0]->writeRegister(addr, value);
}

unsigned SoapyMultiSDR::readRegister(const unsigned addr) const
{
    return _devices[0]->readRegister(addr);
}

/*******************************************************************
 * Settings API
 ******************************************************************/

SoapySDR::ArgInfoList SoapyMultiSDR::getSettingInfo(void) const
{
    SoapySDR::ArgInfoList result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (auto info : _devices[i]->getSettingInfo())
        {
            info.key = toIndexedName(info.key, i);
            info.name += " - Device" + std::to_string(i);
            result.push_back(info);
        }
    }
    return result;
}

void SoapyMultiSDR::writeSetting(const std::string &key, const std::string &value)
{
    size_t index = 0;
    const auto localKey = splitIndexedName(key, index);
    return _devices[index]->writeSetting(localKey, value);
}

std::string SoapyMultiSDR::readSetting(const std::string &key) const
{
    size_t index = 0;
    const auto localKey = splitIndexedName(key, index);
    return _devices[index]->readSetting(localKey);
}

SoapySDR::ArgInfoList SoapyMultiSDR::getSettingInfo(const int direction, const size_t channel) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->getSettingInfo(direction, localChannel);
}

void SoapyMultiSDR::writeSetting(const int direction, const size_t channel, const std::string &key, const std::string &value)
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->writeSetting(direction, localChannel, key, value);
}

std::string SoapyMultiSDR::readSetting(const int direction, const size_t channel, const std::string &key) const
{
    size_t localChannel = 0;
    auto device = this->getDevice(direction, channel, localChannel);
    return device->readSetting(direction, localChannel, key);
}

/*******************************************************************
 * GPIO API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listGPIOBanks(void) const
{
    std::vector<std::string> result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (const auto &name : _devices[i]->listGPIOBanks())
        {
            result.push_back(toIndexedName(name, i));
        }
    }
    return result;
}

void SoapyMultiSDR::writeGPIO(const std::string &bank, const unsigned value)
{
    size_t index = 0;
    const auto localBank = splitIndexedName(bank, index);
    return _devices[index]->writeGPIO(localBank, value);
}

void SoapyMultiSDR::writeGPIO(const std::string &bank, const unsigned value, const unsigned mask)
{
    size_t index = 0;
    const auto localBank = splitIndexedName(bank, index);
    return _devices[index]->writeGPIO(localBank, value, mask);
}

unsigned SoapyMultiSDR::readGPIO(const std::string &bank) const
{
    size_t index = 0;
    const auto localBank = splitIndexedName(bank, index);
    return _devices[index]->readGPIO(localBank);
}

void SoapyMultiSDR::writeGPIODir(const std::string &bank, const unsigned dir)
{
    size_t index = 0;
    const auto localBank = splitIndexedName(bank, index);
    return _devices[index]->writeGPIODir(localBank, dir);
}

void SoapyMultiSDR::writeGPIODir(const std::string &bank, const unsigned dir, const unsigned mask)
{
    size_t index = 0;
    const auto localBank = splitIndexedName(bank, index);
    return _devices[index]->writeGPIODir(localBank, dir, mask);
}

unsigned SoapyMultiSDR::readGPIODir(const std::string &bank) const
{
    size_t index = 0;
    const auto localBank = splitIndexedName(bank, index);
    return _devices[index]->readGPIODir(localBank);
}

/*******************************************************************
 * I2C API
 ******************************************************************/

void SoapyMultiSDR::writeI2C(const int addr, const std::string &data)
{
    return _devices[0]->writeI2C(addr, data);
}

std::string SoapyMultiSDR::readI2C(const int addr, const size_t numBytes)
{
    return _devices[0]->readI2C(addr, numBytes);
}

/*******************************************************************
 * SPI API
 ******************************************************************/

unsigned SoapyMultiSDR::transactSPI(const int addr, const unsigned data, const size_t numBits)
{
    return _devices[0]->transactSPI(addr, data, numBits);
}

/*******************************************************************
 * UART API
 ******************************************************************/

std::vector<std::string> SoapyMultiSDR::listUARTs(void) const
{
    std::vector<std::string> result;
    for (size_t i = 0; i < _devices.size(); i++)
    {
        for (const auto &name : _devices[i]->listUARTs())
        {
            result.push_back(toIndexedName(name, i));
        }
    }
    return result;
}

void SoapyMultiSDR::writeUART(const std::string &which, const std::string &data)
{
    size_t index = 0;
    const auto localUART = splitIndexedName(which, index);
    return _devices[index]->writeUART(localUART, data);
}

std::string SoapyMultiSDR::readUART(const std::string &which, const long timeoutUs) const
{
    size_t index = 0;
    const auto localUART = splitIndexedName(which, index);
    return _devices[index]->readUART(localUART, timeoutUs);
}
