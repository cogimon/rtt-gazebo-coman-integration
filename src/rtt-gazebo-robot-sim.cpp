#include <rtt-gazebo-robot-sim.hpp>
#include <rtt/Operation.hpp>
#include <string>
#include <fstream>
#include <streambuf>

#include "pid_values_tmp.h"

using namespace cogimon;
using namespace RTT;
using namespace RTT::os;
using namespace Eigen;

robotSim::robotSim(const std::string &name):
    TaskContext(name),
    is_configured(false)
{
    this->provides("gazebo")->addOperation("WorldUpdateBegin",
            &robotSim::WorldUpdateBegin, this, RTT::ClientThread);
    this->provides("gazebo")->addOperation("WorldUpdateEnd",
            &robotSim::WorldUpdateEnd, this, RTT::ClientThread);

    this->addOperation("getModel", &robotSim::getModel, this, ClientThread);


    this->addOperation("setControlMode", &robotSim::setControlMode,
                this, RTT::ClientThread);
    ///

    world_begin = gazebo::event::Events::ConnectWorldUpdateBegin(
            boost::bind(&robotSim::WorldUpdateBegin, this));
    world_end = gazebo::event::Events::ConnectWorldUpdateEnd(
            boost::bind(&robotSim::WorldUpdateEnd, this));
}

bool robotSim::setControlMode(const std::string& controlMode) {

    log(Warning) << "NEEDS TO BE IMPLEMENTED!"
            << endlog();

    return true;
}

bool robotSim::getModel(const std::string& gazebo_comp_name,
        const std::string& model_name, double timeout_s) {
    if (model) {
        log(Warning) << "Model [" << model_name << "] already loaded !"
                << endlog();
        return true;
    }
    gazebo::printVersion();
    if (!gazebo::physics::get_world()) {
        log(Error) << "getWorldPtr does not seem to exists" << endlog();
        return false;
    }
    model = gazebo::physics::get_world()->GetModel(model_name);
    if (model) {
        log(Info) << "Model [" << model_name << "] successfully loaded !"
                << endlog();
        return true;
    }
    return bool(model);
}

void robotSim::updateHook() {
}

bool robotSim::configureHook() {
    this->is_configured = gazeboConfigureHook(model);
    return is_configured;
}

//Here we already have the model!
bool robotSim::gazeboConfigureHook(gazebo::physics::ModelPtr model) {
    if (model.get() == NULL) {
        RTT::log(RTT::Error) << "No model could be loaded" << RTT::endlog();
        return false;
    }

    // Get the joints
    gazebo_joints_ = model->GetJoints();
    model_links_ = model->GetLinks();

    RTT::log(RTT::Info) << "Model name "<< model->GetName() << RTT::endlog();
    RTT::log(RTT::Info) << "Model has " << gazebo_joints_.size() << " joints" << RTT::endlog();
    RTT::log(RTT::Info) << "Model has " << model_links_.size() << " links" << RTT::endlog();

    whole_robot.reset(new KinematicChain("whole_robot", *(this->ports()), model));
    if(!whole_robot->initKinematicChain()){
        RTT::log(RTT::Warning) << "Problem in initKinematicChain" << RTT::endlog();
        return false;
    }

    RTT::log(RTT::Warning) << "Done configuring component" << RTT::endlog();
    return true;
}


ORO_CREATE_COMPONENT(cogimon::robotSim)
