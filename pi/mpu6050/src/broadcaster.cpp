#include "mpu6050.hpp"
#include "calibration.hpp"
#include "utility.hpp"
#include "kalman.hpp"
#include "attitude.hpp"

#include <boost/program_options.hpp>
#include <json/json.h>
#include <nanomsg/nn.h>
#include <nanomsg/pair.h>

#include <sstream>

using namespace std::chrono;
namespace po = boost::program_options;

int main(int argc, const char* argv[]) {
  const auto OPT_HELP = "help";
  const auto OPT_CALIBRATION = "calibration";
  const auto OPT_URI = "uri";
  const auto OPT_CLIENT = "client";
  const auto OPT_KALMANFILTER = "kalmanfilter";
  const auto OPT_SAMPLERATE = "samplerate";

  auto samplerate = 100;

  po::options_description desc("Allowed options");
  desc.add_options()
    (OPT_HELP, "produce help message")
    (OPT_CALIBRATION, po::value<std::string>(), "calibration settings")
    (OPT_URI, po::value<std::string>(), "nanomsg uri to serve data on")
    (OPT_CLIENT, "run as client instead of server")
    (OPT_KALMANFILTER, po::value<std::string>(), "add kalman filter, parametrized through the passed json-file")
    (OPT_SAMPLERATE, po::value<int>(&samplerate), "samplerate in hz")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  assert(vm.count(OPT_URI));
  const auto uri = vm[OPT_URI].as<std::string>();
  const auto runAsClient = vm.count(OPT_CLIENT) > 0;

  if(vm.count(OPT_HELP)) {
    std::cout << desc << "\n";
    return 1;
  }

  I2CBus bus(1);
  MPU6050 mpu(bus, MPU6050::GYRO_SENSITIVITY::DEG1000PS);

  std::function<IMUData()> pipeline = [&bus, &mpu]() {
    return mpu.sample(bus);
  };

  IMUCalibration calibration;
  if(vm.count(OPT_CALIBRATION)) {
    calibration = IMUCalibration::fromJsonFile(
	vm[OPT_CALIBRATION].as<std::string>()
    );
    pipeline = [&calibration, pipeline]() {
      return calibration(pipeline());
    };
  }

  std::unique_ptr<IMUKalmanFilter> pKalmanFilter;
  if(vm.count(OPT_KALMANFILTER)) {
    auto configFile = vm[OPT_KALMANFILTER].as<std::string>();
    assert(configFile != "");
    printf("%s\n", configFile.c_str());
    pKalmanFilter = decltype(pKalmanFilter)(
	new IMUKalmanFilter(configFile)
    );
    pipeline = [pipeline, &pKalmanFilter]() {
      return (*pKalmanFilter)(pipeline());
    };
  }

  AttitudeFilter attitude;
  pipeline = [&attitude, pipeline]() {
    return attitude(pipeline());
  };

  Json::FastWriter writer;
  int socket = nn_socket (AF_SP, NN_PAIR);
  if(runAsClient) {
    assert (nn_connect (socket, uri.c_str()) >= 0);
  } else {
    assert (nn_bind (socket, uri.c_str()) >= 0);
  }

  auto start_time = std::chrono::steady_clock::now();
  const std::chrono::duration<double, std::chrono::seconds::period> sampleperiod (1.0 / double(samplerate));
  auto next = start_time + sampleperiod;

  while(true) {
    const auto sample = pipeline();
    if(std::chrono::steady_clock::now() >= next) {
      const auto document = writer.write(imuToJson(sample, start_time));
      nn_send (socket, document.data(), document.size(), 0);
      next += sampleperiod;
    }
  }

  return 0;
}
