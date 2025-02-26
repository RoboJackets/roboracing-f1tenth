# Welcome to RoboRacing's 2025 F1tenth Season

## Team Members: 
- Eshan Jaffar: Project Manager
- Andrew Son: Software Lead
- Zane Pressley: Mechatronic Lead

Software Team: 
- Sasanka Polisetti
- Justin Gardiner
- Cipriano Dorbessan
- Daniel Thompson
- Rohan Nair
- Nathan Donagi
- Eshan Jaffar
- Andrew Son

Mechanical Team: 
- Ian Causseaux

Electronic: 
- Jackie Mac Hale
- Zane Pressley

## 2025 Notes:
### Current Simulation Installation Instructions
Link to F1tenth simulation: https://github.com/f1tenth/f1tenth_gym_ros
1. First go down to the section "Without an NVIDIA gpu" if you are on MacOS or Windows without an   NVIDIA gpu.
2. Follow instructions as in the read me (make sure to clone f1tenth_gym_ros)
3. When running the setup scripts make sure to run them in a separate terminal in the "Launching the Simulation section" 

## History:
F1-tenth + AKS autonomous racing starting 2024
previously: [software repo](https://github.com/RoboJackets/roboracing-software)

## UDP connection setup Jetson Orin Nano
1. Install netcat on Jetson if it doesn't already exist (check with command nc)
2. Run this command to set manual mode "insert manual mode command here"
3. Set static ip, preferably as 192.168.20.(some number here)
4. Run this command to send UDP packets

To test UDP commands on windows, use the following commands below (make sure netcat is installed):
1. ssh roboracing@192.168.20.2
2. ncat -ul 8888 (to catch incoming UDP commands.)

