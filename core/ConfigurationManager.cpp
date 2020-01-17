#include "ConfigurationManager.hpp"

#include <grpcpp/grpcpp.h>
#include <type_traits>
#include <boost/lexical_cast.hpp>

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
    {
      if (stringMustBeFromList && stringMustBeFromList.value() && stringValues)
        return std::find(stringValues->begin(), stringValues->end(), arg) != stringValues->end();
      else
        return true;
    }
    else if constexpr (std::is_same_v<T, bool>)
      return true;
    else
      static_assert(always_false<T>::value, "non-exhaustive visitor!");
  }, value);

  return v;
}

void ConfigurationManager::Option::notifyUpdated()
{
  if (updateCallback)
    updateCallback.value()(value);
}

std::string ConfigurationManager::Option::toString() const
{
  return std::visit([](auto&& arg)
  {
    return boost::lexical_cast<std::string>(std::decay_t<decltype(arg)>(arg));
  }, defaultValue);
}

bool ConfigurationManager::Option::fromString(const std::string& str)
{
  try
  {
    OptionValue newValue = std::visit([&str](auto&& arg)
    {
      return OptionValue(boost::lexical_cast<std::decay_t<decltype(arg)>>(str));
    }, defaultValue);

    if (validateNewValue(newValue))
    {
      value = newValue;
      notifyUpdated();
      return true;
    }
    else
      return false;
  }
  catch (boost::bad_lexical_cast const&)
  {
    return false;
  }
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

grpc::Status ConfigurationManager::GetKeys(grpc::ServerContext* context, const cfs::Empty* request, grpc::ServerWriter<cfs::Key>* writer)
{
  (void)context;
  (void)request;

  for (auto& option : current.options)
  {
    cfs::Key key;
    key.set_name(option.second.name);
    writer->Write(key);
  }

  return grpc::Status::OK;
}

grpc::Status ConfigurationManager::GetValue(grpc::ServerContext* context, const cfs::Key* request, cfs::Value* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  if (mapValue == current.options.end())
    response->set_status(cfs::KEYNOTFOUND);
  else
  {
    response->set_status(cfs::SUCCESS);
    response->set_value(mapValue->second.toString());
  }

  return grpc::Status::OK;
}

grpc::Status ConfigurationManager::GetDisplayHint(grpc::ServerContext* context, const cfs::Key* request, cfs::ValueDisplayHint* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  if (mapValue == current.options.end())
    response->set_status(cfs::KEYNOTFOUND);
  else
  {
    Option& option = mapValue->second;

    response->set_status(cfs::SUCCESS);
    response->set_runtimemutable(option.runtimeMutable);
    response->set_description(option.description);

    std::visit([&option, &response](auto&& arg)
    {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, int>)
      {
        cfs::IntValueDisplayHint* hint = new cfs::IntValueDisplayHint;
        if (option.max)
          hint->set_max(std::get<int>(option.max.value()));
        if (option.min)
          hint->set_min(std::get<int>(option.min.value()));
        response->set_allocated_intdisplayhint(hint);
      }
      else if constexpr (std::is_same_v<T, double>)
      {
        cfs::RealValueDisplayHint* hint = new cfs::RealValueDisplayHint;
        if (option.max)
          hint->set_max(std::get<double>(option.max.value()));
        if (option.min)
          hint->set_min(std::get<double>(option.min.value()));
        response->set_allocated_realdisplayhint(hint);
      }
      else if constexpr (std::is_same_v<T, std::string>)
      {
        cfs::StringValueDisplayHint* hint = new cfs::StringValueDisplayHint;
        if (option.stringValues)
          for (const std::string& val : option.stringValues.value())
            hint->add_values(val);
        if (option.stringMustBeFromList)
          hint->set_mustbeoneof(option.stringMustBeFromList.value());
        response->set_allocated_stringdisplayhint(hint);
      }
      else if constexpr (std::is_same_v<T, bool>)
      {
        cfs::BoolValueDisplayHint* hint = new cfs::BoolValueDisplayHint;
        response->set_allocated_booldisplayhint(hint);
      }
      else
        static_assert(always_false<T>::value, "non-exhaustive visitor!");
    }, option.value);
  }

  return grpc::Status::OK;
}

grpc::Status ConfigurationManager::SetValue(grpc::ServerContext* context, const cfs::Pair* request, cfs::Result* response)
{
  (void)context;

  auto mapValue = current.options.find(request->name());
  if (mapValue == current.options.end())
    response->set_status(cfs::KEYNOTFOUND);
  else
  {
    if (mapValue->second.fromString(request->value()))
      response->set_status(cfs::SUCCESS);
    else
      response->set_status(cfs::VALUEINVALID);
  }

  return grpc::Status::OK;
}
