#ifndef __PARSER_H__
#define __PARSER_H__

#include <control_modes.h>
#include <tinyxml.h>
#include <srdfdom_advr/model.h>
#include <urdf/model.h>
#include <algorithm>

namespace cogimon {

struct parsed_words{
        static constexpr const char* robot_tag = "robot";
        static constexpr const char* rtt_gazebo_tag = "rtt-gazebo";
        static constexpr const char* controller_tag = "controller";
        static constexpr const char* reference_attribute = "reference";
        static constexpr const char* type_attribute = "type";
        static constexpr const char* gains_tag = "gains";
        static constexpr const char* P_attribute = "P";
        static constexpr const char* I_attribute = "I";
        static constexpr const char* D_attribute = "D";
        static constexpr const char* stiffness_attribute = "stiffness";
        static constexpr const char* damping_attribute = "damping";

};

struct PIDGain{
    std::string joint_name;
    double P = 0.0;
    double I = 0.0;
    double D = 0.0;
};

struct VelPIDGain{
    std::string joint_name;
    double P = 0.0;
    double I = 0.0;
    double D = 0.0;
};

struct ImpedanceGain{
    std::string joint_name;
    double stiffness = 0.0;
    double damping = 0.0;
};

struct gains{
    typedef std::string kinematic_chain;
    typedef std::vector<PIDGain> PIDGains;
    typedef std::vector<VelPIDGain> VelPIDGains;
    typedef std::vector<ImpedanceGain> ImpedanceGains;

    std::map<kinematic_chain, std::vector<std::string>> map_controllers;
    std::map<kinematic_chain, PIDGains> map_PIDGains;
    std::map<kinematic_chain, VelPIDGains> map_VelPIDGains;
    std::map<kinematic_chain, ImpedanceGains> map_ImpedanceGains;

    gains()
    {

    }

    gains(const gains& _gains)
    {
        map_controllers = _gains.map_controllers;
        map_PIDGains = _gains.map_PIDGains;
        map_VelPIDGains = _gains.map_VelPIDGains;
        map_ImpedanceGains = _gains.map_ImpedanceGains;
    }

    bool getPID(const kinematic_chain& kc, const std::string& joint_name, PIDGain& pid)
    {
        if(map_controllers.find(kc) == map_controllers.end())
            return false;

        std::vector<std::string> available_controllers = map_controllers.at(kc);
        if(std::find(available_controllers.begin(), available_controllers.end(),
                     std::string(ControlModes::JointPositionCtrl)) != available_controllers.end())
        {
            PIDGains pids= map_PIDGains.at(kc);
            for(unsigned int i = 0; i < pids.size(); ++i)
            {
                if(joint_name.compare(pids[i].joint_name) == 0)
                {
                    pid = pids[i];
                    return true;
                }
            }
            return false;
        }
        else
            return false;
    }

    bool getVelPID(const kinematic_chain& kc, const std::string& joint_name, VelPIDGain& velPid)
    {
        if(map_controllers.find(kc) == map_controllers.end())
            return false;

        std::vector<std::string> available_controllers = map_controllers.at(kc);
        if(std::find(available_controllers.begin(), available_controllers.end(),
                     std::string(ControlModes::JointVelocityCtrl)) != available_controllers.end())
        {
            VelPIDGains velPids= map_VelPIDGains.at(kc);
            for(unsigned int i = 0; i < velPids.size(); ++i)
            {
                if(joint_name.compare(velPids[i].joint_name) == 0)
                {
                    velPid = velPids[i];
                    return true;
                }
            }
            return false;
        }
        else
            return false;
    }

    bool getImpedance(const kinematic_chain& kc, const std::string& joint_name,
                             ImpedanceGain& impedance)
    {
        if(map_controllers.find(kc) == map_controllers.end())
            return false;

        std::vector<std::string> available_controllers = map_controllers.at(kc);
        if(std::find(available_controllers.begin(), available_controllers.end(),
                     std::string(ControlModes::JointImpedanceCtrl)) != available_controllers.end())
        {
            ImpedanceGains impedances= map_ImpedanceGains.at(kc);
            for(unsigned int i = 0; i < impedances.size(); ++i)
            {
                if(joint_name.compare(impedances[i].joint_name) == 0)
                {
                    impedance = impedances[i];
                    return true;
                }
            }
            return false;
        }
        else
            return false;
    }
};

class gain_parser{
public:
    gain_parser();

    bool initFile(const std::string& filename);
    void printGains();

    gains Gains;
private:
    boost::shared_ptr<TiXmlDocument> _doc;



};

}

#endif
