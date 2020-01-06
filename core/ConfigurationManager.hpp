#ifndef CONFIGURATIONMANAGER_HPP
#define CONFIGURATIONMANAGER_HPP

#include <cpptoml.h>
#include <variant>
#include <map>

class ConfigurationManager
{
public:
  typedef std::variant<double, std::string, bool> OptionValue;

  struct Option
  {
    std::string name;
    OptionValue defaultValue;
    OptionValue value;

    Option(const std::string& name, const OptionValue& defaultValue)
      : name(name),
        defaultValue(defaultValue),
        value(defaultValue) {}
  };

  struct OptionSet
  {
    friend class ConfigurationManager;

  public:
    void registerOption(Option option);
  private:
    std::map<std::string, Option> options;
  };

  ConfigurationManager(OptionSet options);

  void reset(const std::string& name);
  void flush();

  template<typename T>
  const T& get(const std::string& name) const
  {
    return std::get<T>(current.options.at(name).value);
  }

  template<typename T>
  void set(const std::string& name, const T& value, bool flush = false)
  {
    current.options.at(name).value = value;
    dirty = true;

    if (flush)
      this->flush();
  }
private:
  OptionSet current;
  std::shared_ptr<cpptoml::table> configFile;
  bool dirty;
};

#endif