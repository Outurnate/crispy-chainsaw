#ifndef CONFIGURATIONMANAGER_HPP
#define CONFIGURATIONMANAGER_HPP

#include <cpptoml.h>
#include <vector>
#include <variant>
#include <map>
#include <thread>
#include <memory>
#include <functional>
#include <optional>
#include <ConfigurationServer.grpc.pb.h>

class ConfigurationManager final : private cfs::ConfigurationServer::Service
{
public:
  typedef std::variant<double, std::string, bool, int> OptionValue;

  struct Option
  {
    typedef std::function<bool(const OptionValue&)> ValidationCallback;
    typedef std::function<void(const OptionValue&)> UpdateCallback;

    std::string name;
    std::string description;
    OptionValue defaultValue;
    OptionValue value;
    bool runtimeMutable;
    std::optional<OptionValue> min;
    std::optional<OptionValue> max;
    std::optional<bool> stringMustBeFromList;
    std::optional<std::vector<std::string> > stringValues;
    std::optional<ValidationCallback> validationCallback;
    std::optional<UpdateCallback> updateCallback;

    Option(
        std::string name,
        std::string description,
        OptionValue defaultValue,
        bool runtimeMutable = true,
        std::optional<UpdateCallback> updateCallback = std::nullopt,
        std::optional<OptionValue> min = std::nullopt,
        std::optional<OptionValue> max = std::nullopt,
        std::optional<bool> stringMustBeFromList = std::nullopt,
        std::optional<std::vector<std::string> > stringValues = std::nullopt,
        std::optional<ValidationCallback> validationCallback = std::nullopt)
      : name(name),
        description(description),
        defaultValue(defaultValue),
        value(defaultValue),
        runtimeMutable(runtimeMutable),
        min(min),
        max(max),
        stringMustBeFromList(stringMustBeFromList),
        stringValues(stringValues),
        validationCallback(validationCallback),
        updateCallback(updateCallback) {}

    bool validateNewValue(const OptionValue& value) const;
    void notifyUpdated();
    std::string toString() const;
    bool fromString(const std::string& str);
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
  grpc::Status GetKeys(grpc::ServerContext* context, const cfs::Empty* request, grpc::ServerWriter<cfs::Key>* writer) override;
  grpc::Status GetValue(grpc::ServerContext* context, const cfs::Key* request, cfs::Value* response) override;
  grpc::Status GetDisplayHint(grpc::ServerContext* context, const cfs::Key* request, cfs::ValueDisplayHint* response) override;
  grpc::Status SetValue(grpc::ServerContext* context, const cfs::Pair* request, cfs::Result* response) override;

  OptionSet current;
  std::shared_ptr<cpptoml::table> configFile;
  bool dirty;
  std::thread rpcThread;
};

#endif
