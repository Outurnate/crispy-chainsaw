#ifndef CONFIGURATIONMANAGER_HPP
#define CONFIGURATIONMANAGER_HPP

#include <cpptoml.h>
#include <variant>
#include <map>
#include <thread>
#include <memory>
#include <functional>
#include <optional>
#include <ConfigurationServer.grpc.pb.h>

class ConfigurationManager final : public cfs::ConfigurationServer::Service
{
public:
  typedef std::variant<double, std::string, bool, int> OptionValue;

  struct Option
  {
    typedef std::function<bool(const OptionValue&)> ValidationCallback;
    typedef std::function<void(const OptionValue&)> UpdateCallback;

    std::string name;
    OptionValue defaultValue;
    OptionValue value;
    std::optional<OptionValue> min;
    std::optional<OptionValue> max;
    std::optional<ValidationCallback> validationCallback;
    std::optional<UpdateCallback> updateCallback;

    Option(std::string name, OptionValue defaultValue, std::optional<UpdateCallback> updateCallback = std::nullopt, std::optional<OptionValue> min = std::nullopt, std::optional<OptionValue> max = std::nullopt, std::optional<ValidationCallback> validationCallback = std::nullopt)
      : name(name),
        defaultValue(defaultValue),
        value(defaultValue),
        updateCallback(updateCallback),
        min(min),
        max(max),
        validationCallback(validationCallback) {}

    bool validateNewValue(const OptionValue& value) const;
    void notifyUpdated();
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

  grpc::Status ListSettings(grpc::ServerContext* context, const cfs::Empty* request, grpc::ServerWriter<cfs::SettingDescription>* writer) override;
  grpc::Status GetDetails(grpc::ServerContext* context, const cfs::SettingDescription* request, cfs::SettingDetails* response) override;
  grpc::Status ValidateValue(grpc::ServerContext* context, const cfs::SettingValue* request, cfs::Result* response) override;
  grpc::Status SetSetting(grpc::ServerContext* context, const cfs::SettingValue* request, cfs::Result* response) override;
  grpc::Status GetSetting(grpc::ServerContext* context, const cfs::SettingDescription* request, cfs::SettingValue* response) override;
  grpc::Status ValidValues(grpc::ServerContext* context, const cfs::SettingDescription* request, grpc::ServerWriter<cfs::SettingValue>* writer) override;
private:
  OptionSet current;
  std::shared_ptr<cpptoml::table> configFile;
  bool dirty;
  std::thread rpcThread;
};

#endif
