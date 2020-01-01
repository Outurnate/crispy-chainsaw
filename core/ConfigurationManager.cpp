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
  {
    switch(optionDefinition.second.optionType)
    {
    case OptionType::Bool:
      optionDefinition.second.value = configFile->get_as<bool>(optionDefinition.first).value_or(std::get<bool>(optionDefinition.second.defaultValue));
      break;

    case OptionType::String:
      optionDefinition.second.value = configFile->get_as<std::string>(optionDefinition.first).value_or(std::get<std::string>(optionDefinition.second.defaultValue));
      break;

    case OptionType::Real:
      optionDefinition.second.value = configFile->get_as<double>(optionDefinition.first).value_or(std::get<double>(optionDefinition.second.defaultValue));
      break;
    }
  }
}

void ConfigurationManager::reset(const std::string& name)
{
  current.options.at(name).value = current.options.at(name).defaultValue;
}

void ConfigurationManager::flush()
{
  std::shared_ptr<cpptoml::table> newConfig = cpptoml::make_table();

  for (auto& optionDefinition : current.options)
  {
    switch(optionDefinition.second.optionType)
    {
    case OptionType::Bool:
      newConfig->insert(optionDefinition.first, std::get<bool>(optionDefinition.second.value));
      break;

    case OptionType::String:
      newConfig->insert(optionDefinition.first, std::get<std::string>(optionDefinition.second.value));
      break;

    case OptionType::Real:
      newConfig->insert(optionDefinition.first, std::get<double>(optionDefinition.second.value));
      break;
    }
  }

  std::ofstream configStream;
  configStream.open(locateConfigFile());
  configStream << *newConfig;
  configStream.close();

  configFile = newConfig;

  dirty = false;
}
