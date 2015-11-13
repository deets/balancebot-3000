#include "mpu6050.hpp"
#include "calibration.hpp"
#include "utility.hpp"
#include "attitude.hpp"

#include <boost/program_options.hpp>
#include <json/json.h>

#include <sstream>
#include <iostream>

using namespace std::chrono;
namespace po = boost::program_options;

int main(int argc, const char* argv[]) {
  const auto OPT_HELP = "help";
  const auto OPT_CALIBRATION = "calibration";
  const auto OPT_SAMPLETIME = "sampletime";

  auto seconds = 10;

  po::options_description desc("Allowed options");
  desc.add_options()
    (OPT_HELP, "produce help message")
    (OPT_CALIBRATION, po::value<std::string>(), "calibration settings")
    (OPT_SAMPLETIME, po::value<int>(), "sampletime in seconds")
    ;

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if(vm.count(OPT_HELP)) {
    std::cout << desc << "\n";
    return 1;
  }

  IMUCalibration calibration;
  if(vm.count(OPT_CALIBRATION)) {
    calibration = IMUCalibration::fromJsonFile(
	vm[OPT_CALIBRATION].as<std::string>()
    );
  }

  if(vm.count(OPT_SAMPLETIME)) {
    seconds = vm[OPT_SAMPLETIME].as<int>();
  }

  I2CBus bus(1);
  MPU6050 mpu(bus, MPU6050::GYRO_SENSITIVITY::DEG1000PS);


  std::vector<IMUData> samples;

  AttitudeFilter attitude;

  auto until = steady_clock::now() + std::chrono::seconds(seconds);

  while(steady_clock::now() < until) {
    samples.push_back(attitude(calibration(mpu.sample(bus))));
  }

  Json::Value res(Json::arrayValue);
  res.resize(samples.size());

  auto start_time = samples[0].timestamp;

  for(decltype(samples)::size_type i = 0; i < samples.size(); i++) {
    res[i] = imuToJson(samples[i], start_time);
  }

  Json::FastWriter writer;
  const auto document = writer.write(res);
  std::cout << document;
  return 0;
}
