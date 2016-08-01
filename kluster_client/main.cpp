#include <boost/program_options.hpp>

#include "kluster_client.h"

namespace po = boost::program_options;

int main(int argc, const char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()
      ("help", "produce help message")
      ("broker", po::value<std::string>(), "broker url")
      ("job", po::value<std::string>(), "path to job files")
      ("tasks", po::value<std::string>(), "path to tasks files")
      ("cmd", po::value<std::string>(), "command line to launch job");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
      std::cout << desc << "\n";
      return 1;
  }

  std::string brokerUrl = "tcp://127.0.0.1:12345";
  if (vm.count("broker"))
  {
      brokerUrl = vm["broker"].as<std::string>();
  }
  std::cout << "Broker at " << brokerUrl << std::endl;

  std::string jobPath = "job";
  if (vm.count("job"))
  {
      jobPath = vm["job"].as<std::string>();
  }
  std::cout << "Job files at " << jobPath << std::endl;

  std::string tasksPath = "tasks";
  if (vm.count("tasks"))
  {
      tasksPath = vm["tasks"].as<std::string>();
  }
  std::cout << "Tasks files at " << tasksPath << std::endl;

  std::string cmdLine;
  if (vm.count("cmd"))
  {
      cmdLine = vm["cmd"].as<std::string>();
  }
  std::cout << "Job command " << cmdLine << std::endl;

  KlusterClient kc;

  kc.Connect(brokerUrl);
  kc.SendJobRequest(jobPath, tasksPath, cmdLine);
  kc.WaitForResults();

  return 0;
}
