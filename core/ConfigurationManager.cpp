#include "ConfigurationManager.hpp"

#include <grpcpp/grpcpp.h>
#include <fstream>
#include <type_traits>

template<class T> struct always_false : std::false_type {};

bool ConfigurationManager::Option::validateNewValue(const ConfigurationManager::OptionValue& value) const
{
  if (validationCallback)
    return validationCallback.value()(value);

  auto v = std::visit([&](auto&& arg)
  {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int>)
    {
      if (this->min && this->max)
      {
        const int min = std::get<int>(this->min.value());
        const int max = std::get<int>(this->max.value());
        return min < arg && arg < max;
      }
      else
        return true;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
      if (this->min && this->max)
      {
        const double min = std::get<double>(this->min.value());
        const double max = std::get<double>(this->max.value());
        return min < arg && arg < max;
      }
      else
        return true;
    }
    else if constexpr (std::is_same_v<T, std::string>)
      return true;
    else if constexpr (std::is_same_v<T, bool>)
      return true;
    else
      static_assert(always_false<T>::value, "non-exhaustive visitor!");
  }, value);

  return v;
}

void ConfigurationManager::Option::notifyUpdated()
{
}

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
    dirty(false),
    rpcThread([&]()
  {
    std::string serverAddress("0.0.0.0:50051");

    grpc::ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(this);
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    server->Wait();
  })
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

cfs::SettingType getType(const ConfigurationManager::OptionValue& option)
{
  return std::visit([](auto&& arg)
  {
    using T = std::decay_t<decltype(arg)>;
    if constexpr (std::is_same_v<T, int>)
      return cfs::SettingType::INTEGER;
    else if constexpr (std::is_same_v<T, double>)
      return cfs::SettingType::REAL;
    else if constexpr (std::is_same_v<T, std::string>)
      return cfs::SettingType::STRING;
    else if constexpr (std::is_same_v<T, bool>)
      return cfs::SettingType::BOOL;
    else
      static_assert(always_false<T>::value, "non-exhaustive visitor!");
  }, option);
}

std::optional<bool> validateValue(const ConfigurationManager::Option& option, const cfs::SettingValue& newValue)
{
  if (newValue.value_case() == cfs::SettingValue::VALUE_NOT_SET)
    return std::nullopt;

  switch(getType(option.value))
  {
  case cfs::SettingType::BOOL:
    if (newValue.value_case() != cfs::SettingValue::kBoolValue)
      return std::nullopt;
    else
      return option.validateNewValue(newValue.boolvalue());
    break;
  case cfs::SettingType::INTEGER:
    if (newValue.value_case() != cfs::SettingValue::kIntValue)
      return std::nullopt;
    else
      return option.validateNewValue(newValue.intvalue());
    break;
  case cfs::SettingType::REAL:
    if (newValue.value_case() != cfs::SettingValue::kRealValue)
      return std::nullopt;
    else
      return option.validateNewValue(newValue.realvalue());
    break;
  case cfs::SettingType::STRING:
    if (newValue.value_case() != cfs::SettingValue::kStringValue)
      return std::nullopt;
    else
      return option.validateNewValue(newValue.stringvalue());
    break;
  }
}

grpc::Status ConfigurationManager::ListSettings(grpc::ServerContext* context, const cfs::Empty* request, grpc::ServerWriter<cfs::SettingDescription>* writer)
{
  (void)context;
  (void)request;

  for (auto& option : current.options)
  {
    cfs::SettingDescription description;
    description.set_name(option.second.name);
    writer->Write(description);
  }

  return grpc::Status::OK;
}

grpc::Status ConfigurationManager::GetDetails(grpc::ServerContext* context, const cfs::SettingDescription* request, cfs::SettingDetails* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  bool doesOptionExist = mapValue != current.options.end();

  if (!doesOptionExist)
    return grpc::Status::CANCELLED;

  response->set_type(getType(mapValue->second.value));

  return grpc::Status::OK;
}

grpc::Status ConfigurationManager::ValidateValue(grpc::ServerContext* context, const cfs::SettingValue* request, cfs::Result* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  bool doesOptionExist = mapValue != current.options.end();

  if (!doesOptionExist)
    return grpc::Status::CANCELLED;

  std::optional<bool> success = validateValue(mapValue->second, *request);

  if (!success)
    return grpc::Status::CANCELLED;
  else
  {
    response->set_success(success.value());
    return grpc::Status::OK;
  }
}

grpc::Status ConfigurationManager::SetSetting(grpc::ServerContext* context, const cfs::SettingValue* request, cfs::Result* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  bool doesOptionExist = mapValue != current.options.end();

  if (!doesOptionExist)
    return grpc::Status::CANCELLED;
}

grpc::Status ConfigurationManager::GetSetting(grpc::ServerContext* context, const cfs::SettingDescription* request, cfs::SettingValue* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  bool doesOptionExist = mapValue != current.options.end();

  if (!doesOptionExist)
    return grpc::Status::CANCELLED;

  switch(getType(mapValue->second.value))
  {
  case cfs::SettingType::BOOL:
    response->set_boolvalue(std::get<bool>(mapValue->second.value));
    return grpc::Status::OK;

  case cfs::SettingType::INTEGER:
    response->set_intvalue(std::get<int>(mapValue->second.value));
    return grpc::Status::OK;

  case cfs::SettingType::REAL:
    response->set_realvalue(std::get<double>(mapValue->second.value));
    return grpc::Status::OK;

  case cfs::SettingType::STRING:
    response->set_stringvalue(std::get<std::string>(mapValue->second.value));
    return grpc::Status::OK;

  default:
    return grpc::Status::CANCELLED;
  }
}

grpc::Status ConfigurationManager::ValidValues(grpc::ServerContext* context, const cfs::SettingDescription* request, grpc::ServerWriter<cfs::SettingValue>* writer)
{
  (void)context;
}
