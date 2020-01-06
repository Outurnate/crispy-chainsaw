#include "ConfigurationManager.hpp"

#include <fstream>

void ConfigurationManager::OptionSet::registerOption(ConfigurationManager::Option option)
{
  options.insert(std::pair(option.name, option));
}

std::string locateConfigFile()
{
  return "config.toml";
}

ConfigurationManager::ConfigurationManager(ConfigurationManager::OptionSet options)
  : current(options),
    configFile(cpptoml::parse_file(locateConfigFile())),
    dirty(false)
{
  for (auto& optionDefinition : current.options)
    std::visit([&optionDefinition, this](auto&& arg)
    {
      using T = std::decay_t<decltype(arg)>;
      optionDefinition.second.value = configFile->get_as<T>(optionDefinition.first).value_or(arg);
    }, optionDefinition.second.defaultValue);
}

void ConfigurationManager::reset(const std::string& name)
{
  current.options.at(name).value = current.options.at(name).defaultValue;
}

void ConfigurationManager::flush()
{
  std::shared_ptr<cpptoml::table> newConfig = cpptoml::make_table();

  for (auto& optionDefinition : current.options)
    std::visit([&optionDefinition, &newConfig, this](auto&& arg)
    {
      newConfig->insert(optionDefinition.first, arg);
    }, optionDefinition.second.value);

  std::ofstream configStream;
  configStream.open(locateConfigFile());
  configStream << *newConfig;
  configStream.close();

  configFile = newConfig;

  dirty = false;
}
